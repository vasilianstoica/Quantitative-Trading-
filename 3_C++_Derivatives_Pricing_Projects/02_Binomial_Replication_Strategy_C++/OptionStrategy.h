// OptionStrategy.h
#ifndef OPTION_STRATEGY_H
#define OPTION_STRATEGY_H

#include "BinModel02.h"
#include "EurOption05.h"
#include <utility>

// ---------------------------------------------------------------------------
// Helper functions that used to be methods of EurOption ----------------------
// ---------------------------------------------------------------------------

// Value of a (stock,bond) portfolio at node (n,i)
double Strategy_Value(BinModel& Model,
                      double X,
                      double Y,
                      int n,
                      int i);

// Prints whether the user is under‑/over‑/perfectly‑funded
void Check_Balance(double balance);

// Prints the trades required to reach (total_stock_position,total_cash_position)
void Adjust_Strategy(double total_stock_position,
                     double total_cash_position,
                     double X,
                     double Y);

// Delta‑hedging portfolio (stock & cash) for *any* European option
std::pair<double, double> HedgingPortfolio(EurOption& option,
                                           int n,
                                           int i,
                                           BinModel& Model);

// ---------------------------------------------------------------------------
// The interactive driver -----------------------------------------------------
// ---------------------------------------------------------------------------
class OptionStrategyRunner {
public:
    void Run(BinModel& model);
};

#endif // OPTION_STRATEGY_H
