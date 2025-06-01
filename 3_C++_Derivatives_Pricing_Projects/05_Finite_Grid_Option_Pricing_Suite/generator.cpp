//This is the file generator.cpp
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "generator.h"
#include <ctime>   // add this

using namespace std;
double Gauss()
{
      double U1 = (rand()+1.0)/(RAND_MAX+1.0);
      double U2 = (rand()+1.0)/(RAND_MAX+1.0);
      return sqrt(-2.*log(U1)) * cos(8.*atan(1.)*U2);
}
void TestGauss(int N)
{
srand((unsigned)time(0));
double Z;
double Mean = 0.;
double MeanSqr = 0.;
for(int k=1; k<=N; k++)
{
Z = Gauss();
Mean=((k-1)*Mean+Z)/k;
MeanSqr=((k-1)*MeanSqr+Z*Z)/k;
}
cout << "TESTING THE MEAN AND VARIANCE OF GAUSSIAN GENERATOR: " << endl;
cout << "Mean " << Mean << endl;
cout << "Variance " << N*(MeanSqr - Mean*Mean)/(N-1) << endl << endl;
}
