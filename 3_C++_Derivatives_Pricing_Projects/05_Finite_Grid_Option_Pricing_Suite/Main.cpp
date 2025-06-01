#include <iostream>
#include "grid.h"
#include "option.h"
#include "replication.h"
#include "BSModel.h"
#include "MCOption.h"
#include "Logger.h"
using namespace std;

int main()
{
    // Parameters
    double r = 0.05;          // risk-free rate
    double T = 1.0 / 12.0;    // time to maturity (1 month)
    double S0 = 100.0;        // initial stock price
    double sigma = 0.2;       // volatility
    double Smin = 2.0;        // minimum stock price in grid
    double Smax = 200.0;      // maximum stock price in grid
    double K = 100.0;         // strike price
    int N = 2000;             // number of time steps
    int N_Asian = 30;             // number of time steps
    int imax = 700;           // number of price steps
    long SIM = 100000;        // number of simulations
    double epsilon = 0.001;
    CSVLogger log("session_output.csv");

    // Initialize grid
    grid Eurgrid(r, sigma, T, S0, Smax, Smin, N, imax);
    BSModel model(S0, r, sigma, T);
    // Initialize Call and Put options
    Call call(K);
    Put put(K);
    // Compute and display the European option prices and replicating strategy
    double callPrice = call.EurOption::Price(Eurgrid);
    double putPrice = put.EurOption::Price(Eurgrid);
    cout << "Price of the European Call Option = " << callPrice << endl;
    Replication::computeReplicationStrategyEUR(r, sigma, T, S0, Smax, Smin, N, imax, epsilon, K, call);
    cout << "Price of the European Put Option = " << putPrice << endl;
    Replication::computeReplicationStrategyEUR(r, sigma, T, S0, Smax, Smin, N, imax, epsilon, K, put);

    // Compute and display the American option prices and replicating strategy
    double AmCallPrice = call.AmOption::Price(Eurgrid);
    double AmPutPrice = put.AmOption::Price(Eurgrid);
    cout << "Price of the American Call Option = " << AmCallPrice << endl;
    Replication::computeReplicationStrategyAM(r, sigma, T, S0, Smax, Smin, N, imax, epsilon, K, call);
    cout << "Price of the American Put Option = " << AmPutPrice << endl;
    Replication::computeReplicationStrategyAM(r, sigma, T, S0, Smax, Smin, N, imax, epsilon, K, put);
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    MCCall AsianCall(T, SIM, N_Asian, K);
    MCPut AsianPut(T, SIM, N_Asian, K);

    ReplicationResult replicationResultCall = AsianCall.ReplicationStrategy(model, SIM, epsilon);
    cout << "Price and Replication Strategy for Asian Call Option: H(0) = " << replicationResultCall.H << ", X = " << replicationResultCall.delta << ", Y = " << replicationResultCall.y << endl;
    double precisionCallAsian = AsianCall.GetPrecision();
    cout << "Precision = " << precisionCallAsian << endl;

    ReplicationResult replicationResultPut = AsianPut.ReplicationStrategy(model, SIM, epsilon);
    cout << "Price and Replication Strategy for Asian Put Option: H(0) = " << replicationResultPut.H << ", X = " << replicationResultPut.delta << ", Y = " << replicationResultPut.y << endl;
    double precisionPutAsian = AsianPut.GetPrecision();
    cout << "Precision = " << precisionPutAsian << endl;

    return 0;
}


