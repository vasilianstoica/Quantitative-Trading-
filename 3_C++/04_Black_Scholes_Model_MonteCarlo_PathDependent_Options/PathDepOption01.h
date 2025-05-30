#ifndef PathDepOption01_h
#define PathDepOption01_h

#include "BSModel01.h"
#include <utility> // For std::pair

class PathDepOption
{
   public:
      double T, Price, PricingError;
      int m;
      double PriceByMC(BSModel Model, long N);
      double normsdist(double x);
      double PriceBSFormulaCall(BSModel Model, double S0, double r, double sigma, double T, double K);
      double PriceBSFormulaPut(BSModel Model, double S0, double r, double sigma, double T, double K);
      //std::pair<double, double> ReplicationStrategy(BSModel Model, double S0, double H);
      std::pair<double, double> ReplicationStrategy(BSModel Model, long N, double epsilon);
      virtual double Payoff(SamplePath& S)=0;
};

class ArthmAsianCall: public PathDepOption
{
   public:
      double K;
      ArthmAsianCall(double T_, double K_, int m_)
            {T=T_; K=K_; m=m_;}
      double Payoff(SamplePath& S);
};

class LookbackOption : public PathDepOption
{
   public:
      LookbackOption(double T_, int m_) {
            T = T_; m = m_;}
      double Payoff(SamplePath& S);
};

class EurCall : public PathDepOption
{
   public:
      double K;
      EurCall(double T_, double K_)
            {T=T_; K=K_; m=1;}
      double Payoff(SamplePath& S);
};

class EurPut : public PathDepOption
{
   public:
      double K;
      EurPut(double T_, double K_)
            {T=T_; K=K_; m=1;}
      double Payoff(SamplePath& S);
};

class Butterfly : public PathDepOption
{
   public:
      double K1;
      double K2;
      Butterfly (double T_, double K1_, double K2_)
            {T=T_; K1=K1_; K2=K2_; m=1;}
      double Payoff(SamplePath& S);
};
#endif
