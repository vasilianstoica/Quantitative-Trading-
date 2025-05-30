#include "Pricing.h"
using namespace std;
#include <iostream>
#include <functional>  // Include this for std::function
#include <random>
#include <vector>
#include "Logger.h"

int main() {
    double S0 = 100.0;  // Initial stock price
    double U = 0.10;    // Upward movement percentage
    double D = -0.05;   // Downward movement percentage
    double K = 100.0;
    int steps = 4;      // Number of steps in the binomial tree
    double R = 0.05;   // Risk-free rate
    long N = 1000000;    // Number of paths for Monte Carlo
    srand(time(nullptr)); // Seed the random number generator once

    CSVLogger log("session_output.csv");
    Pricing pricing(S0, U, D, R, steps);
    auto timeZeroValueAsian = pricing.calculateTimeZeroValue(K, R, Option::computeAsianCallPayoff);
    std::cout << "Time-0 value of the Asian option: " << timeZeroValueAsian << std::endl;

    double timeZeroValueLookBack = pricing.calculateTimeZeroValue(K, R, Option::computeLookbackCallPayoff);
    std::cout << "Time-0 value of the Lookback option: " << timeZeroValueLookBack << std::endl;

    double amerPut = pricing.calculateAmericanPutPrice(K);
    std::cout << "Time-0 value of the American put option: " << amerPut << std::endl;

    auto [asianPrice, asianstandardDeviation] = pricing.calculateMonteCarlo(S0, K, steps, N, Option::computeAsianCallPayoff);
    std::cout << "Monte Carlo estimated price of Asian Option: " << asianPrice << std::endl;
    std::cout << "Standard Deviation of the estimate: " << asianstandardDeviation << std::endl;

    auto [lookbackPrice, standardDeviation] = pricing.calculateMonteCarlo(S0, K, steps, N, Option::computeLookbackCallPayoff);
    std::cout << "Monte Carlo estimated price of Lookback Option: " << lookbackPrice << std::endl;
    std::cout << "Standard Deviation of the estimate: " << standardDeviation << std::endl;

    pair<double,double> mcRes = pricing.calculateAmericanPutMC_Tree(K, N);
    double amerPutMC     = mcRes.first;
    double amerPutStdErr = mcRes.second;
    std::cout << "Monte Carlo estimated price of American Put Option: " << amerPutMC << std::endl;
    std::cout << "Standard Deviation of the estimate: " << amerPutStdErr << std::endl;


    return 0;
}
