// Tests.cpp
#include <iostream>
#include <cmath>
#include <cassert>
#include "Option.h"
#include "Pricing.h"

bool approxEqual(double a, double b, double tol = 1e-6) {
    return std::fabs(a - b) < tol;
}

int main() {
    // --- Option payoff tests ---
    {
        std::vector<double> v1{100, 110, 90};
        // Asian Call: average of [110,90]=100, strike=100 => 0
        assert(approxEqual(
            Option::computeAsianCallPayoff(v1, 100), 0.0));
        // Asian Call: strike=90 => payoff = 100-90 = 10
        assert(approxEqual(
            Option::computeAsianCallPayoff(v1, 90), 10.0));
    }
    {
        std::vector<double> v2{100, 110, 105};
        // Lookback Call: final=105, min=100 => payoff = 5
        assert(approxEqual(
            Option::computeLookbackCallPayoff(v2, 0), 5.0));
    }

    // --- One-step binomial model tests ---
    {
        Pricing pr1(100.0, 0.1, -0.1, 0.0, 1);
        // Node prices
        assert(approxEqual(pr1.getPriceAtNode(0, 0), 100.0));
        assert(approxEqual(pr1.getPriceAtNode(1, 0), 100.0 * 0.9));
        assert(approxEqual(pr1.getPriceAtNode(1, 1), 100.0 * 1.1));

        // Risk-neutral probability = (0 - (-0.1)) / (0.1 - (-0.1)) = 0.5
        assert(approxEqual(pr1.RiskNeutralProbability(), 0.5));

        // European payoffs via calculateTimeZeroValue
        // Asian Call on one-step: average of [110]=110, [90]=90 => expected = (0.5*(110-100)+0.5*(90-100)) = (0.5*10 + 0.5*(-10)) = 0
        // but since payoff = max(avg-100,0), only up path contributes => value = 0.5 * 10 = 5
        assert(approxEqual(
            pr1.calculateTimeZeroValue(100.0, 0.0, Option::computeAsianCallPayoff),
            5.0));
        // Lookback Call => (final(110)-min(100)=10) with prob 0.5, and 0 with prob 0.5 => expected 5
        assert(approxEqual(
            pr1.calculateTimeZeroValue(100.0, 0.0, Option::computeLookbackCallPayoff),
            5.0));

        // American Put price by Snell => 5
        assert(approxEqual(pr1.calculateAmericanPutPrice(100.0), 5.0));
}    }
