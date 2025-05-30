// Tests.cpp
#include <iostream>
#include <cassert>
#include <cmath>
#include "PathDepOption01.h"
#include "BSModel01.h"

bool approxEqual(double a, double b, double tol = 1e-6) {
    return std::fabs(a - b) < tol;
}

int main() {
    // --- normsdist tests ---
    {
        PathDepOption* dummy = nullptr; // only to call normsdist
        PathDepOption& pd = *reinterpret_cast<PathDepOption*>(&dummy);
        double cdf0 = pd.normsdist(0.0);
        assert(approxEqual(cdf0, 0.5));
        double cdf1 = pd.normsdist(1.0);
        // known value ~0.8413447
        assert(approxEqual(cdf1, 0.8413447, 1e-5));
    }

    // --- BS formula parity tests ---
    {
        double S0    = 100.0;
        double r     = 0.05;
        double sigma = 0.2;
        double T     = 1.0;
        double K     = 100.0;
        BSModel model(S0, r, sigma);
        PathDepOption* dummy = nullptr;
        PathDepOption& pd = *reinterpret_cast<PathDepOption*>(&dummy);

        double C = pd.PriceBSFormulaCall(model, S0, r, sigma, T, K);
        double P = pd.PriceBSFormulaPut (model, S0, r, sigma, T, K);
        double diff = C - P;
        double parity = S0 - K * std::exp(-r * T);
        assert(approxEqual(diff, parity, 1e-6));
    }

    // --- Payoff tests ---
    {
        // ArthmAsianCall
        int m = 3;
        double T = 1.0, K = 10.0;
        ArthmAsianCall asian(T, K, m);
        SamplePath sp(m);
        sp[0] = 10; sp[1] = 12; sp[2] = 14;
        // average = 12 => payoff = 2
        assert(approxEqual(asian.Payoff(sp), 2.0));

        // Lookback
        LookbackOption look(T, m);
        sp = {5, 3, 7};
        // final=7, min=3 => payoff=4
        assert(approxEqual(look.Payoff(sp), 4.0));

        // EurCall
        EurCall ecall(T, 50.0);
        sp = {80};
        assert(approxEqual(ecall.Payoff(sp), 30.0));
        sp = {40};
        assert(approxEqual(ecall.Payoff(sp), 0.0));

        // EurPut
        EurPut eput(T, 50.0);
        sp = {40};
        assert(approxEqual(eput.Payoff(sp), 10.0));
        sp = {60};
        assert(approxEqual(eput.Payoff(sp), 0.0));

        // Butterfly (K1=90,K2=110)
        Butterfly bf(T, 90.0, 110.0);
        sp = {95};  // between 90 and 100 => 5
        assert(approxEqual(bf.Payoff(sp), 5.0));
        sp = {105}; // between 100 and 110 => 5
        assert(approxEqual(bf.Payoff(sp), 5.0));
        sp = {85};  // outside => 0
        assert(approxEqual(bf.Payoff(sp), 0.0));
        sp = {115}; // outside => 0
        assert(approxEqual(bf.Payoff(sp), 0.0));
    }

    std::cout << "All unit tests passed." << std::endl;
    return 0;
}
