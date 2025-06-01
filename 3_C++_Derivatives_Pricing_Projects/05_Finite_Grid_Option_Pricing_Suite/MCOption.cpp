// This is the file MCOption.cpp
#include "MCOption.h"
#include "BSModel.h"
#include "BSOption.h"
#include <cmath>
#include <iostream>
using namespace std;
PriceResult MCAsOption::Price(BSModel stock, double epsilon)
{
    long N = GetN();
    double T = GetT();
    double Mean = 0.;
    double H;
    double Mean2 = 0.;
    double S0 = stock.GetS0();
    double S0_plus = S0 * (1 + epsilon);
    double S0_minus = S0 * (1 - epsilon);
    double Mean_plus = 0.;
    double Mean_minus = 0.;

    for (long k = 1; k <= N; k++)
    {
        H = Payoff(stock);
        Mean = ((k - 1) * Mean + H) / k;
        Mean2 = ((k - 1) * Mean2 + H * H) / k;

        // Compute H_plus and H_minus using the same path
        stock.SetS0(S0_plus);
        double H_plus = Payoff(stock);
        Mean_plus = ((k - 1) * Mean_plus + H_plus) / k;

        stock.SetS0(S0_minus);
        double H_minus = Payoff(stock);
        Mean_minus = ((k - 1) * Mean_minus + H_minus) / k;

        // Reset S0
        stock.SetS0(S0);
    }

    SetPrecision(exp(-T * stock.Getr()) * (Mean2 - Mean * Mean) / sqrt(N - 1));
    double price = Mean * exp(-stock.Getr() * GetT()) + ControlVariate(stock);
    double price_plus = Mean_plus * exp(-stock.Getr() * GetT()) + ControlVariate(stock);
    double price_minus = Mean_minus * exp(-stock.Getr() * GetT()) + ControlVariate(stock);

    return {price, price_plus, price_minus};
}

double Average(BSModel stock, double T, int M)
{
double dt=T/M;
double St=stock.GetS0();
double Mean=0.;
for(int i=1; i<=M; i++)
{
St=stock.S(St,dt);
Mean = ((i-1)*Mean + St)/i;
}
return Mean;
}
double MCCall::Payoff(BSModel stock)
{
double Av = Average(stock, GetT(), GetNofSteps());
if(Av>X) return Av-X;
return 0.;
}
double MCPut::Payoff(BSModel stock)
{
double Av = Average(stock, GetT(), GetNofSteps());
if(X>Av) return X-Av;
return 0.;
}

ReplicationResult MCOption::ReplicationStrategy(BSModel Model, long N, double epsilon)
{
    double S0 = Model.GetS0();
    PriceResult priceResult = static_cast<MCAsOption*>(this)->Price(Model, epsilon);

    double H = priceResult.price;
    double H_plus = priceResult.H_plus;
    double H_minus = priceResult.H_minus;

    double delta = (H_plus - H_minus) / (2 * epsilon * S0);
    double y = H - delta * S0;

    return {H, delta, y};
}
