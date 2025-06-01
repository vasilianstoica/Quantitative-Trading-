//This is file Main10.cpp
#include <vector>
#include "BinModel02.h"
#include "EurOption05.h"
#include <iostream>
#include <cmath>
#include "Logger.h"
#include "OptionStrategy.h"

using namespace std;

int main()
{
    CSVLogger log("session_output.csv");
    BinModel model;
    OptionStrategyRunner runner;
    runner.Run(model);
    return 0;
}
