//This is the file grid.cpp
#include "grid.h"
grid::grid(double r_, double sigma_, double T_,
double S0_, double Smax, double Smin,
int N_, int imax_)
{
r=r_;
sigma=sigma_;
T=T_;
S0=S0_;
N=N_;
imin=0;
imax=imax_;
i0=imax*S0/(Smax-Smin);
k=T/N;
h=(S0-Smin)/i0;
}
double grid::a(int i)
{
return k*S(i)*(sigma*sigma*S(i)/h - r)/(2.*h);
}
double grid::b(int i)
{
return 1. - k*(sigma*sigma*S(i)*S(i)/h/h + r);
}
double grid::c(int i)
{
return k*S(i)*(sigma*sigma*S(i)/h + r)/(2.*h);
}
