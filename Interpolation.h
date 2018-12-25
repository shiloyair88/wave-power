#pragma once
#include <math.h>
#define min(a,b) ((a)>(b)?(b):(a))
#define max(a,b) ((a)<(b)?(b):(a))
//com numner operators
struct complex
{
	double r, i;
	complex(double r = 0, double i = 0)
	{
		this->r = r;
		this->i = i;
	}
	complex operator +(complex b)
	{
		complex re;
		re.r = this->r + b.r;
		re.i = this->i + b.i;
		return re;
	}
	complex& operator=(complex a)
	{
		this->r = a.r;
		this->i = a.i;
		return *this;
	}
	complex operator -()
	{
		return complex(-this->r, -this->i);
	}
	complex operator -(complex a)
	{
		return *this + (-a);
	}
	complex operator *()
	{
		return complex(this->r, -this->i);
	}
	complex operator *(complex a)
	{
		return complex(this->r*a.r - this->i*a.i, this->r*a.i + this->i*a.r);
	}
	double len2()
	{
		return this->r*this->r + this->i*this->i;
	}
	complex operator /(double a)
	{
		return complex(this->r / a, this->i / a);
	}
	complex operator /(complex a)
	{
		return *this*(*a) / a.len2();
	}
	int operator ==(complex a)
	{
		return r == a.r && i == a.i;
	}
	int operator !=(complex a)
	{
		return r != a.r || i != a.i;
	}
	double len()
	{
		return sqrt(len2());
	}
};
//coordinates
struct cordinate
{
	complex x;
	complex y;
};
//point struct in the space
struct point
{
	double x, y, z, time;
	int qc;
	complex output;	
};
struct stat_and_same_point
{
	point* p;
	double statics;
};
//operators on vector
struct vector
{
	complex zero;
	complex *array;
	size_t lenght = 0;
	complex& operator[](size_t x)
	{
		if (x >= lenght)
		{
			zero = 0;
			return zero;
		}
		return array[x];
	}
	vector(size_t len = 0)
	{
		if(lenght = len)array = new complex[len];
		for (size_t i = 0; i < len; i++)
		{
			array[i] = 0;
		}
		zero = 0;
	}
	complex operator*(vector a)
	{
		size_t len = min(lenght, a.lenght);
		complex re = 0;
		for (size_t i = 0; i < len; i++)
		{
			re = re + array[i] * (*a.array[i]);
		}
		return re;
	}
	vector operator+(vector a)
	{
		vector re = vector(max(lenght, a.lenght));
		for (size_t i = 0; i < re.lenght; i++)
		{
			re.array[i] = (*this)[i] + a[i];
		}
		return re;
	}
	vector operator-(vector a)
	{
		vector re = vector(max(lenght, a.lenght));
		for (size_t i = 0; i < re.lenght; i++)
		{
			re.array[i] = (*this)[i] - a[i];
		}
		return re;
	}
	complex operator|(vector a)
	{
		return (*this*a) / (a*a);
	}
	vector operator*(complex a)
	{
		vector re = vector(lenght);
		for (size_t i = 0; i < lenght; i++)
		{
			re.array[i] = array[i] * a;
		}
		return re;
	}
	vector(const vector &a)
	{
		zero = a.zero;
		if(lenght = a.lenght)array = new complex[lenght];
		for (size_t i = 0; i < lenght; i++)
		{
			array[i] = a.array[i];
		}
	}
	vector& operator=(vector a)
	{
		if (lenght == a.lenght)for (size_t i = 0; i < lenght; i++)
		{
			array[i] = a.array[i];
		}
		else
		{
			lenght = a.lenght;
			if (lenght)array = new complex[lenght];
			for (size_t i = 0; i < lenght; i++)
			{
				array[i] = a.array[i];
			}
		}
		zero = a.zero;
		return *this;
	}
	~vector()
	{
		if (lenght)delete [] array;
	}
	vector divide(vector a, vector *reminder)
	{
		*reminder = *this;
		int nzero = a.lenght - 1;
		for (; nzero >= 0 && a[nzero] == 0; nzero--);
		if (nzero < 0)return vector(0);
		vector ret = vector(lenght - nzero);
		for (size_t i = ret.lenght - 1; i < ret.lenght; i--)
		{
			ret[i] = (*reminder)[i + nzero] / a[nzero];
			for (size_t j = 0; j <= nzero; j++)
			{
				(*reminder)[i + j] = (*reminder)[i + j] - ret[i] * a[j];
			}
		}
		return ret;
	}
};

void Display_the_results_on_the_screen(complex a[], point* points, int len,  double time);
complex new_point(double x, double y, double time, complex* coefficients, int len);
vector trinom(point* arr, int len, int deg);
complex clac_point(double x, double y, double time, vector trynom);
vector ftransform(point * points, int len, int deg = -1, int ec = 0);