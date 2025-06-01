//This is the file grid.h
#ifndef grid_h
#define grid_h
class grid
{
private:
double r, mu, sigma, T, S0;
double k, h;
int N, imax, imin, i0;
public:
double S(int i){return S0 + (i-i0)*h;}
double t(int j){return j*k;}
double a(int i);
double b(int i);
double c(int i);
int Getimax(){return imax;}
int Getimin(){return imin;}
int GetN(){return N;}
int Geti0(){return i0;}
double Getr(){return r;}
double GetT(){return T;}
double Geth(){return h;}
double GetS0(){return S0;}
grid(double r, double sigma_,
double T_, double S0_, double Smax,
double Smin, int N_, int Idelta);
};
#endif
