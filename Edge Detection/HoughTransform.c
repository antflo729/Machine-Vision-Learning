#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROWS	480
#define COLUMNS	640
#define VROWS 180
#define VCOLUMNS 400
#define PI 3.14159265358979323846

#define sqr(x)	((x)*(x))




void clear( unsigned char image[][COLUMNS] );
void header( int row, int col, unsigned char head[32] );
unsigned char voting[VROWS][VCOLUMNS];
int votingtally[VROWS][VCOLUMNS];

int main(int argc, char** argv)
{
	int				i, j, sgtemp, sgmmax = 0;
	int				dedx, dedy, sgm, localmax[3] = { 0, 0, 0 }, index[3][2] = { 0, 0, 0, 0, 0, 0 };
	int				sgm_threshold = 18, hough_threshold;
	FILE* fp;
	unsigned char	image[ROWS][COLUMNS], simage[ROWS][COLUMNS], head[32];
	char			filename[50], ifilename[50], ch;
	float           theta, rho, rad, radian[3] = { 0,0,0 };

	clear(simage);
	strcpy(filename, "image.raw");
	memset(voting, 0, sizeof(char) * VROWS * VCOLUMNS);  //180 * 400 is the suggested size of voting array
	header(ROWS, COLUMNS, head);

	/* Read in the image */
	if (!(fp = fopen(filename, "rb")))
	{
		fprintf(stderr, "error: couldn't open %s\n", argv[1]);
		exit(1);
	}

	for (i = 0; i < ROWS; i++)
		if (!(COLUMNS == fread(image[i], sizeof(char), COLUMNS, fp)))
		{
			fprintf(stderr, "error: couldn't read %s\n", argv[1]);
			exit(1);
		}
	fclose(fp);

	/* Compute SGM */
	for (i = 1; i < ROWS - 1; i++) {
		for (j = 1; j < COLUMNS - 1; j++) {
			sgtemp = ((abs(image[i - 1][j - 1] +	2 * image[i][j - 1] +	image[i + 1][j - 1]			//dx^2
						 - image[i - 1][j + 1] -	2 * image[i][j + 1] -	image[i + 1][j + 1])) * 

					  (abs(image[i - 1][j - 1] +	2 * image[i][j - 1] +	image[i + 1][j - 1]
						  -image[i - 1][j + 1] -	2 * image[i][j + 1] -	image[i + 1][j + 1]))) 
				+ 
					  ((abs(-image[i - 1][j + 1] -	2 * image[i - 1][j] -	image[i - 1][j - 1]			//dy^2
						    +image[i + 1][j + 1] +	2 * image[i + 1][j] +	image[i + 1][j - 1])) * 

					   (abs(-image[i - 1][j + 1] -	2 * image[i - 1][j] -	image[i - 1][j - 1]
							+image[i + 1][j + 1] + 2 * image[i + 1][j] +	image[i + 1][j - 1]))); //SGM
			if (sgtemp > sgmmax)
				sgmmax = sgtemp;
		}
	}
	for (i = 1; i < ROWS - 1; i++) {
		for (j = 1; j < COLUMNS - 1; j++) {
			simage[i][j] = (float)(((abs(image[i - 1][j - 1] + 2 * image[i][j - 1] + image[i + 1][j - 1]
				- image[i - 1][j + 1] - 2 * image[i][j + 1] - image[i + 1][j + 1])) * (abs(image[i - 1][j - 1] + 2 * image[i][j - 1] + image[i + 1][j - 1]
					- image[i - 1][j + 1] - 2 * image[i][j + 1] - image[i + 1][j + 1]))) + ((abs(-image[i - 1][j + 1] - 2 * image[i - 1][j] - image[i - 1][j - 1]
						+ image[i + 1][j + 1] + 2 * image[i + 1][j] + image[i + 1][j - 1])) * (abs(-image[i - 1][j + 1] - 2 * image[i - 1][j] - image[i - 1][j - 1]
							+ image[i + 1][j + 1] + 2 * image[i + 1][j] + image[i + 1][j - 1])))) / sgmmax * 255; //normalized sgm
		}
	}

	/* build up voting array */
	/* Save SGM to an image */
	strcpy(filename, "image");
	if (!(fp = fopen(strcat(filename, "-sgm.ras"), "wb")))
	{
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}
	fwrite(head, 4, 8, fp);
	for (i = 0; i < ROWS; i++)
		fwrite(simage[i], sizeof(char), COLUMNS, fp);
	fclose(fp);

	/* Compute the binary image */
	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLUMNS; j++) {
			if (simage[i][j] < sgm_threshold) {
				simage[i][j] = 0;
			}
			else
				simage[i][j] = 255;
		}
	}

	/* Save the thresholded SGM to an image */
	strcpy(filename, "image");
	if (!(fp = fopen(strcat(filename, "-binary.ras"), "wb")))
	{
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}
	fwrite(head, 4, 8, fp);
	for (i = 0; i < ROWS; i++)
		fwrite(simage[i], sizeof(char), COLUMNS, fp);
	fclose(fp);



	float rhomax = 800, rhomin = 0;

	float normalized_rho = 0;
	for (i = 0; i < ROWS; i++) {		//normalize rho from 0 to column size and create voting array
		for (j = 0; j < COLUMNS; j++) {
			if (simage[i][j] == 255)
			{
				for (theta = 0; theta < 180; theta++)
				{
					rad = (PI / 180) * (theta);
					rho = (-sin(rad) * i) + (cos(rad)*j);
					
					normalized_rho = (((rho)/(800))*200);
					votingtally[(int)((theta))][(int)(normalized_rho + 200)] += 1;

				}
			}
			
		}
		
	}
	
	int tallymax = 0;
	for (i = 0; i < VROWS; i++) {		//find max value in voting array
		for (j = 0; j < VCOLUMNS; j++) {
			
			if (tallymax < votingtally[i][j]){
				tallymax = votingtally[i][j];
			}
		}
	}

	for (i = 0; i < VROWS; i++) {		//normalized the voting array values from 0 - 255 for display
		for (j = 0; j < VCOLUMNS; j++) {
			voting[i][j] = (votingtally[i][j]/(float)tallymax) *255;
		}
	}



	/* Save original voting array to an image */
	strcpy(filename, "image");
	header(VROWS, VCOLUMNS, head);
	if (!(fp = fopen(strcat(filename, "-voting_array.ras"), "wb")))
	{
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}
	fwrite(head, 4, 8, fp);

	for (i = 0; i < VROWS; i++)
		fwrite(voting[i], sizeof(char), VCOLUMNS, fp);
	fclose(fp);

	/* Threshold the voting array */
	for (i = 0; i < VROWS; i++) {
		for (j = 0; j < VCOLUMNS; j++) {
			if (votingtally[i][j] > localmax[0])
			{
				localmax[0] = votingtally[i][j];
				index[0][0] = i; //theta max 1
				index[0][1] = j; //rho max 1
			}
		}
	}
		
	for (i = 0; i < VROWS; i++) {
		for (j = 0; j < VCOLUMNS; j++) {
			if (votingtally[i][j] > localmax[1] && abs(i - index[0][0]) > 3)
			{
				localmax[1] = votingtally[i][j];
				index[1][0] = i; //theta max 2
				index[1][1] = j; //rho max 2
			}
		}
	}
		
	for (i = 0; i < VROWS; i++) {
		for (j = 0; j < VCOLUMNS; j++) {
			if (votingtally[i][j] > localmax[2] && abs(i - index[0][0]) > 3 && abs(i - index[1][0]) > 3)
			{
				localmax[2] = votingtally[i][j];
				index[2][0] = i; //theta max 3
				index[2][1] = j; //rho max 3
			}
		}
	}
	hough_threshold = localmax[2];
		//index[k][1] -= 762;
		//radian[k] = index[k][0] * PI / 180;
		for (i = 0; i < VROWS; i++) {
			for (j = 0; j < VCOLUMNS; j++) {
				voting[i][j] = 0;
			}
		}
		for (i = 0; i < VROWS; i++) {
			for (j = 0; j < VCOLUMNS; j++) {
				if (i == index[0][0] && j == index[0][1])
					voting[i][j] = 255;
				else if (i == index[1][0] && j == index[1][1])
					voting[i][j] = 255;
				else if (i == index[2][0] && j == index[2][1])
					voting[i][j] = 255;
	
			}
		}
		

	/* Write the thresholded voting array to a new image */
	strcpy(filename, "image");
	header(VROWS, VCOLUMNS, head);
	if (!(fp = fopen(strcat(filename, "-voting_array2.ras"), "wb")))
	{
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}
	fwrite(head, 4, 8, fp);

	for (i = 0; i < VROWS; i++)
		fwrite(voting[i], sizeof(char), VCOLUMNS, fp);
	fclose(fp);


	index[0][1] = ((index[0][1] - 200.0) / 200.0) * 800;
	index[1][1] = ((index[1][1] - 200.0) / 200.0) * 800;
	index[2][1] = ((index[2][1] - 200.0) / 200.0) * 800;
	

	printf("Hough threshold: %d\n", hough_threshold);
	printf("%d %d %d\n%d %d %d\n%d %d %d\n", index[0][0], index[0][1], localmax[0],
											index[1][0], index[1][1] , localmax[1],
											index[2][0], index[2][1], localmax[2]);

	/* Reconstruct an image from the voting array */
	for (i = 0; i < ROWS; i++) {
		for (j = 0; j < COLUMNS; j++) {
			simage[i][j] = 0;
		}
	}
	
	radian[0] = (index[0][0] * (PI / 180.0)); //1.326
	radian[1] = (index[1][0] * (PI / 180.0)); //0.68
	radian[2] = (index[2][0] * (PI / 180.0)); //2.46
	
	double xint1 = (index[0][1]) / cos(radian[0]); //  216
	double xint2 = (index[1][1]) / cos(radian[1]); //  -1300
	double xint3 = (index[2][1]) / cos(radian[2]); //  386
	


	for (i = 0; i < ROWS; i++) {
		j = (int)((float)i * tan(radian[0])) + xint1; // negative slope line
		
		if (j < COLUMNS && j >= 0)
			//printf("%d\n", j);
			simage[i][j] = 255;
		j = (int)((float)i * tan(radian[1])) + xint2; //dotted line
		
		if (j < COLUMNS && j >= 0)
			simage[i][j] = 255;
		j = (int)((float)i * tan(radian[2])) + xint3;//positive slope
		//printf("%d\n", j);
		if (j < COLUMNS && j >= 0)
			simage[i][j] = 255;
	//	printf("%d \n", j);
	}


	/* Write the reconstructed figure to an image */
	strcpy(filename, "image");
	header(ROWS, COLUMNS, head);
	if (!(fp = fopen(strcat(filename, "-reconstructed_image.ras"), "wb")))
	{
		fprintf(stderr, "error: could not open %s\n", filename);
		exit(1);
	}
	fwrite(head, 4, 8, fp);
	for (i = 0; i < ROWS; i++)
		fwrite(simage[i], sizeof(char), COLUMNS, fp);
	fclose(fp);

	printf("Press any key to exit: ");
	gets(&ch);

	return 0;
}

void clear( unsigned char image[][COLUMNS] )
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLUMNS ; j++ ) 
			image[i][j] = 0;
}

void header( int row, int col, unsigned char head[32] )
{
	int *p = (int *)head;
	char *ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify this */
	/* Little-endian for PC */
	
	*p = 0x956aa659;
	*(p + 3) = 0x08000000;
	*(p + 5) = 0x01000000;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8000000;

	ch = (char*)&col;
	head[7] = *ch;
	ch ++; 
	head[6] = *ch;
	ch ++;
	head[5] = *ch;
	ch ++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch ++; 
	head[10] = *ch;
	ch ++;
	head[9] = *ch;
	ch ++;
	head[8] = *ch;
	
	ch = (char*)&num;
	head[19] = *ch;
	ch ++; 
	head[18] = *ch;
	ch ++;
	head[17] = *ch;
	ch ++;
	head[16] = *ch;
	

	/* Big-endian for unix */
	/*
	*p = 0x59a66a95;
	*(p + 1) = col;
	*(p + 2) = row;
	*(p + 3) = 0x8;
	*(p + 4) = num;
	*(p + 5) = 0x1;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8;
*/
}

