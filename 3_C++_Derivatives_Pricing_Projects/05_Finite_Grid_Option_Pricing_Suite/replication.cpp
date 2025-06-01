#include <iostream>
#include "grid.h"
#include "option.h"
#include "replication.h"

using namespace std;

void Replication::computeReplicationStrategyEUR(double r, double sigma, double T, double S0, double Smax, double Smin, int N, int imax, double epsilon, double K, EurOption& option) {
    double H = static_cast<EurOption&>(option).Price(grid(r, sigma, T, S0, Smax, Smin, N, imax));

    double H_plus, H_minus, delta, y;

    grid Eurgrid_plus(r, sigma, T, S0 * (1 + epsilon), Smax, Smin, N, imax);
    if (dynamic_cast<Call*>(&option)) {
        Call option_plus(K);
        H_plus = static_cast<EurOption&>(option_plus).Price(Eurgrid_plus);
    } else {
        Put option_plus(K);
        H_plus = static_cast<EurOption&>(option_plus).Price(Eurgrid_plus);
    }

    grid Eurgrid_minus(r, sigma, T, S0 * (1 - epsilon), Smax, Smin, N, imax);
    if (dynamic_cast<Call*>(&option)) {
        Call option_minus(K);
        H_minus = static_cast<EurOption&>(option_minus).Price(Eurgrid_minus);
    } else {
        Put option_minus(K);
        H_minus = static_cast<EurOption&>(option_minus).Price(Eurgrid_minus);
    }

    delta = (H_plus - H_minus) / (2 * epsilon * S0);
    y = H - delta * S0;
    cout << "Replication Strategy (Delta) = " << delta << endl;
    cout << "Replication Strategy (y) = " << y << endl;
}

void Replication::computeReplicationStrategyAM(double r, double sigma, double T, double S0, double Smax, double Smin, int N, int imax, double epsilon, double K, AmOption& option) {
    double H = static_cast<AmOption&>(option).Price(grid(r, sigma, T, S0, Smax, Smin, N, imax));

    double H_plus, H_minus, delta, y;

    grid Eurgrid_plus(r, sigma, T, S0 * (1 + epsilon), Smax, Smin, N, imax);
    if (dynamic_cast<Call*>(&option)) {
        Call option_plus(K);
        H_plus = static_cast<AmOption&>(option_plus).Price(Eurgrid_plus);
    } else {
        Put option_plus(K);
        H_plus = static_cast<AmOption&>(option_plus).Price(Eurgrid_plus);
    }

    grid Eurgrid_minus(r, sigma, T, S0 * (1 - epsilon), Smax, Smin, N, imax);
    if (dynamic_cast<Call*>(&option)) {
        Call option_minus(K);
        H_minus = static_cast<AmOption&>(option_minus).Price(Eurgrid_minus);
    } else {
        Put option_minus(K);
        H_minus = static_cast<AmOption&>(option_minus).Price(Eurgrid_minus);
    }

    delta = (H_plus - H_minus) / (2 * epsilon * S0);
    y = H - delta * S0;
    cout << "Replication Strategy (Delta) = " << delta << endl;
    cout << "Replication Strategy (y) = " << y << endl;
}
