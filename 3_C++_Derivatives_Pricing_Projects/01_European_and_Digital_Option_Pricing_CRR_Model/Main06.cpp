//This is file Main06.cpp

#include "BinModel01.h"
#include "EurOption02.h"
#include "DigiOption02.h"
#include <iostream>
#include <cmath>
using namespace std;

int main()
{
   //spot price, and one-step up, down and risk-free returns
   double S0,U,D,R;

   if (GetInputData(S0,U,D,R)==1) return 1;

   double K;   //strike price
   int N;      //steps to expiry

   cout << "Enter European call data:" << endl;
   GetOptionData(N,K);
   cout << "Option price = "
        << PriceEurOptByCRR(S0,U,D,R,N,K,CallPayoff)
        << endl << endl;

   cout << "Enter European put data:" << endl;
   GetOptionData(N,K);
   cout << "Option price =  "
        << PriceEurOptByCRR(S0,U,D,R,N,K,PutPayoff)
        << endl << endl;

   cout << "Enter Digital call data:" << endl;
   GetOptionData(N,K);
   cout << "Option price = "
        << PriceEurOptByCRR(S0,U,D,R,N,K,DigitalCallPayoff)
        << endl << endl;

   cout << "Enter Digital put data:" << endl;
   GetOptionData(N,K);
   cout << "Option price = "
        << PriceEurOptByCRR(S0,U,D,R,N,K,DigitalPutPayoff)
        << endl << endl;

   char x;
   cin >> x;

   return 0;
}
