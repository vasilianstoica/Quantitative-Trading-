//This is a file BSModel.cpp
#include "BSModel.h"
#include "generator.h"
#include <cmath>
BSModel::BSModel(double S0_, double r_, double sigma_, double T_)
{
S0 = S0_;
r = r_;
sigma = sigma_;
T = T_;
}
double BSModel::S(double St, double dt)
{
return St*exp( (r-sigma*sigma/2.)*dt + sigma*sqrt(dt)*Gauss() );
}
