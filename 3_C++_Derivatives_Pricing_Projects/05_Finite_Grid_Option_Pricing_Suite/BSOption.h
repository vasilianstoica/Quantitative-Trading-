//This is the file BSOption.h
#ifndef BSOption_h
#define BSOption_h

class BSCall
{
private:
double x,y,T,X;
public:
double Price(BSModel stock);
double Getx(){return x;}
double Gety(){return y;}
BSCall(double T_, double X_){ T=T_; X=X_;}
};
class BSPut
{
private:
double x,y,T,X;
public:
double Price(BSModel stock);
double Getx(){return x;}
double Gety(){return y;}
BSPut(double T_, double X_){ T=T_; X=X_;}
};
#endif
