/* 
 * File:   matrix_invert.h
 * Author: iliya
 *
 * Created on 5 Октябрь 2011 г., 19:11
 */

#ifndef MATRIX_INVERT_H
#define	MATRIX_INVERT_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "DebugPrint.h"

#define eps 1e-32

void I(double* Mat, int numCol, int i, int j);
void II(double* Mat, int numCol, int i, double l, int begin);
void III(double* Mat, int numCol, int i, int j, double l, int begin);
int invert(double* Mat, double* Res, int num);
void swap(double* a, double* b);
int mult(double* Mat0, int numStr0, int numCol0, double* Mat1, int numStr1,
		int numCol1, double* Res);
int sub_mult(double* Mat0, int numStr0, int numCol0, double* Mat1, int numStr1,
		int numCol1, double* Res);
void subb(double* b0, double* b1, int n);
double norm(double* Mat, int num);
double normb(double* b, int n);

void I(double* Mat, int numCol, int i, int j)
{
	int k;
	double *pi, *pj;
	for (pi = Mat + numCol * i, pj = Mat + numCol * j, k = 0; k < numCol; k++)
		swap(pi++, pj++);
}

void II(double* Mat, int numCol, int i, double l, int begin)
{
	int k;
	double *p;
	for (p = Mat + numCol * i + begin, k = begin; k < numCol; k++)
		*(p++) /= l;
}

void III(double* Mat, int numCol, int i, int j, double l, int begin)
{
	int k;
	double *pi, *pj;
	for (pi = Mat + numCol * i + begin, pj = Mat + numCol * j + begin, k = begin;
			k < numCol; k++)
		*(pi++) -= *(pj++) * l;
}

int invert(double* Mat, double* Res, int num)
{
	int i, j, step, imax;
	double *p, o, tmp;
	for (p = Res, i = 0; i < num; i++)
	{
		for (j = 0; j < num; j++, p++)
		{
			if (i != j)
				*p = 0.;
			else
				*p = 1.;
		}
	}

	for (step = 0; step < num; step++)
	{
		for (imax = step, tmp = 0., p = Mat + step, i = step; i < num; i++)
		{
			if (fabs(p[num * i]) > tmp)
			{
				tmp = fabs(p[num * i]);
				imax = i;
			}
		}
		if (tmp < eps)
		{
			return 0;
		}
		if (imax > step)
		{
			I(Mat, num, step, imax);
			I(Res, num, step, imax);
		}
		o = Mat[num * step + step];
		II(Mat, num, step, o, step + 1);
		Mat[num * step + step] = 1.;
		II(Res, num, step, o, 0);
		for (p = Mat + step, i = step + 1; i < num; i++)
		{
			o = p[num * i];
			III(Mat, num, i, step, o, step + 1);
			III(Res, num, i, step, o, 0);
		}
	}
	for (step = 1; step < num; step++)
	{
		for (p = Mat + step, i = 0; i < step; i++)
		{
			o = p[num * i];
			III(Mat, num, i, step, o, step + 1);
			III(Res, num, i, step, o, 0);
		}
	}
	return 1;
}

void swap(double* a, double* b)
{
	double sw;
	sw = *a;
	*a = *b;
	*b = sw;
}

int mult(double* Mat0, int numStr0, int numCol0, double* Mat1, int numStr1,
		int numCol1, double* Res)
{
	int i, j, t;
	double s00 = 0., s01 = 0., s10 = 0., s11 = 0.;

	if (numCol0 != numStr1)
	{
		ERROR_PRINT("Error. Multiplication unavailable.");
		return 1;
	}

	for (i = 0; i < numStr0 - 1; i += 2)
		for (j = 0; j < numCol1 - 1; j += 2)
		{
			s00 = s01 = s10 = s11 = 0.;
			for (t = 0; t < numCol0; t++)
			{
				s00 += Mat0[numCol0 * i + t] * Mat1[numCol1 * t + j];
				s01 += Mat0[numCol0 * i + t] * Mat1[numCol1 * t + j + 1];
				s10 += Mat0[numCol0 * i + t + numCol0] * Mat1[numCol1 * t + j];
				s11 += Mat0[numCol0 * i + t + numCol0]
						* Mat1[numCol1 * t + j + 1];
			}
			Res[numCol1 * i + j] = s00;
			Res[numCol1 * i + j + 1] = s01;
			Res[numCol1 * i + j + numCol1] = s10;
			Res[numCol1 * i + j + numCol1 + 1] = s11;
		}
	if ((numStr0 & 1) != 0)
	{
		for (i = 0; i < numCol1 - 1; i += 2)
		{
			s00 = 0.;
			s01 = 0.;
			for (t = 0; t < numCol0; t++)
			{
				s00 += Mat0[numStr0 * numCol0 - numCol0 + t]
						* Mat1[numCol1 * t + i];
				s01 += Mat0[numStr0 * numCol0 - numCol0 + t]
						* Mat1[numCol1 * t + i + 1];
			}
			Res[numStr0 * numCol1 - numCol1 + i] = s00;
			Res[numStr0 * numCol1 - numCol1 + i + 1] = s01;
		}
	}
	if ((numCol1 & 1) != 0)
	{
		for (i = 0; i < numStr0 - 1; i += 2)
		{
			s00 = 0;
			s10 = 0;
			for (t = 0; t < numCol0; t++)
			{
				s00 += Mat0[numCol0 * i + t] * Mat1[numCol1 * t + numCol1 - 1];
				s10 += Mat0[numCol0 * i + numCol0 + t]
						* Mat1[numCol1 * t + numCol1 - 1];
			}
			Res[i * numCol1 + numCol1 - 1] = s00;
			Res[i * numCol1 + numCol1 + numCol1 - 1] = s10;
		}
	}
	if (((numStr0 & numCol1) & 1) != 0)
	{
		s00 = 0;
		for (t = 0; t < numCol0; t++)
			s00 += Mat0[numStr0 * numCol0 - numCol0 + t]
					* Mat1[numCol1 * t + numCol1 - 1];
		Res[numStr0 * numCol1 - 1] = s00;
	}
	return 0;
}

int sub_mult(double* Mat0, int numStr0, int numCol0, double* Mat1, int numStr1,
		int numCol1, double* Res)
{
	int i, j, t;
	double s00 = 0., s01 = 0., s10 = 0., s11 = 0.;

	if (numCol0 != numStr1)
	{
		ERROR_PRINT("Error. Multiplication unavailable.");
		return 1;
	}

	for (i = 0; i < numStr0 - 1; i += 2)
		for (j = 0; j < numCol1 - 1; j += 2)
		{
			s00 = s01 = s10 = s11 = 0.;
			for (t = 0; t < numCol0; t++)
			{
				s00 += Mat0[numCol0 * i + t] * Mat1[numCol1 * t + j];
				s01 += Mat0[numCol0 * i + t] * Mat1[numCol1 * t + j + 1];
				s10 += Mat0[numCol0 * i + t + numCol0] * Mat1[numCol1 * t + j];
				s11 += Mat0[numCol0 * i + t + numCol0]
						* Mat1[numCol1 * t + j + 1];
			}
			Res[numCol1 * i + j] -= s00;
			Res[numCol1 * i + j + 1] -= s01;
			Res[numCol1 * i + j + numCol1] -= s10;
			Res[numCol1 * i + j + numCol1 + 1] -= s11;
		}
	if ((numStr0 & 1) != 0)
	{
		for (i = 0; i < numCol1 - 1; i += 2)
		{
			s00 = 0.;
			s01 = 0.;
			for (t = 0; t < numCol0; t++)
			{
				s00 += Mat0[numStr0 * numCol0 - numCol0 + t]
						* Mat1[numCol1 * t + i];
				s01 += Mat0[numStr0 * numCol0 - numCol0 + t]
						* Mat1[numCol1 * t + i + 1];
			}
			Res[numStr0 * numCol1 - numCol1 + i] -= s00;
			Res[numStr0 * numCol1 - numCol1 + i + 1] -= s01;
		}
	}
	if ((numCol1 & 1) != 0)
	{
		for (i = 0; i < numStr0 - 1; i += 2)
		{
			s00 = 0;
			s10 = 0;
			for (t = 0; t < numCol0; t++)
			{
				s00 += Mat0[numCol0 * i + t] * Mat1[numCol1 * t + numCol1 - 1];
				s10 += Mat0[numCol0 * i + numCol0 + t]
						* Mat1[numCol1 * t + numCol1 - 1];
			}
			Res[i * numCol1 + numCol1 - 1] -= s00;
			Res[i * numCol1 + numCol1 + numCol1 - 1] -= s10;
		}
	}
	if (((numStr0 & numCol1) & 1) != 0)
	{
		s00 = 0;
		for (t = 0; t < numCol0; t++)
			s00 += Mat0[numStr0 * numCol0 - numCol0 + t]
					* Mat1[numCol1 * t + numCol1 - 1];
		Res[numStr0 * numCol1 - 1] -= s00;
	}
	return 0;
}

void subb(double* b0, double* b1, int n)
{
	int i;
	for (i = 0; i < n; i++)
		b0[i] -= b1[i];
}

double norm(double* Mat, int num)
{
	int i, j;
	double tmp, max = 0., *p0, *p1, s0 = 0., s1 = 0., s2 = 0., s3 = 0.;

	if (num % 2 == 0)
	{
		for (i = 0; i < num - 1; i += 2)
		{
			s0 = 0.;
			s1 = 0.;
			s2 = 0.;
			s3 = 0.;
			p0 = Mat + num * i;
			p1 = Mat + num * i + num;
			for (j = 0; j < num - 1; j += 2)
			{
				s0 += fabs(p0[j]);
				s1 += fabs(p0[j + 1]);
				s2 += fabs(p1[j]);
				s3 += fabs(p1[j + 1]);
			}
			tmp = s0 + s1;
			if (tmp > max || i == 0)
				max = tmp;
			tmp = s2 + s3;
			if (tmp > max)
				max = tmp;
			s0 = 0.;
			s1 = 0.;
			s2 = 0.;
			s3 = 0.;
		}
	}
	else
	{
		for (i = 0; i < num - 1; i += 2)
		{
			s0 = 0.;
			s1 = 0.;
			s2 = 0.;
			s3 = 0.;
			p0 = Mat + num * i;
			p1 = Mat + num * i + num;
			for (j = 0; j < num - 1; j += 2)
			{
				s0 += fabs(p0[j]);
				s1 += fabs(p0[j + 1]);
				s2 += fabs(p1[j]);
				s3 += fabs(p1[j + 1]);
			}
			tmp = s0 + s1 + fabs(p0[j]);
			if (tmp > max || i == 0)
				max = tmp;
			tmp = s2 + s3 + fabs(p1[j]);
			if (tmp > max)
				max = tmp;
			s0 = 0.;
			s1 = 0.;
			s2 = 0.;
			s3 = 0.;
		}
		s0 = 0.;
		s1 = 0.;
		s2 = 0.;
		s3 = 0.;
		p0 = Mat + num * i;
		for (j = 0; j < num - 1; j += 2)
		{
			s0 += fabs(p0[j]);
			s1 += fabs(p0[j + 1]);
		}
		tmp = s0 + s1 + fabs(p0[j]);
		if (tmp > max)
			max = tmp;
		s0 = 0.;
		s1 = 0.;
	}
	return max;
}

double normb(double* b, int n)
{
	int i;
	double tmp, max;
	for (i = 0, max = 0.; i < n; i++)
	{
		tmp = fabs(b[i]);
		if (tmp > max)
			max = tmp;
	}
	return max;

}

#undef eps

#endif	/* MATRIX_INVERT_H */

