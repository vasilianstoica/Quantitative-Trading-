//This is file EurOption02.cpp

#include "EurOption02.h"
#include "BinModel01.h"
#include <iostream>
#include <cmath>
using namespace std;

int GetOptionData(int& N, double& K)
{
   cout << "Enter steps to expiry N: "; cin >> N;
   cout << "Enter strike price K:    "; cin >> K;
   cout << endl;
   cout << "N = " << N << endl;
   cout << "K = " << K << endl;
   cout << endl;
   return 0;
}

double PriceEurOptByCRR(double S0, double U, double D,
                        double R, int N, double K,
                        double (*Payoff)(double S, double K))
{
   double p=RiskNeutProb(U,D,R);

   double Price=0;
   for (int n=0; n<=N; n++)
   {
      Price=Price+NewtonSymb(N,n)*pow(p,n)*pow(1-p,N-n)
            *Payoff(S(S0,U,D,N,n),K)/pow(R,N);
   }
   return Price;
}

double NewtonSymb(int N, int n)
{
   if (n<0 || n>N) return 0;
   double result=1;
   for (int i=1; i<=n; i++) result=result*(N-n+i)/i;
   return result;
}

double CallPayoff(double S, double K)
{
   if (S>K) return S-K;
   return 0.0;
}

double PutPayoff(double S, double K)
{
   if (S<K) return K-S;
   return 0.0;
}
