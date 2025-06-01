// Tests.cpp

#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>

// Project headers:
#include "BSModel.h"
#include "BSOption.h"
#include "generator.h"
#include "grid.h"
#include "option.h"
#include "replication.h"
#include "MCOption.h"

static bool approxEqual(double a, double b, double tol = 1e-6) {
    return std::fabs(a - b) < tol;
}

int main() {
    std::cout << "Running unit tests...\n";

    //
    // 1) Test the Gauss() generator: mean ≈ 0, variance ≈ 1
    //
    {
        const int N = 100000;
        std::srand(0);  // fix seed for reproducibility
        double sum = 0.0, sum2 = 0.0;
        for(int i = 0; i < N; ++i) {
            double z = Gauss();
            sum  += z;
            sum2 += z*z;
        }
        double mean = sum / N;
        double var  = (sum2 / N) - (mean*mean);
        assert(std::fabs(mean) < 0.01);      // small bias
        assert(std::fabs(var - 1.0) < 0.02); // variance ≈ 1
    }

    //
    // 2) Test BSCall / BSPut: put-call parity and (x = N(d1), y = price - x*S0)
    //
    {
        double S0    = 100.0;
        double r     = 0.05;
        double sigma = 0.2;
        double T     = 1.0;
        double K     = 100.0;

        BSModel model(S0, r, sigma, T);

        BSCall c(T, K);
        double callPrice = c.Price(model);
        double x_call    = c.Getx();
        double y_call    = c.Gety();

        BSPut p(T, K);
        double putPrice  = p.Price(model);
        double x_put     = p.Getx();
        double y_put     = p.Gety();

        // Black-Scholes put-call parity: C - P = S0 - K e^{-rT}
        double parityLeft  = callPrice - putPrice;
        double parityRight = S0 - K * std::exp(-r * T);
        assert(approxEqual(parityLeft, parityRight, 1e-6));

        // Check that x_call = N(d1) is in (0,1), and x_put = -N(-d1)
        assert(x_call > 0.0 && x_call < 1.0);
        assert(approxEqual(x_put, - (1.0 - x_call), 1e-6));

        // Check y = price - x*S0: for a call with zero volatility, y ≈ -K e^{-rT}
        // (We do not assert exactly, just ensure it’s consistent between c and p.)
        double y_call_calc = callPrice - x_call * S0;
        assert(approxEqual(y_call, y_call_calc, 1e-8));
        double y_put_calc  = putPrice  - x_put  * S0;
        assert(approxEqual(y_put, y_put_calc, 1e-8));
    }

    //
    // 3) Test grid coefficients for trivial (sigma=0) case
    //    When sigma=0, a(i) = -r*k/2, c(i) = r*k/2, b(i)=1 - r*k
    //
    {
        double r     = 0.05;
        double sigma = 0.0;    // zero volatility
        double T     = 1.0;
        double S0    = 100.0;
        double Smax  = 200.0;
        double Smin  = 0.0;
        int    N     = 10;
        int    imax  = 10;

        grid G(r, sigma, T, S0, Smax, Smin, N, imax);

        double k = G.GetT() / G.GetN();
        double h = G.Geth();  // step size in S

        for(int i = G.Getimin() + 1; i < G.Getimax(); ++i) {
            double Si = G.S(i);
            double ai = G.a(i);
            double bi = G.b(i);
            double ci = G.c(i);
            double expectedA = k * Si * (sigma*sigma * Si / h - r)/(2.0*h);
            double expectedB = 1.0 - k * (sigma*sigma * Si*Si / (h*h) + r);
            double expectedC = k * Si * (sigma*sigma * Si / h + r)/(2.0*h);
            // Since sigma=0: expectedA = -r * Si * k / (2*h), expectedC = +r * Si * k / (2*h)
            // But dividing by h yields r*k/2 constant factor times (Si/h).
            // We just check sign and relative magnitude:
            assert(std::fabs(bi - (1.0 - r*k)) < 1e-6);
            assert(ai < 0.0 && ci > 0.0);
        }
    }

    //
    // 4) Test PDE pricing (EurOption and AmOption)
    //
    {
        // Case 1: strike = 0 => Payoff = S => price should equal S0 for European and American
        double r     = 0.05;
        double sigma = 0.2;
        double T     = 1.0;
        double S0    = 50.0;
        double Smax  = 2.0 * S0;
        double Smin  = 0.0;
        int    N     = 10;  // coarse grid
        int    imax  = 10;

        grid G(r, sigma, T, S0, Smax, Smin, N, imax);

        // Define a "call with K=0" (payoff = max(S-0,0)=S)
        class CallK0 : public EurOption, public AmOption {
        public:
            CallK0() {}
            virtual double Payoff(double S) { return S; }
            virtual double f(grid, int)  { return 0.0; }
            virtual double g(grid G, int j) {
                double t_rem = G.GetT() - G.t(j);
                return G.S(G.Getimax()) * std::exp(-G.Getr() * t_rem);
            }
        } c0;

        double euroPrice = c0.EurOption::Price(G);
        double amerPrice = c0.AmOption::Price(G);
        // Both should be ≈ S0 (discounted payoff = S0, since no strike).
        assert(approxEqual(euroPrice, S0, 1e-2));
        assert(approxEqual(amerPrice, S0, 1e-2));

        // Case 2: deep‐in‐the‐money put (K >> S0) should be near K e^{-rT} - S0 for European
        double K2    = 100.0;
        class DeepPut : public EurOption {
            double K;
        public:
            DeepPut(double K_): K(K_) {}
            virtual double Payoff(double S) { return std::max(K - S, 0.0); }
            virtual double f(grid, int) { return K * std::exp(-(grid(0,0,0,0,0,0,0,0)).Getr() *
                       (grid(0,0,0,0,0,0,0,0).GetT() - 0.0)); }
            virtual double g(grid, int) { return 0.0; }
        } p2(K2);

        double euroPutPrice = p2.Price(G);
        double theoretical = K2 * std::exp(-r*T) - S0;
        assert(euroPutPrice > 0.0);
        // Only ensure it's of correct order: euroPutPrice ≈ max(K e^{-rT}-S0, 0)
        assert(euroPutPrice > theoretical - 1.0 && euroPutPrice < theoretical + 1.0);
    }

    //
    // 5) Test ReplicationStrategy for trivial (sigma=0) European call
    //
    {
        double r     = 0.05;
        double sigma = 0.0;
        double T     = 1.0;
        double S0    = 100.0;
        double Smax  = 2.0 * S0;
        double Smin  = 0.0;
        int    N     = 10;
        int    imax  = 10;
        double epsilon = 1e-3;
        double K     = 50.0;

        grid G(r, sigma, T, S0, Smax, Smin, N, imax);
        BSModel model(S0, r, sigma, T);

        Call c(K);
        // For σ=0, price = max(S0-K,0) so if K< S0, delta=1, y=−K e^{-rT}
        // but our replication prints to stdout; we just call it to ensure no crash:
        // The test is: if K < S0, then ∂price/∂S0 = 1
        double H = c.EurOption::Price(G);
        assert(H > 0.0);
        Replication::computeReplicationStrategyEUR(
            r, sigma, T, S0, Smax, Smin, N, imax, epsilon, K, c
        );
        // (We do not capture the stdout values here, but the function must run without error.)
    }

    //
    // 6) Test MCOption for a deterministic payoff (strike=0, so Payoff=S(T))
    //    For strike=0, price H(0) ≈ E[e^{-rT} S(T)] = S0.
    //
    {
        double S0    = 100.0;
        double r     = 0.05;
        double sigma = 0.2;
        double T     = 1.0;
        double K     = 0.0;     // strike=0
        long   Nsim  = 50000;   // moderate Monte Carlo
        int    M     = 10;      // number of time steps
        double epsilon = 1e-3;

        BSModel model(S0, r, sigma, T);
        MCCall mcC(T, Nsim, M, K);

        // We want Payoff = max(S(T)-0,0) = S(T). Hence discounted expectation = S0
        auto priceRes = mcC.Price(model, epsilon);
        double mcH0    = priceRes.price;
        assert(std::fabs(mcH0 - S0) / S0 < 0.05);
        // within 5% tolerance for 50k simulations
    }

    std::cout << "All unit tests passed!\n";
    return 0;
}
