#include "Pricing.h"
#include "Option.h"
#include <iostream>
#include <cmath>
#include <functional>  // Include this for std::function
#include <algorithm> // For std::min_element
#include <random>
#include <vector>
#include <chrono>

Pricing::Pricing(double S0, double U, double D, double R, int steps)
    : U(U), D(D), R(R)
{
    generateBinomialTree(S0, U, D, steps);
}

void Pricing::generateBinomialTree(double S0, double U, double D, int steps) {
    tree.resize(steps + 1, std::vector<Node>(steps + 1));

    tree[0][0] = {S0, {""}};

    for (int i = 1; i <= steps; ++i) {
        for (int j = 0; j <= i; ++j) {
            if (j > 0) {
                double up_price = tree[i - 1][j - 1].price * (1 + U);
                tree[i][j].price = up_price;
                for (auto& path : tree[i - 1][j - 1].paths) {
                    tree[i][j].paths.push_back(path + "u");
                }
            }
            if (j < i) {
                double down_price = tree[i - 1][j].price * (1 + D);
                tree[i][j].price = down_price;
                for (auto& path : tree[i - 1][j].paths) {
                    tree[i][j].paths.push_back(path + "d");
                }
            }
        }
    }
}

void Pricing::printBinomialTree() const {
    int steps = tree.size() - 1;
    for (int i = 0; i <= steps; ++i) {
        for (int j = 0; j <= i; ++j) {
            std::cout << "S(" << i << "," << j << ") = " << tree[i][j].price << ": ";
            for (auto& path : tree[i][j].paths) {
                std::cout << path << " ";
            }
            std::cout << std::endl;
        }
    }
}

double Pricing::getPriceAtNode(int stepIndex, int nodeIndex) const {
    if (stepIndex < 0 || stepIndex >= tree.size() ||
        nodeIndex < 0 || nodeIndex >= tree[stepIndex].size()) {
        std::cerr << "Invalid index" << std::endl;
        return -1; // Return an error value if indices are out of bounds
    }
    return tree[stepIndex][nodeIndex].price;
}

void Pricing::generatePaths(std::vector<std::string>& paths, std::string currentPath, int steps) const {
    if (steps == 0) {
        paths.push_back(currentPath);
        return;
    }
    generatePaths(paths, currentPath + "u", steps - 1);
    generatePaths(paths, currentPath + "d", steps - 1);
}

std::vector<std::string> Pricing::getAllPaths(int steps) const {
    std::vector<std::string> paths;
    generatePaths(paths, "", steps);
    return paths;
}


std::vector<std::pair<std::string, double>> Pricing::calculateAllPathPayoffs(double strike, std::function<double(const std::vector<double>&, double)> payoffFunc) const {
    std::vector<std::string> paths = getAllPaths(tree.size() - 1);
    std::vector<std::pair<std::string, double>> pathPayoffs;

    for (const auto& path : paths) {
        std::vector<double> prices;
        double S = tree[0][0].price;
        prices.push_back(S);
        for (char step : path) {
            S *= (step == 'u') ? (1 + U) : (1 + D);
            prices.push_back(S);
        }
        double payoff = payoffFunc(prices, strike);
        pathPayoffs.emplace_back(path, payoff);
    }

    return pathPayoffs;
}

double Pricing::RiskNeutralProbability() const {
    return (R - D) / (U - D);
}

double Pricing::calculateTimeZeroValue(double strike, double riskFreeRate, std::function<double(const std::vector<double>&, double)> payoffFunc) {
    auto pathPayoffs = calculateAllPathPayoffs(strike, payoffFunc);
    double q = RiskNeutralProbability();  // Risk-neutral probability of an upward move
    int N = tree.size() - 1;
    double discountFactor = pow(1 + R, -(N));  // Discount factor based on the risk-free rate and number of periods

    double expectedValue = 0.0;
    for (const auto& [path, payoff] : pathPayoffs) {
        double pathProbability = 1.0;  // Initialize the probability of this path occurring

        // Calculate the probability of this path occurring
        for (char step : path) {
            pathProbability *= (step == 'u') ? q : (1 - q);
        }

        // Accumulate the discounted expected payoff
        expectedValue += pathProbability * payoff;
    }

    // Return the total expected value discounted back to the present

    return expectedValue * discountFactor;
}

void Pricing::generateMonteCarloPath(double initialPrice, double upProbability, int steps, std::vector<double>& path) const {

    path.clear();
    double currentPrice = initialPrice;
    path.push_back(currentPrice);
for (int i = 0; i < steps; ++i) {
        double r = static_cast<double>(rand()) / RAND_MAX; // Generate random number between 0 and 1
        if (r < upProbability) // Taking into account risk neutral probability
            currentPrice *= (1 + U);
        else
            currentPrice *= (1 + D);
        path.push_back(currentPrice);
    }
}

std::pair<double, double>  Pricing::calculateMonteCarlo(double initialPrice, double strike, int steps, int numPaths, std::function<double(const std::vector<double>&, double)> payoffFunc) const {
    std::vector<double> path;
    std::vector<double> payoffs;
    double sumPayoffs = 0.0;
    double up_probability = RiskNeutralProbability(); // Use the internal method to calculate the risk-neutral probability
    double sumDiff = 0.0;
    double diff;
    double numPathsfloat = numPaths;

    for (long i = 0; i < numPaths; ++i) {
        generateMonteCarloPath(initialPrice, up_probability, steps, path);  // Already using internal U, D, R
        double payoff = payoffFunc(path, strike);
        sumPayoffs += payoff;
        payoffs.push_back(payoff);
    }

    double discountFactor = pow(1 + R, -steps);  // Discount the average payoff to present value
    double meanPayoff = sumPayoffs / numPaths;
        for (double payoff : payoffs) {
        diff = pow(payoff - meanPayoff, 2);
        sumDiff +=diff;
    }
    double Variance = (1/(numPathsfloat-1))*sumDiff;
    double standardDeviation = (sqrt(Variance)/(sqrt(numPathsfloat)))* discountFactor;
    double discountedMeanPayoff = meanPayoff * discountFactor;

    return {discountedMeanPayoff, standardDeviation};
}
// --- NEW: binomial‐tree American‐put (Snell envelope) ---------------
double Pricing::calculateAmericanPutPrice(double strike) const {
    int N = tree.size() - 1;
    double q = RiskNeutralProbability();
    std::vector<double> V(N+1);

    // 1) payoff at maturity
    for(int i = 0; i <= N; ++i) {
        double ST = tree[N][i].price;
        V[i] = std::max(strike - ST, 0.0);
    }

    // 2) step backwards, checking early exercise
    for(int n = N-1; n >= 0; --n) {
        for(int i = 0; i <= n; ++i) {
            double cont = ( q * V[i+1] + (1 - q) * V[i] ) / (1 + R);
            double exercise = std::max(strike - tree[n][i].price, 0.0);
            V[i] = std::max(cont, exercise);
        }
    }

    return V[0];
}

std::pair<double,double>
Pricing::calculateAmericanPutMC_Tree(double strike, int numPaths) const {
    int N  = static_cast<int>(tree.size()) - 1;
    double q  = RiskNeutralProbability();
    double df = 1.0 / (1.0 + R);

    // 0) One‐time engine seeded from clock
    static std::mt19937_64 rng(
        std::chrono::high_resolution_clock::now()
            .time_since_epoch()
            .count()
    );

    // 1) Build Snell envelope and record exercise
    std::vector<std::vector<bool>> exercise(N+1);
    std::vector<double> V(N+1);

    // (a) at maturity
    exercise[N].resize(N+1);
    for(int i = 0; i <= N; ++i) {
        double ST        = tree[N][i].price;
        double intrinsic = std::max(strike - ST, 0.0);
        V[i]             = intrinsic;
        exercise[N][i]   = (intrinsic > 0.0);
    }

    // (b) backward‐induction Snell
    for(int n = N - 1; n >= 0; --n) {
        exercise[n].resize(n+1);
        for(int i = 0; i <= n; ++i) {
            double contValue = (q * V[i+1] + (1 - q) * V[i]) * df;
            double intrinsic = std::max(strike - tree[n][i].price, 0.0);
            if (intrinsic >= contValue) {
                V[i]            = intrinsic;
                exercise[n][i] = true;
            } else {
                V[i]            = contValue;
                exercise[n][i] = false;
            }
        }
    }

    // 2) MC simulate paths _through_ that tree
    std::bernoulli_distribution coin(q);
    std::vector<double> payoffs(numPaths);

    for(int p = 0; p < numPaths; ++p) {
        int    i    = 0;
        double S    = tree[0][0].price;  // S0
        int    t_ex = -1;
        double payoff = 0.0;

        for(int n = 0; n <= N; ++n) {
            if (exercise[n][i]) {
                payoff = std::max(strike - S, 0.0);
                t_ex   = n;
                break;
            }
            if (n == N) {
                payoff = std::max(strike - S, 0.0);
                t_ex   = N;
                break;
            }
            // step
            if (coin(rng)) {
                ++i;
                S *= (1 + U);
            } else {
                S *= (1 + D);
            }
        }

        payoffs[p] = payoff * std::pow(df, t_ex);
    }

    // 3) collect stats
    double sum = 0.0, sum2 = 0.0;
    for(double v: payoffs) {
        sum  += v;
        sum2 += v*v;
    }
    double mean   = sum / numPaths;
    double var    = (sum2 - sum*sum/numPaths) / (numPaths - 1);
    double stdErr = std::sqrt(var / numPaths);

    return { mean, stdErr };
}
