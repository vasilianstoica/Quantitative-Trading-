//This is the file MCOption.h
#ifndef MCOption_h
#define MCOption_h
#include "BSModel.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <vector>

struct ReplicationResult {
    double H;
    double delta;
    double y;
};

struct PriceResult {
    double price;
    double H_plus;
    double H_minus;
};

class MCOption
{
private:
double T, Precision, S0;
long N;
public:
virtual double Payoff(BSModel stock)=0;
virtual double ControlVariate(BSModel stock)=0;
long GetN(){ return N;}
double GetT(){ return T;}
double GetPrecision(){return Precision;}
void SetT(double T_){T=T_;}
void SetN(long N_){N=N_;}
void SetPrecision(double P){Precision = P;}
ReplicationResult ReplicationStrategy(BSModel Model, long N, double epsilon);
};
class MCAsOption: public MCOption
{
private:
int NofSteps;
public:
PriceResult Price(BSModel model, double epsilon);
int GetNofSteps(){return NofSteps;}
void SetNofSteps(int M){NofSteps = M;}
};
class MCCall: public MCAsOption
{
private:
double X;
public:
double Payoff(BSModel stock);
double ControlVariate(BSModel stock){return 0.;}
MCCall(double T, long N, int M, double X_)
{ SetT(T); SetN(N); SetNofSteps(M); X=X_; srand((unsigned)time(0));}
};
class MCPut: public MCAsOption
{
private:
double X;
public:
double Payoff(BSModel stock);
double ControlVariate(BSModel stock){return 0.;}
MCPut(double T, long N, int M, double X_)
{ SetT(T); SetN(N); SetNofSteps(M); X=X_; srand((unsigned)time(0));}
};
#endif
