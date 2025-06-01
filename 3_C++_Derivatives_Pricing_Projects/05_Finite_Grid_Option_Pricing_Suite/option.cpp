#include "option.h"
#include <vector>
#include <iostream>
using namespace std;

double EurOption::Price(grid G)
{
    int imax = G.Getimax();
    int imin = G.Getimin();
    vector<double> V(imax + 1);
    vector<double> V2(imax + 1);
    for (int i = imin; i <= imax; i++)
    {
        V[i] = Payoff(G.S(i));
    }
    for (int j = G.GetN() - 1; j >= 0; j--)
    {
        V2[imin] = f(G, j);
        V2[imax] = g(G, j);
        for (int i = imin + 1; i <= imax - 1; i++)
        {
            V2[i] = G.a(i) * V[i - 1] + G.b(i) * V[i] + G.c(i) * V[i + 1];
        }
        V = V2;
    }
    return V[G.Geti0()];
}

double AmOption::Price(grid G)
{
    int imax = G.Getimax();
    int imin = G.Getimin();
    vector<double> V(imax + 1);
    vector<double> V2(imax + 1);
    for (int i = imin; i <= imax; i++)
    {
        V[i] = Payoff(G.S(i));
    }
    for (int j = G.GetN() - 1; j >= 0; j--)
    {
        V2[imin] = f(G, j);
        V2[imax] = g(G, j);
        for (int i = imin + 1; i <= imax - 1; i++)
        {
            V2[i] = G.a(i) * V[i - 1] + G.b(i) * V[i] + G.c(i) * V[i + 1];
            V2[i] = std::max(V2[i], Payoff(G.S(i))); // Adjust for American option
        }
        V = V2;
    }
    return V[G.Geti0()];
}
