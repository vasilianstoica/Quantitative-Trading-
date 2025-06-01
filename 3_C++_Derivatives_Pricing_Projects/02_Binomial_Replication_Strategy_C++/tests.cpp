// tests.cpp  ---------------------------------------------------------------
#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include "BinModel02.h"
#include "EurOption05.h"
#include "OptionStrategy.h"

#include <algorithm>   // for std::max

TEST_CASE("Risk-neutral probability & stock prices")
{
    BinModel m(100, 1.2, 0.8, 1.1);                  // S0, U, D, R
    REQUIRE( m.RiskNeutProb() ==
             Catch::Approx( (1.1 - 0.8) / (1.2 - 0.8) ) );

    REQUIRE( m.S(1,0) == Catch::Approx(100 * 0.8) );
    REQUIRE( m.S(2,1) == Catch::Approx(100 * 1.2 * 0.8) );
}

TEST_CASE("One-step European call price")
{
    BinModel m(100, 1.2, 0.8, 1.1);
    EurCall  c;  c.SetN(1);  c.SetK(100);

    double p = m.RiskNeutProb();
    double expected =
        (  p   * std::max(m.S(1,1) - 100.0, 0.0)
         + (1-p)* std::max(m.S(1,0) - 100.0, 0.0) ) / 1.1;

    REQUIRE( c.PriceByCRR(m,0,0) ==
             Catch::Approx(expected).margin(1e-12) );
}

TEST_CASE("Hedging portfolio reproduces option value")
{
    BinModel m(100, 1.2, 0.8, 1.1);
    EurCall  c;  c.SetN(1);  c.SetK(100);

    auto hp  = HedgingPortfolio(c, 0, 0, m);             // (Δ , cash)
    double v = Strategy_Value(m, hp.first, hp.second, 0, 0);

    REQUIRE( v == Catch::Approx(c.PriceByCRR(m,0,0)).margin(1e-12) );
}
