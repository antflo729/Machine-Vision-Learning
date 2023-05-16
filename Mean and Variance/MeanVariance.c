#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define ROWS		100
#define COLUMNS		100

int main( int argc, char **argv )
{

	int			i,x,y,k,z;
	FILE			*fp;
	float			u[4], var[4];
	unsigned char	image[ROWS][COLUMNS];
  	char			*ifile, *ofile, ch;
	char			filename[4][50];

	strcpy(filename[0], "image1.raw");
	strcpy(filename[1], "image2.raw");
	strcpy(filename[2], "image3.raw");
	strcpy(filename[3], "image4.raw");

	for (z = 0; z < 4; z++)
	{
		u[z] = 0;
		var[z] = 0;
	}
	
	//Assign each image name in filename to ifile here	
	/* example: ifile = filename[k]; k=0,1,2,3; a loop might be needed*/
	for (k = 0; k < 4; k++)
	{
		ifile = filename[k];

		if ((fp = fopen(ifile, "rb")) == NULL)
		{
			fprintf(stderr, "error: couldn't open %s\n", ifile);
			exit(1);
		}

		for (i = 0; i < ROWS; i++)
			if (fread(image[i], 1, COLUMNS, fp) != COLUMNS)
			{
				fprintf(stderr, "error: couldn't read enough stuff\n");
				exit(1);
			}

		fclose(fp);

		//Calculate Mean for each image here
		for (x = 0; x < ROWS; x++)
		{
			for (y = 0; y < COLUMNS; y++)
			{
				i = image[x][y];
				u[k] += (1 / (100.0 * 100.0)) * i;

			}
		}
		for (x = 0; x < ROWS; x++)
		{
			for (y = 0; y < COLUMNS; y++)
			{
				var[k] += (1 / ((100.0 * 100.0) - 1)) * ((image[x][y] - u[k]) * (image[x][y] - u[k]));

			}
		}
	}
		//Calculate Variance for each image here

		//Print mean and variance for each image
		
		printf("%s: %f %f\n", filename[0], u[0], var[0]);
		printf("%s: %f %f\n", filename[1], u[1], var[1]);
		printf("%s: %f %f\n", filename[2], u[2], var[2]);
		printf("%s: %f %f\n", filename[3], u[3], var[3]);
		//printf("Press any key to exit: ");
		//gets(&ch);

		return 0;
	
}


