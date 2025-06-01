//This is file DigiOption02.cpp

#include "DigiOption02.h"
#include "BinModel01.h"
#include <iostream>
#include <cmath>
using namespace std;

double DigitalCallPayoff(double S, double K)
{
   if (K<S) return 1;
   return 0.0;
}

double DigitalPutPayoff(double S, double K)
{
   if (K>S) return 1;
   return 0.0;
}
