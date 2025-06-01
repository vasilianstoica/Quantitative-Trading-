//This is file BinModel01.cpp

#include <iostream>
#include <cmath>
using namespace std;

double RiskNeutProb(double U, double D, double R)
{
   return (R-D)/(U-D);
}

double S(double S0, double U, double D, int n, int i)
{
   return S0*pow(U,i)*pow(D,n-i);
}

int GetInputData(double& S0, double& U, double& D, double& R)
{
   //entering data
   cout << "Enter S0: "; cin >> S0;
   cout << "Enter U:  "; cin >> U;
   cout << "Enter D:  "; cin >> D;
   cout << "Enter R:  "; cin >> R;
   cout << endl;

   //displaying entered data
   cout << "S0 = " << S0 << endl;
   cout << "U  = " << U << endl;
   cout << "D  = " << D << endl;
   cout << "R  = " << R << endl;
   cout << endl;

   //making sure that 0<S0, 0<D<U, 0<R
   if (S0<=0 || U<=0 || D<=0 || U<=D || R<=0)
   {
      cout << "Illegal data ranges" << endl;
      cout << "Terminating program" << endl;
      return 1;
   }

  //checking for arbitrage
   if (R>=U || R<=D)
   {
      cout << "Arbitrage exists" << endl;
      cout << "Terminating program" << endl;
      return 1;
   }

   cout << "Input data checked" << endl;
   cout << "There is no arbitrage" << endl << endl;

   return 0;
}
