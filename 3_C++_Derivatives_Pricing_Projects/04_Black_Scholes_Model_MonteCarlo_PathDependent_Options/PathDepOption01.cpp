#include "PathDepOption01.h"
#include <cmath>
#include <algorithm>
#include <vector>

double PathDepOption::PriceByMC(BSModel Model, long N)
{
   double H=0.0, Hsq=0.0;
   SamplePath S(m);
   for(long i=0; i<N; i++)
   {
      Model.GenerateSamplePath(T,m,S);
      H = (i*H + Payoff(S))/(i+1.0);
      Hsq = (i*Hsq + pow(Payoff(S),2.0))/(i+1.0);
   }
   Price = exp(-Model.r*T)*H;
   PricingError = exp(-Model.r*T)*sqrt(Hsq-H*H)/sqrt(N-1.0);
   return Price;
}

std::pair<double, double> PathDepOption::ReplicationStrategy(BSModel Model, long N, double epsilon)
{
    double S0 = Model.S0;
    double H = PriceByMC(Model, N);
    double H_plus, H_minus, delta, y;

    Model.S0 = S0 * (1 + epsilon);
    H_plus = PriceByMC(Model, N);

    Model.S0 = S0 * (1 - epsilon);
    H_minus = PriceByMC(Model, N);

    delta = (H_plus - H_minus) / (2 * epsilon * S0);

    // Reset S0
    Model.S0 = S0;
    y = H - delta * S0;

    return std::make_pair(delta, y);
}

double PathDepOption::PriceBSFormulaCall(BSModel Model, double S0, double r, double sigma, double T, double K)
{
   double d_plus = (log(S0/K)+(r+(sigma*sigma)/2)*T)/(sigma*sqrt(T));
   double d_minus = (log(S0/K)+(r-(sigma*sigma)/2)*T)/(sigma*sqrt(T));
   double H_0 = S0*normsdist(d_plus)-K*exp(-r*T)*normsdist(d_minus);
   return H_0;
}

double PathDepOption::PriceBSFormulaPut(BSModel Model, double S0, double r, double sigma, double T, double K)
{
   double d_plus = (log(S0/K)+(r+(sigma*sigma)/2)*T)/(sigma*sqrt(T));
   double d_minus = (log(S0/K)+(r-(sigma*sigma)/2)*T)/(sigma*sqrt(T));
   double H_0 = K*exp(-r*T)*normsdist(-d_minus)-S0*normsdist(-d_plus);
   return H_0;
}


double PathDepOption::normsdist(double x) {
    // Constants
    const double a1 = 0.254829592;
    const double a2 = -0.284496736;
    const double a3 = 1.421413741;
    const double a4 = -1.453152027;
    const double a5 = 1.061405429;
    const double p = 0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0) {
        sign = -1;
    }
    x = std::fabs(x) / std::sqrt(2.0);

    // A&S formula 7.1.26
    double t = 1.0 / (1.0 + p * x);
    double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

    return 0.5 * (1.0 + sign * y);
}

double ArthmAsianCall::Payoff(SamplePath& S)
{
   double Ave=0.0;
   for (int k=0; k<m; k++) Ave=(k*Ave+S[k])/(k+1.0);
   if (Ave<K) return 0.0;
   return Ave-K;
}

double LookbackOption::Payoff(SamplePath& S) {
    double finalPrice = S.back(); // Last element in the sample path
    double minPrice = *std::min_element(S.begin(), S.end()); // Minimum element in the sample path
    return finalPrice - minPrice;
}

double EurCall::Payoff(SamplePath& S)
{
   if (S[0]<K) return 0.0;
   return S[0]-K;
}

double EurPut::Payoff(SamplePath& S)
{
   if (K<S[0]) return 0.0;
   return K-S[0];
}

double Butterfly::Payoff(SamplePath& S)
{
   if (K1<S[0] and S[0]<=((K1+K2)/2))
   {
   return S[0]-K1;
   }
   else if (((K1+K2)/2)<S[0] and S[0]<=K2)
   {
   return K2-S[0];
   }
   else
   {
   return 0.0;
   }
}
