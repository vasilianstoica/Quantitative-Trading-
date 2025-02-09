import pandas as pd

from IterativeBaseMASR import *
import talib as ta
from itertools import product

class IterativeBacktest(IterativeBase):
    ''' Class for iterative (event-driven) backtesting of trading strategies.
    '''
    def __init__(self, symbol, start, end, amount, SMA_Trend, EMA_Trend, PCT_BUY, PCT_SELL, stop_loss, Keep_Signal_1, Keep_Signal_2, Keep_Signal_3, Keep_Signal_4, leverage, fee):
        super().__init__(symbol, start, end, amount, fee)
        self.SMA_Trend = SMA_Trend
        self.EMA_Trend = EMA_Trend
        self.PCT_BUY = PCT_BUY
        self.PCT_SELL = PCT_SELL
        self.stop_loss = stop_loss
        self.leverage = leverage
        self.get_data_indicators_a()
        #self.get_data_smamacd_b()
        self.amount = amount
        self.Keep_Signal_1 = Keep_Signal_1
        self.Keep_Signal_2 = Keep_Signal_2
        self.Keep_Signal_3 = Keep_Signal_3
        self.Keep_Signal_4 = Keep_Signal_4
        self.stop = False

    # helper method
    def go_long(self, bar, units=None, amount=None):
        if self.position == -1:
            self.buy_instrument(bar, units=-self.units)  # if short position, go neutral first
        if units:
            self.buy_instrument(bar, units=units)

        elif self.current_balance > 10:
            self.buy_instrument(bar, amount=self.current_balance * self.leverage)  # go long
        else:
            self.position = 0  # neutral position
            self.stop_loss_level = 0
            print("Not enough balance to open position")
            self.stop = True

    def opt_go_long(self, bar, units=None, amount=None):
        if self.position == -1:
            self.opt_buy_instrument(bar, units=-self.units)  # if short position, go neutral first
        if units:
            self.opt_buy_instrument(bar, units=units)

        elif self.current_balance > 10:
            self.opt_buy_instrument(bar, amount=self.current_balance * self.leverage)  # go long
        else:
            self.position = 0  # neutral position
            self.stop_loss_level = 0
            self.stop = True
        
    def get_data_indicators_a(self):
        raw = self.data
        raw["SMA_Trend"] = ta.SMA(raw["Close"], timeperiod=self.SMA_Trend)
        raw["EMA_Trend"] = ta.EMA(raw["Close"], timeperiod=self.EMA_Trend)
        self.data = raw


    #def get_data_smamacd_b(self):
        #raw = self.data_b
        #raw["SMA_Trend"] = ta.SMA(raw["Close"], timeperiod=self.SMA_Trend)
        #self.data_b = raw

    def set_parameters(self, SMA_Trend=None, EMA_Trend = None):
        ''' Updates parameters
        '''
        if SMA_Trend is not None:
            self.SMA_Trend = SMA_Trend

        if EMA_Trend is not None:
            self.EMA_Trend = EMA_Trend

    def test_strategy(self):
        ''' 
        Backtests an WILLR+MACD strategy.
        
        Parameters
        ----------
        stop_loss: int
            stop loss, it is actually in percetnage so 10 = 10%
        '''
        
        # nice printout
        #self.stop_loss = self.stop_loss/100
        #self.PCT_BUY = self.PCT_BUY / 100
        #self.PCT_SELL = self.PCT_SELL/100
        
        stm = "Testing MASR {} | SMA_Trend = {} EMA_Trend = {} PCT_BUY = {} PCT_SELL = {} stop_loss = {}%".format(self.symbol, self.SMA_Trend, self.EMA_Trend, self.PCT_BUY, self.PCT_SELL, self.stop_loss)
        print("-" * 75)
        print(stm)
        print("-" * 75)
        
        # reset 
        self.position = 0  # initial neutral position
        self.trades = 0  # no trades yet
        self.current_balance = self.initial_balance  # reset initial capital
        self.stop_loss_level = 0
        
        # prepare data
        #self.data_b["SMA_Trend"] = ta.SMA(self.data_b["Close"], timeperiod=self.SMA_Trend)
        #self.data_b = self.data_b.resample("4h").last()
        #self.data_b = self.data_b.ffill()
        #self.data["SMA_Trend"] =  self.data_b["SMA_Trend"]

        self.data.dropna(inplace = True)
        self.cond_buy_1 = 0
        self.cond_buy_2 = 0
        self.cond_sell_1 = 0

        # MASR Strategy
        for bar in range(len(self.data)-1): # all bars (except the last bar)
############################################################## Buy conditions ####################################################
            if self.cond_buy_1 >= 1:
                self.signal1 = (self.data["Close"].iloc[bar] > self.data["SMA_Trend"].iloc[bar] and self.data["Low"].iloc[bar] < (self.data["SMA_Trend"].iloc[bar]*(1+self.PCT_BUY))) * self.Keep_Signal_1
                if self.signal1 == 0:
                    self.cond_buy_1 -= 1
                elif self.signal1 >= 1:
                    self.cond_buy_1 = self.signal1

            if self.cond_buy_1 == 0:
                self.cond_buy_1 = (self.data["Close"].iloc[bar] > self.data["SMA_Trend"].iloc[bar] and self.data["Low"].iloc[bar] < (self.data["SMA_Trend"].iloc[bar]*(1+self.PCT_BUY))) * self.Keep_Signal_1
                
            if self.cond_buy_2 >= 1:
                self.signal2 = (self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar]) * self.Keep_Signal_2
                if self.signal2 == 0:
                    self.cond_buy_2 -= 1
                elif self.signal2 >= 1:
                    self.cond_buy_2 = self.signal2

            if self.cond_buy_2 == 0:
                self.cond_buy_2 = (self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar]) * self.Keep_Signal_2
############################################################## Buy conditions ####################################################            
            if self.stop:
                break

            if self.position == 0:
                if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                    if self.position == 0:
                        self.position = 1  # long position
                        self.go_long(bar, amount = self.current_balance) # go long
                        self.entry_price = float(self.data.Open.iloc[bar+1])
                        if self.stop_loss_level == 0:
                            self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                        if self.data["Low"].iloc[bar+1] < self.stop_loss_level: # stop-loss is hit
                            self.close_instrument(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral with full amount
                            self.position = 0 # neutral position
                            self.stop_loss_level = 0

                    elif self.position == 1:
                        if self.data["Low"].iloc[bar] < self.stop_loss_level: # stop-loss is hit
                            self.close_instrument_two(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral with full amount
                            self.position = 0 # neutral position
                            self.stop_loss_level = 0
                            if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                                if self.position == 0:
                                    self.position = 1  # long position
                                    price = self.data.Close.iloc[bar]
                                    self.go_long(bar, amount = self.current_balance) # go long
                                    self.entry_price = float(self.data.Open.iloc[bar+1])
                                    if self.stop_loss_level == 0:
                                        self.stop_loss_level = price - (price * self.stop_loss)

            elif self.position == 1:
                if self.data["Low"].iloc[bar] < self.stop_loss_level: # stop-loss is hit
                    self.close_instrument_two(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral
                    self.position = 0 # neutral position
                    self.stop_loss_level = 0
                    if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                        if self.position == 0:
                            self.position = 1  # long position
                            price = self.data.Close.iloc[bar]
                            self.go_long(bar, amount = self.current_balance) # go long
                            self.entry_price = float(self.data.Open.iloc[bar+1])
                            if self.stop_loss_level == 0:
                                self.stop_loss_level = price - (price * self.stop_loss)

                elif self.position == 1:
                    if self.data["Close"].iloc[bar] < self.data["SMA_Trend"].iloc[bar]: # signal to go neutral
                        if self.position == 1:
                            self.sell_instrument(bar, "SMA Close", units = self.units) # go neutral with full amount
                            self.position = 0 # neutral position
                            self.stop_loss_level = 0
                            if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                                if self.position == 0:
                                    self.position = 1  # long position
                                    self.go_long(bar, amount = self.current_balance) # go long
                                    self.entry_price = float(self.data.Open.iloc[bar+1])
                                    if self.stop_loss_level == 0:
                                        self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                                    if self.data["Low"].iloc[bar+1] < self.stop_loss_level: # stop-loss is hit
                                        self.close_instrument(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral with full amount
                                        self.position = 0 # neutral position
                                        self.stop_loss_level = 0
                    
                    elif self.data["High"].iloc[bar] > (self.data["SMA_Trend"].iloc[bar]*(1+self.PCT_SELL)): # go neutral with full amount
                        if self.position == 1:
                            sell_price = float(self.data["SMA_Trend"].iloc[bar]*(1+self.PCT_SELL))
                            if self.entry_price > sell_price:
                                if self.data.High.iloc[bar] > (self.entry_price * 1.0):
                                    new_sell_price = self.entry_price * 1.0
                                    self.sell_instrument_sma_profit(bar, "SMA Profit Close", new_sell_price, units = self.units) # go neutral with full amount
                                    self.position = 0 # neutral position
                                    self.stop_loss_level = 0

                                    if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                                        if self.position == 0:
                                            self.position = 1  # long position
                                            self.go_long(bar, amount = self.current_balance) # go long
                                            if self.stop_loss_level == 0:
                                                self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                                            if self.data["Low"].iloc[bar+1] < self.stop_loss_level: # stop-loss is hit
                                                self.close_instrument(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral with full amount
                                                self.position = 0 # neutral position
                                                self.stop_loss_level = 0
                                        
                            else:
                                self.sell_instrument_sma_profit(bar, "SMA Profit Close", sell_price, units = self.units) # go neutral with full amount
                                self.position = 0 # neutral position
                                self.stop_loss_level = 0

                                if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                                    if self.position == 0:
                                        self.position = 1  # long position
                                        self.go_long(bar, amount = self.current_balance) # go long
                                        if self.stop_loss_level == 0:
                                            self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                                        if self.data["Low"].iloc[bar+1] < self.stop_loss_level: # stop-loss is hit
                                            self.close_instrument(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral with full amount
                                            self.position = 0 # neutral position
                                            self.stop_loss_level = 0

        if self.position != 0:
            self.close_pos(bar + 1)  # close position at the last bar
            perf = (self.current_balance - self.initial_balance) / self.initial_balance * 100
        else:
            date, Close, Open = self.get_values(bar + 1)
            perf = (self.current_balance - self.initial_balance) / self.initial_balance * 100
            self.trades += 1
            print(75 * "-")
            print("--- NO POSITION TO CLOSE ---")
            self.print_current_balance(bar)
            print("{} | net performance (%) = {}".format(date, round(perf, 2)))
            print("{} | number of trades executed = {}".format(date, self.trades))
            self.position = 0
            self.units = 0
        
        return perf
        
        
    def opt_strategy(self):
        ''' 
        Backtests an WILLR+MACD strategy.
        
        Parameters
        ----------
        stop_loss: int
            stop loss, it is actually in percetnage so 10 = 10%
        '''
        
        # nice printout
        #self.stop_loss = self.stop_loss/100
        #self.PCT_BUY = self.PCT_BUY / 100
        #self.PCT_SELL = self.PCT_SELL/100
        
        stm = "Testing MASR {} | SMA_Trend = {} EMA_Trend = {} PCT_BUY = {} PCT_SELL = {} stop_loss = {}%".format(self.symbol, self.SMA_Trend, self.EMA_Trend, self.PCT_BUY, self.PCT_SELL, self.stop_loss)
        print("-" * 75)
        print(stm)
        print("-" * 75)
        
        # reset 
        self.position = 0  # initial neutral position
        self.trades = 0  # no trades yet
        self.current_balance = self.initial_balance  # reset initial capital
        self.stop_loss_level = 0
        
        # prepare data
        #self.data_b["SMA_Trend"] = ta.SMA(self.data_b["Close"], timeperiod=self.SMA_Trend)
        #self.data_b = self.data_b.resample("4h").last()
        #self.data_b = self.data_b.ffill()
        #self.data["SMA_Trend"] =  self.data_b["SMA_Trend"]

        self.data.dropna(inplace = True)
        self.cond_buy_1 = 0
        self.cond_buy_2 = 0
        self.cond_sell_1 = 0

        # MASR Strategy
        for bar in range(len(self.data)-1): # all bars (except the last bar)
############################################################## Buy conditions ####################################################
            if self.cond_buy_1 >= 1:
                self.signal1 = (self.data["Close"].iloc[bar] > self.data["SMA_Trend"].iloc[bar] and self.data["Low"].iloc[bar] < (self.data["SMA_Trend"].iloc[bar]*(1+self.PCT_BUY))) * self.Keep_Signal_1
                if self.signal1 == 0:
                    self.cond_buy_1 -= 1
                elif self.signal1 >= 1:
                    self.cond_buy_1 = self.signal1

            if self.cond_buy_1 == 0:
                self.cond_buy_1 = (self.data["Close"].iloc[bar] > self.data["SMA_Trend"].iloc[bar] and self.data["Low"].iloc[bar] < (self.data["SMA_Trend"].iloc[bar]*(1+self.PCT_BUY))) * self.Keep_Signal_1
                
            if self.cond_buy_2 >= 1:
                self.signal2 = (self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar]) * self.Keep_Signal_2
                if self.signal2 == 0:
                    self.cond_buy_2 -= 1
                elif self.signal2 >= 1:
                    self.cond_buy_2 = self.signal2

            if self.cond_buy_2 == 0:
                self.cond_buy_2 = (self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar]) * self.Keep_Signal_2
############################################################## Buy conditions ####################################################            
            if self.stop:
                break

            if self.position == 0:
                if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                    if self.position == 0:
                        self.position = 1  # long position
                        self.opt_go_long(bar, amount = self.current_balance) # go long
                        self.entry_price = float(self.data.Open.iloc[bar+1])
                        if self.stop_loss_level == 0:
                            self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                        if self.data["Low"].iloc[bar+1] < self.stop_loss_level: # stop-loss is hit
                            self.opt_close_instrument(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral with full amount
                            self.position = 0 # neutral position
                            self.stop_loss_level = 0

                    elif self.position == 1:
                        if self.data["Low"].iloc[bar] < self.stop_loss_level: # stop-loss is hit
                            self.opt_close_instrument_two(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral with full amount
                            self.position = 0 # neutral position
                            self.stop_loss_level = 0
                            if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                                if self.position == 0:
                                    self.position = 1  # long position
                                    price = self.data.Close.iloc[bar]
                                    self.opt_go_long(bar, amount = self.current_balance) # go long
                                    self.entry_price = float(self.data.Open.iloc[bar+1])
                                    if self.stop_loss_level == 0:
                                        self.stop_loss_level = price - (price * self.stop_loss)

            elif self.position == 1:
                if self.data["Low"].iloc[bar] < self.stop_loss_level: # stop-loss is hit
                    self.opt_close_instrument_two(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral
                    self.position = 0 # neutral position
                    self.stop_loss_level = 0
                    if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                        if self.position == 0:
                            self.position = 1  # long position
                            price = self.data.Close.iloc[bar]
                            self.opt_go_long(bar, amount = self.current_balance) # go long
                            self.entry_price = float(self.data.Open.iloc[bar+1])
                            if self.stop_loss_level == 0:
                                self.stop_loss_level = price - (price * self.stop_loss)

                elif self.position == 1:
                    if self.data["Close"].iloc[bar] < self.data["SMA_Trend"].iloc[bar]: # signal to go neutral
                        if self.position == 1:
                            self.opt_sell_instrument(bar, "SMA Close", units = self.units) # go neutral with full amount
                            self.position = 0 # neutral position
                            self.stop_loss_level = 0
                            if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                                if self.position == 0:
                                    self.position = 1  # long position
                                    self.opt_go_long(bar, amount = self.current_balance) # go long
                                    self.entry_price = float(self.data.Open.iloc[bar+1])
                                    if self.stop_loss_level == 0:
                                        self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                                    if self.data["Low"].iloc[bar+1] < self.stop_loss_level: # stop-loss is hit
                                        self.opt_close_instrument(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral with full amount
                                        self.position = 0 # neutral position
                                        self.stop_loss_level = 0
                    
                    elif self.data["High"].iloc[bar] > (self.data["SMA_Trend"].iloc[bar]*(1+self.PCT_SELL)): # go neutral with full amount
                        if self.position == 1:
                            sell_price = float(self.data["SMA_Trend"].iloc[bar]*(1+self.PCT_SELL))
                            if self.entry_price > sell_price:
                                if self.data.High.iloc[bar] > (self.entry_price * 1.0):
                                    new_sell_price = self.entry_price * 1.0
                                    self.opt_sell_instrument_sma_profit(bar, "SMA Profit Close", new_sell_price, units = self.units) # go neutral with full amount
                                    self.position = 0 # neutral position
                                    self.stop_loss_level = 0

                                    if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                                        if self.position == 0:
                                            self.position = 1  # long position
                                            self.opt_go_long(bar, amount = self.current_balance) # go long
                                            if self.stop_loss_level == 0:
                                                self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                                            if self.data["Low"].iloc[bar+1] < self.stop_loss_level: # stop-loss is hit
                                                self.opt_close_instrument(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral with full amount
                                                self.position = 0 # neutral position
                                                self.stop_loss_level = 0
                                        
                            else:
                                self.opt_sell_instrument_sma_profit(bar, "SMA Profit Close", sell_price, units = self.units) # go neutral with full amount
                                self.position = 0 # neutral position
                                self.stop_loss_level = 0

                                if self.cond_buy_1 and self.cond_buy_2: ### signal to go long ###
                                    if self.position == 0:
                                        self.position = 1  # long position
                                        self.opt_go_long(bar, amount = self.current_balance) # go long
                                        if self.stop_loss_level == 0:
                                            self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                                        if self.data["Low"].iloc[bar+1] < self.stop_loss_level: # stop-loss is hit
                                            self.opt_close_instrument(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral with full amount
                                            self.position = 0 # neutral position
                                            self.stop_loss_level = 0

        if self.position != 0:
            self.close_pos(bar + 1)  # close position at the last bar
            perf = (self.current_balance - self.initial_balance) / self.initial_balance * 100
        else:
            date, Close, Open = self.get_values(bar + 1)
            perf = (self.current_balance - self.initial_balance) / self.initial_balance * 100
            self.trades += 1
            print(75 * "-")
            print("--- NO POSITION TO CLOSE ---")
            self.print_current_balance(bar)
            print("{} | net performance (%) = {}".format(date, round(perf, 2)))
            print("{} | number of trades executed = {}".format(date, self.trades))
            self.position = 0
            self.units = 0
        
        return perf
    
    def optimize_parameters(self, SMA_Trend, EMA_Trend):
        ''' Finds the optimal strategy (global maximum) given the WILLR Parameters.

        Parameters
        ----------
        SMA, WILLR_Upper, WILLR_Lower: tuple
        Periods_extra" list
            tuples of the form (start, end, step size)
        '''

        combinations = list(product(range(*SMA_Trend), range(*EMA_Trend)))

        # test all combinations
        results = []
        drawdown = []
        for comb in combinations:
            self.set_parameters(comb[0], comb[1])
            self.get_data_indicators_a()
            results.append(self.opt_strategy())
            drawdown.append(self.get_drawdown()[0])
            self.buy_price = []
            self.sell_price = []
            self.buy_amount = []
            self.date_buy = []
            self.date_sell = []
            self.balance = [self.amount]
            self.get_data()
            self.stop = False


        # create a df with many results
        many_results = pd.DataFrame(data=combinations, columns=["SMA_Trend", "EMA_Trend"])
        many_results["performance"] = results
        many_results["drawdown"] = drawdown
        self.results_overview = many_results

        best_perf = np.max(results)  # best performance
        opt = combinations[np.argmax(results)]  # optimal parameters

        # run/set the optimal strategy
        self.set_parameters(int(opt[0]), int(opt[1]))
        self.get_data_indicators_a()
        self.test_strategy()

        return opt, best_perf

    def optimize_drawdown(self, max_drawdown):
        ''' Finds the optimal strategy (global maximum) that has a drawdown less than the inputed drawdown.

        Parameters
        ----------
        Drawdown: float (ex. 0.5)
        '''

        df_results = self.results_overview.copy()
        drawdown_optimized = df_results[df_results.drawdown <= max_drawdown].reset_index()
        self.results_overview = drawdown_optimized

        # I just need to filter it by making another dataframe that only keeps the results where the drawdown is smaller than the one passed
        if drawdown_optimized.performance.count() > 0:
            self.best_perf = drawdown_optimized["performance"].max()
            best_perf = np.max(drawdown_optimized.performance)  # best performance
            opt1, opt2 = drawdown_optimized.iloc[np.argmax(drawdown_optimized.performance), 1:3]
            opt = [opt1, opt2]

            # run/set the optimal strategy
            self.set_parameters(int(opt[0]), int(opt[1]))

            self.buy_price = []
            self.sell_price = []
            self.buy_amount = []
            self.date_buy = []
            self.date_sell = []
            self.balance = [self.amount]
            self.side = []
            self.get_data()
            self.get_data_indicators_a()
            self.stop = False

            self.test_strategy()
        else:
            print("No strategy under required drawdown could be found")

        return opt, self.best_perf