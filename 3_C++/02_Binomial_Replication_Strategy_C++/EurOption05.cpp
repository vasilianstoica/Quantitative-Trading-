//This is file EurOption05.cpp

#include "EurOption05.h"
#include "BinModel02.h"
#include <iostream>
#include <cmath>
using namespace std;

double EurOption::PriceByCRR(BinModel Model, int n, int i)
{
   double p=Model.RiskNeutProb();
   double R = Model.GetR();
   double Price=0;
   for (int j = 0; j <= N - n; j++)
   {
      Price=Price+NewtonSymb(N - n,j)*pow(p,j)*pow(1-p,N - n - j)
            *Payoff(Model.S(N,i+j))/pow(R,N - n);
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

int EurCall::GetInputData()
{
   cout << "Enter European call data:" << endl;
   int N;
   cout << "Enter steps to expiry N: "; cin >> N; SetN(N);
   cout << "Enter strike price K:    "; cin >> K;
   cout << endl;
   cout << "N = " << N << endl;
   cout << "K = " << K << endl;
   cout << endl;
   return 0;
}

int EurPut::GetInputData()
{
   cout << "Enter European put data:" << endl;
   int N;
   cout << "Enter steps to expiry N: "; cin >> N; SetN(N);
   cout << "Enter strike price K:    "; cin >> K;
   cout << endl;
   cout << "N = " << N << endl;
   cout << "K = " << K << endl;
   cout << endl;
   return 0;
}

double EurCall::Payoff(double S)
{
   if (S>K) return S-K;
   return 0.0;
}

double EurPut::Payoff(double S)
{
   if (S<K) return K-S;
   return 0.0;
}
