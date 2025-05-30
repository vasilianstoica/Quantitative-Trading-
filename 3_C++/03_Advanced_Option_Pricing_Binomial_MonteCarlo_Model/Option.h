// Option.h
#ifndef Option_h
#define Option_h
#include <functional>  // Include this for std::function
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>


class Option {
public:
    static double computeAsianCallPayoff(const std::vector<double>& prices, double strike);
    static double computeLookbackCallPayoff(const std::vector<double>& prices, double strike);
private:
    // Optionally add private methods or attributes if needed
};

#endif
