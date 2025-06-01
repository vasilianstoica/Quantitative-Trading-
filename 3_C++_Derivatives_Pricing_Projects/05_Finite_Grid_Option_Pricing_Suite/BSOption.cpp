//This is the file BSOption.cpp
#include <cmath>
#include "BSModel.h"
#include "BSOption.h"
double N(const double x)
{
const double b1 = 0.319381530;
const double b2 = -0.356563782;
const double b3 = 1.781477937;
const double b4 = -1.821255978;
const double b5 = 1.330274429;
const double p = 0.2316419;
const double c = 0.39894228;
if(x >= 0.0) {
double t = 1.0 / ( 1.0 + p * x );
return (1.0 - c * exp( -x * x / 2.0 ) * t *
( t *( t * ( t * ( t * b5 + b4 ) + b3 ) + b2 ) + b1 ));
}
else {
double t = 1.0 / ( 1.0 - p * x );
return ( c * exp( -x * x / 2.0 ) * t *
( t *( t * ( t * ( t * b5 + b4 ) + b3 ) + b2 ) + b1 ));
}
}
double BSCall::Price(BSModel a)
{
double d1, d2, price;
d1 = ( log(a.GetS0()/X) + (a.Getr() + pow(a.Getsigma(),2)/2.)*T )
/ (a.Getsigma()*sqrt(T));
d2 = d1 - a.Getsigma()*sqrt(T);
price = a.GetS0()*N(d1) - X*exp(-a.Getr()*T)*N(d2);
x = N(d1);
y = price - x*a.GetS0();
return price;
}
double BSPut::Price(BSModel a)
{
double d1, d2, price;
d1 = ( log(a.GetS0()/X) + (a.Getr() + pow(a.Getsigma(),2)/2.)*a.GetT() )
/ (a.Getsigma()*sqrt(a.GetT()));
d2 = d1 - a.Getsigma()*sqrt(a.GetT());
price = X*exp(-a.Getr()*a.GetT()) * N(-d2) - a.GetS0() * N(-d1);
x = -N(-d1);
y = price - x*a.GetS0();
return price;
}
