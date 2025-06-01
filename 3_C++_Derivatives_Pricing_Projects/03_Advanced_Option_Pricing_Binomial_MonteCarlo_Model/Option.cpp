// Option.cpp
#include "Option.h"
#include <algorithm> // For std::max
#include <numeric>   // For std::accumulate
#include <functional>  // Include this for std::function
#include <iostream>
#include <numeric>   // For std::accumulate
#include <cmath>

double Option::computeAsianCallPayoff(const std::vector<double>& prices, double strike) {
    if (prices.size() <= 1) return 0.0;  // Guard against insufficient data

    // Calculate the average of prices starting from the second element
    double averagePrice = std::accumulate(prices.begin() + 1, prices.end(), 0.0) / (prices.size() - 1);
    return std::max(averagePrice - strike, 0.0);
}

double Option::computeLookbackCallPayoff(const std::vector<double>& prices, double strike) {
    double finalPrice = prices.back();  // Get the final price in the path
    double minPrice = *std::min_element(prices.begin(), prices.end());  // Find the minimum price in the path
    return finalPrice - minPrice;  // Lookback call payoff
}
