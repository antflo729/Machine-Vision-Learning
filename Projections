#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROWS   		 		 128
#define COLS				 128
#define LOGICAL_X_MIN			-4.0
#define LOGICAL_X_MAX			 4.0
#define LOGICAL_Y_MIN			-4.0
#define LOGICAL_Y_MAX			 4.0

void clear(unsigned char image[][COLS]);
int plot_logical_point(float x, float y, unsigned char image[][COLS]);
int plot_physical_point(int x, int y, unsigned char image[][COLS]);
int in_range(int x, int y);
void header(int row, int col, unsigned char head[32]);/*write header for the output images, do not modify this*/
float generateXY(int probnum, float t, float* x, float* y, float* x1, float* y1, float* z, float* f);


int main(int argc, char **argv)
{
	int				i, ortho;
	float			t, x, y, x1, y1, z, f, xprime, yprime, xprime1, yprime1;
	FILE			*fp;
	char			 *ofile;
	unsigned char	image[ROWS][COLS];
	unsigned char head[32];

	int probnum = 3; //Hw1 problem number 1-3
	ortho = 2;  //orthogonal for 1 and any other number for perspective projection

	clear(image);

	for (t = 0.01; t < 10000.01; t += 0.01)  //change the t range
	{
		/*Modify the coefficiences according to questions*/
		generateXY(probnum, t, &x, &y, &x1, &y1, &z, &f);

		/* Modify these according to the projection */
		if (ortho == 1)
		{
			xprime = x;			/* this is an example of orth */
			yprime = y;
			xprime1 = x1;			/* this is an example of orth */
			yprime1 = y1;
			//printf("xprime1 %f \n", xprime1);
			//printf("yprime1 %f \n", yprime1);
		}
		else
		{
			xprime = (f * x) / z;
			yprime = (f * y) / z;
			xprime1 = (f * x1) / z;
			yprime1 = (f * y1) / z;
			
		}

		plot_logical_point(xprime, yprime, image);
		if (probnum == 2 || probnum ==3)
		{
			plot_logical_point(xprime1, yprime1, image);
		}
		
	}
	//_CRT_SECURE_NO_WARNINGS
	/* Create a header */ 
	header(ROWS, COLS, head);

	ofile = "hw1ex1.ras";// this is the name of your output file, modify the name according to the requirements

	if (!(fp = fopen(ofile, "wb")))
		fprintf(stderr, "error: could not open %s\n", ofile), exit(1);

	/* Write the header */
	fwrite(head, 4, 8, fp);

	for (i = 0; i < ROWS; i++) fwrite(image[i], 1, COLS, fp);
	fclose(fp);

	return 0;
}

float generateXY(int probnum, float t, float* x, float* y, float* x1, float* y1, float* z, float* f)
{
	float xo, yo, zo, zo1, zo2, xo1, yo1, a, b, c;
	if (probnum == 1)
	{
		xo = 0.5;
		yo = -1.0;
		zo = 0.0;
		a = 0.0;
		b = 1.0;
		c = -1.0;
		*f = 1.0;
		*x = xo + t * a;
		*y = yo + t * b;
		*z = zo + t * c;


		return;
	}
	else if (probnum == 2)
	{
		xo = 0.5;
		xo1 = -0.5;
		yo = -1.0;
		yo1 = -1.0;
		zo = -1.0;
		zo1 = -2.0;
		zo2 = -3.0;
		a = 1.0;
		b = 1.0;
		*f = 1.0;
		*x = xo + t * a;
		*y = yo + t * b;
		*x1 = xo1 + t * a;
		*y1 = yo1 + t * b;
		//*z = zo; //for first z
		//*z = zo1; //for second z
		*z = zo2; //for third z
		return;
	}
	else if (probnum == 3)
	{
		xo = -1.0;
		xo1 = 1.0;
		yo = -1.0;
		zo = 0.0;
		//b = 0.0;	//first b
		//b = 1.0;	//second b
		b = -1.0; //third b
		//c = 1.0;	//first c
		c = -1.0; //second c
		*f = 1.0;
		*x = xo;
		*y = yo + t * b;
		*x1 = xo1;
		*y1 = yo + t * b;
		*z = zo + t * c;
	}
	else
		printf("Invalid problem number");
		
}

void clear(unsigned char image[][COLS])
{
	int	i, j;
	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++) image[i][j] = 0;
}

int plot_logical_point(float x, float y, unsigned char image[][COLS])
{
	int	nx, ny;
	float	xc, yc;
	xc = COLS / ((float)LOGICAL_X_MAX - LOGICAL_X_MIN);
	yc = ROWS / ((float)LOGICAL_Y_MAX - LOGICAL_Y_MIN);
	nx = (x - LOGICAL_X_MIN) * xc;
	ny = (y - LOGICAL_Y_MIN) * yc;
	return plot_physical_point(nx, ny, image);
}

int plot_physical_point(int x, int y, unsigned char image[][COLS])
{
	if (in_range(x, y)) //return 0;
	return image[y][x] = 255;
}
int in_range(int x, int y)
{
	return x >= 0 && x < COLS && y >= 0 && y < ROWS;
}

void header(int row, int col, unsigned char head[32])
{
	int *p = (int *)head;
	char *ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify the contents */
	
	/* Little-endian for PC */
	*p = 0x956aa659;
	*(p + 3) = 0x08000000;
	*(p + 5) = 0x01000000;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8000000;

	ch = (char*)&col;
	head[7] = *ch;
	ch++;
	head[6] = *ch;
	ch++;
	head[5] = *ch;
	ch++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch++;
	head[10] = *ch;
	ch++;
	head[9] = *ch;
	ch++;
	head[8] = *ch;

	ch = (char*)&num;
	head[19] = *ch;
	ch++;
	head[18] = *ch;
	ch++;
	head[17] = *ch;
	ch++;
	head[16] = *ch;

	/*
	// Big-endian for unix
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
