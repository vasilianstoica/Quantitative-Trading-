// ---------------------------------------------------------------------------
// OptionStrategy.cpp
// ---------------------------------------------------------------------------
#include "OptionStrategy.h"

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

// ------------------------- helper implementations ---------------------------

double Strategy_Value(BinModel& Model,
                      double X,
                      double Y,
                      int n,
                      int i)
{
    double R = Model.GetR();
    double bond = (n == 0) ? Y
                 : (n == 1) ? Y * R
                 :             Y * std::pow(R, n);

    return X * Model.S(n, i) + bond;
}


void Check_Balance(double balance)
{
    if (balance < 0) {
        cout << "Balance: " << balance << " (You are short of funds!!!)" << endl;
    } else if (balance == 0) {
        cout << "Balance: " << balance << " (Your balance is just right!)" << endl;
    } else {
        cout << "Balance: " << balance << " (Congratulations, you have more funds than you need to replicate this strategy!)" << endl;
    }
}

void Adjust_Strategy(double total_stock_position,
                     double total_cash_position,
                     double X,
                     double Y)
{
    if (X < total_stock_position) {
        cout << "You need to buy stocks " << (total_stock_position - X) << endl;
    } else if (X > total_stock_position) {
        cout << "You need to sell stocks " << (X - total_stock_position) << endl;
    }

    if (Y < total_cash_position) {
        cout << "You need to buy bonds " << (total_cash_position - Y) << endl;
    } else if (Y > total_cash_position) {
        cout << "You need to sell bonds " << (Y - total_cash_position) << endl;
    }
}

std::pair<double, double> HedgingPortfolio(EurOption& option,
                                           int n,
                                           int i,
                                           BinModel& Model)
{
    double R = Model.GetR();
    double Delta = (option.PriceByCRR(Model, n + 1, i + 1) -
                    option.PriceByCRR(Model, n + 1, i)) /
                   (Model.S(n + 1, i + 1) - Model.S(n + 1, i));

    double StockPosition = Delta;
    double CashPosition;

    if (n == 0) {
        CashPosition = option.PriceByCRR(Model, n, i) - Delta * Model.S(n, i);
    } else if (n == 1) {
        CashPosition = (option.PriceByCRR(Model, n, i) - Delta * Model.S(n, i)) / R;
    } else {
        CashPosition = (option.PriceByCRR(Model, n, i) - Delta * Model.S(n, i)) / pow(R, n);
    }

    return make_pair(StockPosition, CashPosition);
}

// -------------------------- runner implementation --------------------------

void OptionStrategyRunner::Run(BinModel& MyModel)
{
    // 1. Gather model parameters ---------------------------------------------
    if (MyModel.GetInputData() == 1) return; // abort on invalid input

    cout << "Enter the number of options to consider for your strategy : " << endl;
    int M; cout << "M = "; cin >> M;

    cout << "Enter the node in which to make computations: " << endl;
    int n; int i; cout << "n = "; cin >> n; cout << "i = "; cin >> i;

    vector<double> stock_position;
    vector<double> cash_position;

    // 2. Collect option data / compute hedging portfolios --------------------
    for (int j = 0; j < M; ++j) {
        string option_type; cout << "\n\nEnter option type (call or put): "; cin >> option_type;

        if (option_type == "call") {
            EurCall callOpt; callOpt.GetInputData();
            cout << "Option price = " << callOpt.PriceByCRR(MyModel, n, i) << "\n\n";
            auto hedge = HedgingPortfolio(callOpt, n, i, MyModel);
            cout << "Hedging Portfolio at step " << n << ", node " << i << ":\n";
            cout << "Delta (stock position): " << hedge.first << "\nBond (cash position): " << hedge.second << "\n";
            stock_position.push_back(hedge.first); cash_position.push_back(hedge.second);
        } else if (option_type == "put") {
            EurPut putOpt; putOpt.GetInputData();
            cout << "Option price = " << putOpt.PriceByCRR(MyModel, n, i) << "\n\n";
            auto hedge = HedgingPortfolio(putOpt, n, i, MyModel);
            cout << "Hedging Portfolio at step " << n << ", node " << i << ":\n";
            cout << "Delta (stock position): " << hedge.first << "\nBond (cash position): " << hedge.second << "\n";
            stock_position.push_back(hedge.first); cash_position.push_back(hedge.second);
        } else {
            cout << "Invalid option type. Please enter \"call\" or \"put\"." << endl; --j; // retry
        }
    }

    // 3. Aggregate hedge ------------------------------------------------------
    double total_stock_position = 0.0, total_cash_position = 0.0;
    for (size_t k = 0; k < stock_position.size(); ++k) {
        total_stock_position += stock_position[k];
        total_cash_position  += cash_position[k];
    }

    cout << "\n\nTotal Delta (stock position) required: " << total_stock_position << "\n";
    cout << "Total Bond (cash position) required: " << total_cash_position  << "\n";

    // 4. User portfolio evaluation -------------------------------------------
    double X, Y; cout << "Enter your position in stocks X and in the bonds Y at node (" << n << ", " << i << "):" << endl;
    cout << "X = "; cin >> X; cout << "Y = "; cin >> Y;

    double user_value  = Strategy_Value(MyModel, X, Y, n, i);
    cout << "\nAT NODE (" << n << ", " << i << "):" << endl;
    cout << "The value of your position (" << X << ", " << Y << ") is: " << user_value << "\n";

    double ideal_value = Strategy_Value(MyModel, total_stock_position, total_cash_position, n, i);
    cout << "\nFor replication of all options you need: " << ideal_value << endl;
    Check_Balance(user_value - ideal_value);

    cout << "\nTo replicate this strategy perfectly you need to: " << endl;
    Adjust_Strategy(total_stock_position, total_cash_position, X, Y);

    cout << "\n\nEnter any key to end the script "; char x; cin >> x;
}
