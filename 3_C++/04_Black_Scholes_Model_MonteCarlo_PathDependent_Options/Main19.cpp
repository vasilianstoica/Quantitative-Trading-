#include <iostream>
#include "PathDepOption01.h"
#include "Logger.h"

using namespace std;
void GetInputData(double& S0, double& r, double& sigma, double& MU) {
   cout << "Enter General Option Data " << endl;
   cout << "Enter S0: "; cin >> S0;
   cout << "Enter r:  "; cin >> r;
   cout << "Enter sigma:  "; cin >> sigma;
   cout << "Enter MU:  "; cin >> MU;
   cout << endl;
}

void AsianOptionData(double& T, double& K, int& m) {
   cout << "Asian Call Option Data " << endl;
   cout << "Enter T: "; cin >> T;
   cout << "Enter K:  "; cin >> K;
   cout << "Enter m:  "; cin >> m;
   cout << endl;
}

void LookbackOptionData(double& T, int& m) {
   cout << endl;
   cout << endl;
   cout << "Loockback Option Data " << endl;
   cout << "Enter T: "; cin >> T;
   cout << "Enter m:  "; cin >> m;
   cout << endl;
}

void EurCallOptionData(double& T, double& K) {
   cout << endl;
   cout << endl;
   cout << "Eur Call Option Data " << endl;
   cout << "Enter T: "; cin >> T;
   cout << "Enter K:  "; cin >> K;
   cout << endl;
}

void EurPutOptionData(double& T, double& K) {
   cout << endl;
   cout << endl;
   cout << "Eur Put Option Data " << endl;
   cout << "Enter T: "; cin >> T;
   cout << "Enter K:  "; cin >> K;
   cout << endl;
}

void ButterflyOptionData(double& T, double& K1, double& K2) {
   cout << endl;
   cout << endl;
   cout << "Butterfly Option Data " << endl;
   cout << "Enter T: "; cin >> T;
   cout << "Enter K:  "; cin >> K1;
   cout << "Enter K:  "; cin >> K2;
   cout << endl;
}

int main()
{
   double S0, r, sigma, T, K, MU;
   int m;
   double epsilon = 0.001;
   long N=1000000;
   CSVLogger log("session_output.csv");
   GetInputData(S0, r, sigma, MU);
   BSModel Model(S0,r,sigma);

   EurCallOptionData(T, K);
   EurCall CallOptionBS(T,K);
   cout << "Eur Call Price with BS = " << CallOptionBS.PriceBSFormulaCall(Model, S0, r, sigma, T, K) << endl << endl;

   EurCall CallOptionMC(T,K);
   pair<double, double> replication = CallOptionMC.ReplicationStrategy(Model, N, epsilon);
   double delta = replication.first;
   double y = replication.second;
   cout << "Eur Call Price with MC = " << CallOptionMC.PriceByMC(Model,N) << endl << "Pricing Error = " << CallOptionMC.PricingError << endl;
   cout << "Replication Strategy (Delta) = " << delta << endl;
   cout << "Replication Strategy (y) = " << y << endl;

   EurPutOptionData(T, K);
   EurPut PutOptionBS(T,K);
   cout << "Eur Put Price with BS = " << PutOptionBS.PriceBSFormulaPut(Model, S0, r, sigma, T, K) << endl << endl;

   EurPut PutOptionMC(T,K);
   pair<double, double> replication_put = PutOptionMC.ReplicationStrategy(Model, N, epsilon);
   double delta_put = replication_put.first;
   double y_put = replication_put.second;
   cout << "Eur Put Price with MC = " << PutOptionMC.PriceByMC(Model,N) << endl << "Pricing Error = " << PutOptionMC.PricingError << endl;
   cout << "Replication Strategy (Delta) = " << delta_put << endl;
   cout << "Replication Strategy (y) = " << y_put << endl;

   double K1, K2;
   ButterflyOptionData(T, K1, K2);
   Butterfly ButterflyOptionMC(T, K1, K2);
   pair<double, double> replication_butt = ButterflyOptionMC.ReplicationStrategy(Model, N, epsilon);
   double delta_butt = replication_butt.first;
   double y_butt = replication_butt.second;
   cout << "Butterfly Price with MC = " << ButterflyOptionMC.PriceByMC(Model,N) << endl << "Pricing Error = " << ButterflyOptionMC.PricingError << endl;
   cout << "Replication Strategy (Delta) = " << delta_butt << endl;
   cout << "Replication Strategy (y) = " << y_butt << endl;

   cout << "Butterfly Price with MC = " << ButterflyOptionMC.PriceByMC(Model,N) << endl << "Pricing Error = " << ButterflyOptionMC.PricingError << endl;

   AsianOptionData(T, K, m);
   ArthmAsianCall Option(T,K,m);
   cout << "Asian Call Price = " << Option.PriceByMC(Model,N) << endl << "Pricing Error = " << Option.PricingError << endl;

   LookbackOptionData(T, m);
   LookbackOption Option1(T,m);
   cout << "Lookback Price = " << Option1.PriceByMC(Model,N) << endl << "Pricing Error = " << Option1.PricingError << endl;

   return 0;
}
