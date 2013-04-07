/*
    Andromeda
    Copyright (C) 2011  Steven van der Schoot

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * \AddToGroup paging
 * @{
 */

#include <math/math.h>
#include <mm/memory.h>

double
abs(double num)
{
	return (num>0)?num:-num;
}

float
absf(float num)
{
	return (num>0)?num:-num;
}

long double
absl(long double num)
{
	return (num>0)?num:-num;
}

unsigned int seedTwo = 1; // Store the seeds
unsigned int seedOne = 1;

void
randomize(unsigned int s) // Create the seed
{
	if (s!=0)
	{
	seedTwo = s;
	seedOne = s/2;
	if (seedOne == 0)
	{
		seedOne = 0x0BADB002;
	}
	}
	else
	{
		seedTwo = 0xDEADBEEF;
		seedOne = 0xCAFEBABE;
	}
}

int
randomA() // Hash the stuff and make it random.
{
	// Doesn't need to be thread safe for extra randomness.
	if (seedTwo == 0)
	{
		seedTwo = 1;
	}
	if (seedOne == 0)
	{
		seedOne = 1;
	}
	int ret1 = 36969 * (seedOne & 0xFFFF) + (seedTwo >> 16);
	int ret2 = 18000 * (seedTwo & 0xFFFF) + (seedOne >> 16);

	int ret = (ret1 << 16) + ret2;

	seedOne = ret1; // Set the seeding values
	seedTwo = ret2;

	return ret;
}

int
random()
{
	int ret = randomA();
	ret %= RANDMAX;
	ret += RANDMIN;

	if (ret < 0)
	{
		ret *= -1;
	}
	return ret;
}

#define PI (3.141592653589793) //238462643383279502884197169399375105820974944592307816406286209 Yes, I know the compiler will skip most of the decimals...

#if 0
/*
 * These functions are bad, real bad.
 */
double
NAN()
{
	double nan;
	*((float*)&nan) = 0x7f800000;
	return nan;
}

float
NANf()
{
	float nan;
	*((int*)&nan) = 0x7f800000;
	return nan;
}

long double
NANl()
{
	long double nan;
	*((int*)&nan) = 0x7f800000;
	return nan;
}
#endif

inline char
isNaN(double num)
{
	return (num != num);
}

inline char
isNaNf(float num)
{
	return (num != num);
}

inline char
isNaNl(long double num)
{
	return (num != num);
}

double
sin(double x)
{
	double ret = x,
	       x2  = x*x,
	       lastRet = x;
	int i = 2;
	for (;i<0xffffff;i+=2)
	{
		lastRet = -lastRet * ( x2 / (i*(i+1)) );
		if ( -0.0000000001<lastRet && lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

float
sinf(float x)
{
	float ret = x,
	      x2  = x*x,
	      lastRet = x;
	int i = 2;
	for (;i<0xffffff;i+=2)
	{
		lastRet = -lastRet * ( x2 / (i*(i+1)) );
		if ( -0.0000000001<lastRet && lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

long double
sinl(long double x)
{
	long double ret = x,
	            x2  = x*x,
	            lastRet = x;
	int i = 2;
	for (;i<0xffffff;i+=2)
	{
		lastRet = -lastRet * ( x2 / (i*(i+1)) );
		if ( -0.0000000001<lastRet && lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

double
asin(double x)
{
	double ret = x,
	       x2  = x*x,
	       lastRet = x;
	int i = 2;
	for (;i<0xffffff;i+=2)
	{
		lastRet = lastRet * x2 * ( (i-1) * (i-2) / ( i * (i+1) ) );
		if ( lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

float
asinf(float x)
{
	float ret = x,
	      x2  = x*x,
	      lastRet = x;
	int i = 2;
	for (;i<0xffffff;i+=2)
	{
		lastRet = lastRet * x2 * ( (i-1) * (i-2) / ( i * (i+1) ) );
		if ( lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

long double
asinl(long double x)
{
	long double ret = x,
	            x2  = x*x,
	            lastRet = x;
	int i = 2;
	for (;i<0xffffff;i+=2)
	{
		lastRet = lastRet * x2 * ( (i-1) * (i-2) / ( i * (i+1) ) );
		if ( lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

double
sinh(double x)
{
	double tmp = exp(x);
	return (tmp - 1/tmp)/2;
}

float
sinhf(float x)
{
	float tmp = expf(x);
	return (tmp - 1/tmp)/2;
}

long double
sinhl(long double x)
{
	long double tmp = expl(x);
	return (tmp - 1/tmp)/2;
}

double
asinh(double x)
{
	return ln(x * sqrt( x*x + 1 ) );
}

float
asinhf(float x)
{
	return lnf(x * sqrtf( x*x + 1 ) );
}

long double
asinhl(long double x)
{
	return lnl(x * sqrtl( x*x + 1 ) );
}

double
cos(double x)
{
	double ret = 1,
	       x2  = x*x,
	       lastRet = -1;
	int i = 1;
	for (;i<0xffffff;i+=2)
	{
		lastRet = -lastRet * ( x2 / (i*(i+1)) );
		if ( -0.0000000001<lastRet && lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

float
cosf(float x)
{
	float ret = 1,
	      x2  = x*x,
	      lastRet = -1;
	int i = 1;
	for (;i<0xffffff;i+=2)
	{
		lastRet = -lastRet * ( x2 / (i*(i+1)) );
		if ( -0.0000000001<lastRet && lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

long double
cosl(long double x)
{
	long double ret = 1,
	            x2  = x*x,
	            lastRet = -1;
	int i = 1;
	for (;i<0xffffff;i+=2)
	{
		lastRet = -lastRet * ( x2 / (i*(i+1)) );
		if ( -0.0000000001<lastRet && lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

double
acos(double x)
{
	return asin(x) * PI/2;
}

float
acosf(float x)
{
	return asinf(x) * PI/2;
}

long double
acosl(long double x)
{
	return asinl(x) * PI/2;
}

double
cosh(double x)
{
	double tmp = exp(x);
	return (tmp + 1/tmp)/2;
}

float
coshf(float x)
{
	float tmp = expf(x);
	return (tmp + 1/tmp)/2;
}

long double
coshl(long double x)
{
	long double tmp = expl(x);
	return (tmp + 1/tmp)/2;
}

double
acosh(double x)
{
	return ln(x * sqrt( x*x - 1 ) );
}

float
acoshf(float x)
{
	return lnf(x * sqrtf( x*x - 1 ) );
}

long double
acoshl(long double x)
{
	return lnl(x * sqrtl( x*x - 1 ) );
}

double
tan(double x)
{
	return sin(x)/cos(x);
}

float
tanf(float x)
{
	return sinf(x)/cosf(x);
}

long double
tanl(long double x)
{
	return sinl(x)/cosl(x);
}

double
atan(double x)
{
	double ret = x,
	       x2  = x*x,
	       lastRet = x;
	int i = 3;
	for (;i<0xffffff;i+=2)
	{
		lastRet = -lastRet * ( x2 * (i-2) / i );
		if ( -0.0000000001<lastRet && lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

float
atanf(float x)
{
	float ret = x,
	      x2  = x*x,
	      lastRet = x;
	int i = 3;
	for (;i<0xffffff;i+=2)
	{
		lastRet = -lastRet * ( x2 * (i-2) / i );
		if ( -0.0000000001<lastRet && lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

long double
atanl(long double x)
{
	long double ret = x,
	            x2  = x*x,
	            lastRet = x;
	int i = 3;
	for (;i<0xffffff;i+=2)
	{
		lastRet = -lastRet * ( x2 * (i-2) / i );
		if ( -0.0000000001<lastRet && lastRet < 0.0000000001 )
			return ret;
		ret += lastRet;
	}
	return ret;
}

double
tanh(double x)
{
	return sinh(x)/cosh(x);
}

float
tanhf(float x)
{
	return sinhf(x)/coshf(x);
}

long double
tanhl(long double x)
{
	return sinhl(x)/coshl(x);
}

#if 0
/*
 * Uses some realy bad functions.
 */
double
atanh(double x)
{
	if (abs(x)>=1)
		return NAN();
	return ln( (1+x) / (1-x) )/2;
}

float
atanhf(float x)
{
	if (absf(x)>=1)
		return NANf();
	return lnf( (1+x) / (1-x) )/2;
}

long double
atanhl(long double x)
{
	if (absl(x)>=1)
		return NANl();
	return lnl( (1+x) / (1-x) )/2;
}
#endif

double
exp(double n) //An other beatyfull taylor polynominal
{
	double ret = 1;
	double x = n;
	int fac = 1, i = 2;
	for (; (fac<0xffffffff) ;i++) // stop if i is insane high
	{
		ret += x/i;  // ret += (x^i)/(i!)
		if ( (x>(0xffffffff/n)) || ( x==(x*n) ) ) // stop if x != relyable anymore
			break;
		fac *= i;  // making it (i+1)!
	}
	return ret;
}

float
expf(float n) //An other beatyfull taylor polynominal
{
	float ret = 1;
	float x = n;
	int fac = 1, i = 2;
	for (; (fac<0xffffffff) ;i++) // stop if i is insane high
	{
		ret += x/i;  // ret += (x^i)/(i!)
		if ( (x>(0xffffffff/n)) || ( x==(x*n) ) ) // stop if x != relyable anymore
			break;
		fac *= i;  // making it (i+1)!
	}
	return ret;
}

long double
expl(long double n) //An other beatyfull taylor polynominal
{
	long double ret = 1;
	long double x = n;
	int fac = 1, i = 2;
	for (; (fac<0xffffffff) ;i++) // stop if i is insane high
	{
		ret += x/i;  // ret += (x^i)/(i!)
		if ( (x>(0xffffffff/n)) || ( x==(x*n) ) ) // stop if x != relyable anymore
			break;
		fac *= i;  // making it (i+1)!
	}
	return ret;
}

double
ln(double x)
{
	if (x < 0)
		return -1; // e^x will never be less than 0
	double ret = x,
	       tmp = -x;
	int i = 1;
	x--; // actualy we calculate ln (1 + x)
	for (;i<0xffffffff;i++)
	{
		ret += tmp/i;
		if ( tmp==(tmp*-x) || ( (tmp>0)?(tmp*-x<tmp):(tmp*-x>tmp) ) ) // stop if tmp != relyable anymore
			break;
		tmp*=-x;
	}
	return ret;
}

float
lnf(float x)
{
	if (x < 0)
		return -1; // e^x will never be less than 0
	float ret = x,
	       tmp = -x;
	int i = 1;
	x--; // actualy we calculate ln (1 + x)
	for (;i<0xffffffff;i++)
	{
		ret += tmp/i;
		if ( tmp==(tmp*-x) || ( (tmp>0)?(tmp*-x<tmp):(tmp*-x>tmp) ) ) // stop if tmp != relyable anymore
			break;
		tmp*=-x;
	}
	return ret;
}

long double
lnl(long double x)
{
	if (x < 0)
		return -1; // e^x will never be less than 0
	long double ret = x,
	            tmp = -x;
	int i = 1;
	x--; // actualy we calculate ln (1 + x)
	for (;i<0xffffffff;i++)
	{
		ret += tmp/i;
		if ( tmp==(tmp*-x) || ( (tmp>0)?(tmp*-x<tmp):(tmp*-x>tmp) ) ) // stop if tmp != relyable anymore
			break;
		tmp*=-x;
	}
	return ret;
}

double
sqrt(double x)
{
	return exp(2*ln(x));
}

float
sqrtf(float x)
{
	return expf(2*ln(x));
}

long double
sqrtl(long double x)
{
	return expl(2*ln(x));
}

double
pow(double x, double exponent)
{
	double ret = 1;
	for (;exponent>=1;exponent--)
	{
		ret *= x;
	}
	if (exponent!=0) // i.a.w.: if exp was not a integer
		ret *= exp(ln(x)*exponent);
	return ret;
}

float
powf(float x, float exponent)
{
	float ret = 1;
	for (;exponent>=1;exponent--)
	{
		ret *= x;
	}
	if (exponent!=0) // i.a.w.: if exp was not a integer
		ret *= exp(ln(x)*exponent);
	return ret;
}

long double
powl(long double x, long double exponent)
{
	long double ret = 1;
	for (;exponent>=1;exponent--)
	{
		ret *= x;
	}
	if (exponent!=0) // i.a.w.: if exp was not a integer
		ret *= exp(ln(x)*exponent);
	return ret;
}

int log2i(int x)
{
        int ret = 0;
        while((x&( ((unsigned int)-1)<<ret )))
        {
                if(ret==sizeof(int)*8-1)
                        return sizeof(int)*8-1;
                ret++;
        }
        return ret-1;
}

unsigned int log2ui(unsigned int x)
{
        unsigned int ret = 0;
        while((x&( ((unsigned int)-1)<<ret )))
        {
                if(ret==sizeof(int)*8-1)
                        return sizeof(int)*8-1;
                ret++;
        }
        return ret-1;
}

/**
 * @} \file
 */
