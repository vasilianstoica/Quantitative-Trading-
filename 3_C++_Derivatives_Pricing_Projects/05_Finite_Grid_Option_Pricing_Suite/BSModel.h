//This is the file BSModel.h
#ifndef BSModel_h
#define BSModel_h
class BSModel
{
private:
double r, sigma, T, S0;
public:
double Getr(){return r;}
double GetS0(){return S0;}
void SetS0(double S0_) { S0 = S0_; }
double GetT(){return T;}
double Getsigma(){return sigma;}
BSModel(double S0_, double r_, double sigma_, double T_);
double S(double St, double dt); // This function is used to
// generate random stock prices
};
#endif
