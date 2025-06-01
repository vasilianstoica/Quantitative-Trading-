#ifndef Pricing_h
#define Pricing_h
#include <vector>
#include <string>
#include "Option.h"
#include <functional>  // Include this for std::function
#include <random>
#include <ctime>

struct Node {
    double price;
    std::vector<std::string> paths;
};

class Pricing {
public:
    Pricing(double S0, double U, double D, double R, int steps);
    void printBinomialTree() const;
    double getPriceAtNode(int stepIndex, int nodeIndex) const; // New function to get price at specific node
    std::vector<std::string> getAllPaths(int steps) const;
    double RiskNeutralProbability() const;
    std::vector<std::pair<std::string, double>> calculateAllPathPayoffs(double strike, std::function<double(const std::vector<double>&, double)> payoffFunc) const;
    double calculateTimeZeroValue(double strike, double riskFreeRate, std::function<double(const std::vector<double>&, double)> payoffFunc);
    std::vector<double> generateRandomPath(double initialPrice, int steps, double U, double D, double seed = std::random_device()()) const;
    void generateMonteCarloPath(double initialPrice, double upProbability, int steps, std::vector<double>& path) const;
    std::pair<double, double>  calculateMonteCarlo(double initialPrice, double strike, int steps, int numPaths, std::function<double(const std::vector<double>&, double)> payoffFunc) const;
    double calculateAmericanPutPrice(double strike) const;
    std::pair<double,double> calculateAmericanPutMC_Tree(double strike, int numPaths) const;
private:
    std::vector<std::vector<Node>> tree;
    void generateBinomialTree(double S0, double U, double D, int steps);
    void generatePaths(std::vector<std::string>& paths, std::string currentPath, int steps) const;
    double U, D, R;  // Up, Down, and Risk-free rate variables
};

#endif
