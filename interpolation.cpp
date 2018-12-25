#include "stdafx.h"
//#include "bitmap_image.hpp"
#include <iostream>
#include <iomanip>
using namespace std;
#include "interpolation.h"
//operatiom for matrices ranking
inline void swapline(double* matrix, int size, int l1, int l2)
{
	for (int ls1 = size * l1, ls2 = size * l2, end = ls1 + size; ls1<end; ls1++, ls2++)
	{
		double temp = matrix[ls1];
		matrix[ls1] = matrix[ls2];
		matrix[ls2] = temp;
	}
}
//operatiom for matrices ranking
inline void muline(double* matrix, int size, int line, double val)
{
	for (int ls = size * line, end = ls + size; ls<end; ls++)
	{
		matrix[ls] *= val;
	}
}
//operatiom for row matrices ranking
inline void subline(double* matrix, int size, int l1, int l2, double val)
{
	for (int ls1 = size * l1, ls2 = size * l2, end = ls1 + size; ls1<end; ls1++, ls2++)
	{
		matrix[ls2] -= matrix[ls1] * val;
	}
}
//Find a inverse matrix using matrices ranking
double invers1(double* out, double* in, int size)
{
	int sp2 = size * size, s1 = size + 1;
	double* temp = (double*)malloc(sp2 * sizeof(double));
	double val = 0;
	double det = 1;
	for (int i = 0; i<sp2; i++)
	{
		temp[i] = in[i];
		out[i] = !(i % (s1));
	}
	for (int i = 0; i<size; i++)
	{
		if (temp[i*s1] == 0)for (int j = i + 1; j<size; j++)if (temp[j*size + i] != 0)
		{
			swapline(temp, size, i, j);
			swapline(out, size, i, j);
			break;
		}
		if (temp[i*s1] == 0)return 0;
		val = 1 / temp[i*s1];
		muline(out, size, i, val);
		muline(temp, size, i, val);
		det *= val;
		for (int j = i + 1; j<size; j++)
		{
			val = temp[j*size + i];
			subline(out, size, i, j, val);
			subline(temp, size, i, j, val);
		}
	}
	for (int i = 1; i<size; i++)for (int j = 0; j<i; j++)
	{
		val = temp[size*j + i];
		subline(out, size, i, j, val);
		subline(temp, size, i, j, val);
	}
	free(temp);
	return det;
}

// create matrix for compute  the trinom
void matrix_trinom(double *matri, point* points, int len)
{
	int len2 = pow(len, 1/3.0);
	double len3 = pow(len, 1/3.0);
	if (len3>len2)
	{
		len2 = len2 + 1;
	}
	//compute matrix to trinom;
	int p = 0;
	for (int s = 0; s<len; s++)
	{
		double temp1 = points[s].x;
		double temp2 = points[s].y;
		double temp3 = points[s].time;
		p = 0;
		for (int i = 0; i<len2; i++)
		{
			temp1 = pow(points[s].x, i);
			for (int j = 0; j<len2; j++)
			{
				temp2 = pow(points[s].y, j);
				for (int t = 0; t<len2; t++)
				{
					temp3 = pow(points[s].time, t);
					matri[s*len+p] = temp1 * temp2*temp3;					
					p++;
					if (p >= len)
					{
						break;
					}
				}
				if (p >= len)
				{
					break;
				}
			}
			if (p >= len)
			{
				break;
			}
		}		
	}
	return;
}
//Multiplication of a matrix in vector 
vector mult(vector a, vector *mat)
{
	vector re = mat[0] * a[0];
	for (int i = 1; i < a.lenght; i++)
	{
		re = re + mat[i] * a[i];
	}
	return re;
}

vector* ortogonalize(vector*array, int len)
{
	vector *re = (vector *)malloc(sizeof(vector)*len);
	double *vlen = new double[len];
	for (int i = 0; i < len; i++)
	{
		re[i] = vector(len);
		for (int j = 0; j < len; j++)
		{
			re[i][j] = i == j;
		}
		vlen[i] = (array[i] * array[i]).r;
	}
	for (int i = 1; i < len; i++) if (vlen[i] != 0)
	{
		for (int j = 0; j < i; j++) if ((array[j] * array[j]).r / vlen[j] > 0.0000001)
		{
			complex tmp = array[i] | array[j];			
			vector flat = array[i] - array[j] * tmp;			
			if ((flat*flat).r < (array[i] * array[i]).r )
			{				
				array[i] = flat;
				re[i] = re[i] - re[j] * tmp;
			}
		}
		if ((array[i] * array[i]).r / vlen[i] < 0.00000001)array[i].zero = complex(1);
	}
	return re;
}
// find the best solution for set of equations with set of variables.
vector solve(vector* orto, vector* a, vector b,int len)
{
	vector re = vector(len);
	for (int i = 0; i < len; i++) if (a[i] * a[i] != 0 && a[i].zero == 0)
	{
		complex tmp = b | a[i];
		vector flat = b - a[i] * tmp;
		if ((flat*flat).r < (b*b).r)
		{
			b = flat;
			re = re + orto[i] * tmp;
		}
	}
	return re;
}
//Auxiliary function 
vector colum(int px, int py, int pt, point* arr, int len)
{
	vector re = vector(len);
	for (int i = 0; i < len; i++)
	{
		re[i] = pow(arr[i].x, px)*pow(arr[i].y, py)*pow(arr[i].time, pt);
	}
	return re;
}
//Auxiliary function
vector* matrix(point* arr, int len, int deg)
{
	int arrlen = (1 + (deg * (11 + deg * (6 + deg))) / 6), ind = 0;
	vector *re = (vector*)malloc(sizeof(vector)*arrlen);
	for (int total = 0; total <= deg; total++)for (int pt = 0; pt <= total; pt++)for (int py = 0; py <= total - pt; py++)
	{
		re[ind] = colum(total - pt - py, py, pt, arr, len);
		ind++;
	}
	return re;
}
//compute trinom fot set of points
vector trinom(point* arr, int len, int deg)
{
	int arrlen = (1 + (deg * (11 + deg * (6 + deg))) / 6);
	vector* mat = matrix(arr, len, deg);
	vector* orto = ortogonalize(mat, arrlen);	
	vector b = vector(len);
	for (int i = 0; i < len; i++)b[i] = arr[i].output;
	return solve(orto, mat, b, arrlen);
}
//compute point on the Trinom
complex clac_point(double x, double y, double time, vector trynom)
{
	complex re = 0;
	int ind = 0;
	for (int total = 0; ind < trynom.lenght; total++)for (int pt = 0; pt <= total; pt++)for (int py = 0; py <= total - pt; py++)
	{
		re = re + trynom[ind] * pow(x, total - pt - py)*pow(y, py)*pow(time, pt);
		ind++;
	}
	return re;
}
//compute polynom for set of points
vector polynom(cordinate * parray, int len, int deg, int ec = 0, int ndeg = 0)
{
	vector powerarray = vector(len), b = vector(len), *mat = new vector[deg + ndeg + 1 + 2 * ec];
	for (size_t i = 0; i < len; i++)
	{
		powerarray[i] = 1;
	}
	size_t end = max(ndeg, deg + ec);
	for (size_t pow = 0; pow <= end; pow++)
	{
		if (pow < ec)
		{
			mat[pow] = vector(len);
			for (size_t i = 0; i < len; i++)
			{
				mat[pow][i] = -powerarray[i] * parray[i].y;
			}
		}
		if (pow == ec)
		{
			for (size_t i = 0; i < len; i++)
			{
				b[i] = powerarray[i] * parray[i].y;
			}
		}
		if (pow <= ndeg)
		{
			mat[ec + ndeg - pow] = vector(len);
			for (size_t i = 0; i < len; i++)
			{
				mat[ec + ndeg - pow][i] = complex(1.0) / powerarray[i];
			}
		}
		if (pow <= deg + ec)
		{
			mat[ec + ndeg + pow] = vector(len);
			for (size_t i = 0; i < len; i++)
			{
				mat[ec + ndeg + pow][i] = powerarray[i];
			}
		}
		for (size_t i = 0; i < len; i++)
		{
			powerarray[i] = powerarray[i] * parray[i].x;
		}
	}
	vector *ort = ortogonalize(mat, deg + ndeg + 1 + 2 * ec), sol = solve(ort, mat, b, deg + ndeg + 1 + 2 * ec), err = vector(ec + 1), q = vector(deg + ndeg + 1 + ec);
	for (size_t i = 0; i < ec; i++)
	{
		err[i] = sol[i];
	}
	err[ec] = 1;
	for (size_t i = 0; i < deg + ndeg + 1 + ec; i++)
	{
		q[i] = sol[ec + i];
	}
	return q.divide(err, &b);
}
//compute Fourier Transform
vector ftransform(point * points, int len, int deg, int ec)
{
	double mint = points[0].time, maxt = mint, tlen;
	for (size_t i = 1; i < len; i++)
	{
		mint = min(mint, points[i].time);
		maxt = max(maxt, points[i].time);
	}
	tlen = maxt - mint;
	if (deg == -1)
	{
		deg = (len - 1) / 2 - ec;
	}
	cordinate *parray = new cordinate[len - 1];
	for (size_t i = 0; i < len - 1; i++)
	{
		double place = (points[i].time - mint) / tlen * 3.1415926535897932384626433 * 2;
		parray[i].x = complex(cos(place), sin(place));
		parray[i].y = points[i].output;
	}
	return polynom(parray, len - 1, deg, ec, deg)*complex(sqrt(len - 1));
}
