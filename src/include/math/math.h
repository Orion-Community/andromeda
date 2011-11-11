/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

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
#ifndef MATH_H
#define MATH_H

#define RANDMAX 0xFFFF
#define RANDMIN 0x0

void randomize(unsigned int seed);
int random();
long abs(long num);

// Some funtions still have to be made...

double sin(double x);
float sinf(float x);
long double sinl(long double x);

double asin(double x);
float asinf(float x);
long double asinl(long double x);

double sinh(double x);
float sinhf(float x);
long double sinhl(long double x);

double asinh(double x);
float asinhf(float x);
long double asinhl(long double x);

double cos(double x);
float cosf(float x);
long double cosl(long double x);cos

double acos(double x);
float acosf(float x);
long double acosl(long double x);

double cosh(double x);
float coshf(float x);
long double coshl(long double x);

double acosh(double x);
float acoshf(float x);
long double acoshl(long double x);

double tan(double x);
float tanf(float x);
long double tanl(long double x);

double atan(double x);
float atanf(float x);
long double atanl(long double x);
/* 
 * Still need to be made:
 * 
double tanh(double x);
float tanhf(float x);
long double tanhl(long double x);

double atanh(double x);
float atanhf(float x);
long double atanhl(long double x);
*/
double exp(double x);
float expf(float x);
long double expl(long double x);

double ln(double x);
float lnf(float x);
long double lnl(long double x);

double sqrt(double x);
float sqrtf(float x);
long double sqrtl(long double x);

double pow(double x);
float powf(float x);
long double powl(long double x);

#endif
