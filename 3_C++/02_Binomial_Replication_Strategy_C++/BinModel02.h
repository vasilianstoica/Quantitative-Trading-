//This is file BinModel02.h

#ifndef BinModel02_h
#define BinModel02_h

class BinModel
{
   private:
      double S0;
      double U;
      double D;
      double R;

   public:
    BinModel() = default;                      // keeps old behaviour
    BinModel(double s0, double u, double d,    // ← NEW: for unit tests
             double r) : S0(s0), U(u), D(d), R(r) {}

    double RiskNeutProb();
    double S(int n,int i);
    int    GetInputData();
    double GetR();
};

#endif
