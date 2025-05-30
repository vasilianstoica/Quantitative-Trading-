//This is file EurOption05.h
#include <utility>  // for std::pair
#ifndef EurOption05_h
#define EurOption05_h

#include "BinModel02.h"

class EurOption
{
   private:
      int N; //steps to expiry

   public:
      void SetN(int N_){N=N_;}
      virtual double Payoff(double S){return 0;}
/*
Payoff defined to return 0 for pedagogical purposes.
To to use a pure virtual function replace by
virtual double Payoff(double S)=0;
*/
      //pricing European option by CRR formula
      double PriceByCRR(BinModel Model, int n, int i);
};

//computing the number of n-element subsets in an N-element set
double NewtonSymb(int N, int n);

class EurCall: public EurOption
{
   private:
      double K; //strike price

   public:
      void SetK(double K_){K=K_;}
      int GetInputData();
      double Payoff(double S);
};

class EurPut: public EurOption
{
   private:
      double K; //strike price

   public:
      void SetK(double K_){K=K_;}
      int GetInputData();
      double Payoff(double S);
};

#endif
