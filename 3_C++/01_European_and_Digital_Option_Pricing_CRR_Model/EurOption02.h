//This is file EurOption02.h

#ifndef EurOption02_h
#define EurOption02_h

//inputting and displaying option data
int GetOptionData(int& N, double& K);

//pricing European option by CRR formula
double PriceEurOptByCRR(double S0, double U, double D,
                        double R, int N, double K,
                        double (*Payoff)(double S, double K));

//computing the number of n-element subsets in an N-element set
double NewtonSymb(int N, int n);

//coumputing call payoff
double CallPayoff(double S, double K);

//coumputing put payoff
double PutPayoff(double S, double K);

#endif
