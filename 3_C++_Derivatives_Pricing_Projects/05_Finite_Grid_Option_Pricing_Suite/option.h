#ifndef option_h
#define option_h
#include "grid.h"
#include <cmath>
#include <algorithm>

class Option
{
public:
    virtual double Payoff(double S) = 0;
    virtual double f(grid G, int j) = 0;
    virtual double g(grid G, int j) = 0;
};

class EurOption : public virtual Option
{
public:
    double Price(grid G);
};

class AmOption : public virtual Option
{
public:
    double Price(grid G);
};

class Call : public EurOption, public AmOption
{
private:
    double K;
    double Payoff(double S) { return std::max(S - K, 0.0); }
    double g(grid G, int j) { return G.S(G.Getimax()) - K * exp(-G.Getr() * (G.GetT() - G.t(j))); }
    double f(grid G, int j) { return 0.0; }

public:
    Call(double K_) { K = K_; }
};

class Put : public EurOption, public AmOption
{
private:
    double K;
    double Payoff(double S) { return std::max(K - S, 0.0); }
    double g(grid G, int j) { return 0.0; }
    double f(grid G, int j) { return K * exp(-G.Getr() * (G.GetT() - G.t(j))); }

public:
    Put(double K_) { K = K_; }
};

#endif
