#ifndef REPLICATION_H
#define REPLICATION_H

#include "grid.h"
#include "option.h"

class Replication {
public:
    static void computeReplicationStrategyEUR(double r, double sigma, double T, double S0, double Smax, double Smin, int N, int imax, double epsilon, double K, EurOption& option);
    static void computeReplicationStrategyAM(double r, double sigma, double T, double S0, double Smax, double Smin, int N, int imax, double epsilon, double K, AmOption& option);
};

#endif // REPLICATION_H
