/*
    Orion OS, The educational operatingsystem
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

#include <math/complex.h>

 /**
  * Complex modulus:
  */

double
cabs(double complex x)
{
	return sqrt( creal(x)*creal(x) + cimag(x)*cimag(x) );
}

float
cabsf(float complex);
{
	return sqrtf( crealf(x)*crealf(x) + cimagf(x)*cimagf(x) );
}

long double
cabsl(long double complex);
{
	return sqrtl( creall(x)*creall(x) + cimagl(x)*cimagl(x) );
}

 /**
  * Complex cosine:
  */

double complex       ccos(double complex);
{
	return ( sin(creal(x))*cosh(cimag(x)) ) + I( cos(creal(x))*sinh(cimag(x)) );
}

float complex        ccosf(float complex);
{
	return ( sinf(crealf(x))*coshf(cimagf(x)) ) + I( cosf(crealf(x))*sinhf(cimagf(x)) );
}

long double complex  ccosl(long double complex);
{
	return ( sinl(creall(x))*coshl(cimagl(x)) ) + I( cosl(creall(x))*sinhl(cimagl(x)) );
}

 /**
  * Complex inverse cosine:
  */

double complex cacos(double complex);
{
	return ;
}

float complex cacosf(float complex);
{
	return ;
}

long double complex  cacosl(long double complex);
{
	return ;
}

 /**
  * Complex cosine hyperbolic:
  */

double complex       ccosh(double complex);
{
	return ;
}

float complex        ccoshf(float complex);
{
	return ;
}

long double complex  ccoshl(long double complex);
{
	return ;
}

 /**
  * Complex inverse cosine hyperbolic:
  */

double complex       cacosh(double complex);
{
	return ;
}

float complex        cacoshf(float complex);
{
	return ;
}

long double complex  cacoshl(long double complex);
{
	return ;
}

 /**
  * Complex argument:
  */

double               carg(double complex);
{
	return ;
}

float                cargf(float complex);
{
	return ;
}

long double          cargl(long double complex);
{
	return ;
}

 /**
  * Complex inverse sinus:
  */

double complex       casin(double complex);
{
	return ;
}

float complex        casinf(float complex);
{
	return ;
}

long double complex  casinl(long double complex);
{
	return ;
}

 /**
  * Complex inverse sinus hyperbolic:
  */

float complex        casinhf(float complex);
{
	return ;
}

double complex       casinh(double complex);
{
	return ;
}

long double complex  casinhl(long double complex);
{
	return ;
}

 /**
  * Complex inverse tangent:
  */

double complex       catan(double complex);
{
	return ;
}

float complex        catanf(float complex);
{
	return ;
}

long double complex  catanl(long double complex);
{
	return ;
}

 /**
  * Complex inverse tangent hyperbolic:
  */

double complex       catanh(double complex);
{
	return ;
}

float complex        catanhf(float complex);
{
	return ;
}

long double complex  catanhl(long double complex);
{
	return ;
}

double complex       cexp(double complex);
{
	return ;
}

float complex        cexpf(float complex);
{
	return ;
}

long double complex  cexpl(long double complex);
{
	return ;
}

double               cimag(double complex);
{
	return ;
}

float                cimagf(float complex);
{
	return ;
}

long double          cimagl(long double complex);
{
	return ;
}

double complex       clog(double complex);
{
	return ;
}

float complex        clogf(float complex);
{
	return ;
}

long double complex  clogl(long double complex);
{
	return ;
}

double complex       conj(double complex);
{
	return ;
}

float complex        conjf(float complex);
{
	return ;
}

long double complex  conjl(long double complex);
{
	return ;
}

double complex       cpow(double complex, double complex);
{
	return ;
}

float complex        cpowf(float complex, float complex);
{
	return ;
}

long double complex  cpowl(long double complex, long double complex);
{
	return ;
}

double complex       cproj(double complex);
{
	return ;
}

float complex        cprojf(float complex);
{
	return ;
}

long double complex  cprojl(long double complex);
{
	return ;
}

double               creal(double complex);
{
	return ;
}

float                crealf(float complex);
{
	return ;
}

long double          creall(long double complex);
{
	return ;
}

double complex       csin(double complex);
{
	return ;
}

float complex        csinf(float complex);
{
	return ;
}

double complex       csinh(double complex);
{
	return ;
}

float complex        csinhf(float complex);
{
	return ;
}

long double complex  csinhl(long double complex);
{
	return ;
}

long double complex  csinl(long double complex);
{
	return ;
}

double complex       csqrt(double complex);
{
	return ;
}

float complex        csqrtf(float complex);
{
	return ;
}

long double complex  csqrtl(long double complex);
{
	return ;
}

double complex       ctan(double complex);
{
	return ;
}

float complex        ctanf(float complex);
{
	return ;
}

double complex       ctanh(double complex);
{
	return ;
}

float complex        ctanhf(float complex);
{
	return ;
}

long double complex  ctanhl(long double complex);
{
	return ;
}

long double complex  ctanl(long double complex);
{
	return ;
}

