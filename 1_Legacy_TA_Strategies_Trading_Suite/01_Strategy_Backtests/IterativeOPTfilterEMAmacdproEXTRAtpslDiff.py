
from IterativeBase import *
import talib as ta
from itertools import product

class IterativeBacktest(IterativeBase):
    ''' Class for iterative (event-driven) backtesting of trading strategies.
    '''
    def __init__(self, symbol, start, end, amount, EMA_S, EMA_L, signal_mw, EMA_Trend, EMA_Trend_Two, stop_loss, take_profit, leverage, fee):
        super().__init__(symbol, start, end, amount, fee)
        self.EMA_S = EMA_S
        self.EMA_L = EMA_L
        self.signal_mw = signal_mw
        self.EMA_Trend = EMA_Trend
        self.EMA_Trend_Two = EMA_Trend_Two
        self.stop_loss = stop_loss
        self.take_profit = take_profit
        self.leverage = leverage
        self.get_data_emamacd_a()
        self.get_data_emamacd_b()
        self.amount = amount

    # helper method
    def go_long(self, bar, units = None, amount = None):
        if self.position == -1:
            self.buy_instrument(bar, units = -self.units) # if short position, go neutral first
        if units:
            self.buy_instrument(bar, units = units)
        elif amount:
            if amount == "100%":
                amount = self.current_balance
            elif amount == "150%":
                amount = self.current_balance * 1.5
            elif amount == "200%":
                amount = self.current_balance * 2
            elif amount == "250%":
                amount = self.current_balance * 2.5
            elif amount == "300%":
                amount = self.current_balance * 3
            self.buy_instrument(bar, amount = amount) # go long

    def opt_go_long(self, bar, units = None, amount = None):
        if self.position == -1:
            self.opt_buy_instrument(bar, units = -self.units) # if short position, go neutral first
        if units:
            self.opt_buy_instrument(bar, units = units)
        elif amount:
            if amount == "100%":
                amount = self.current_balance
            elif amount == "150%":
                amount = self.current_balance * 1.5
            elif amount == "200%":
                amount = self.current_balance * 2
            elif amount == "250%":
                amount = self.current_balance * 2.5
            elif amount == "300%":
                amount = self.current_balance * 3
            self.opt_buy_instrument(bar, amount = amount) # go long
            
    # helper method for when selling with technical indicators
    def sell_all(self, bar, units = None, amount = None):
        if self.position == 1:
            self.sell_instrument(bar, units = self.units) # if long position, go neutral first
        
        
    def opt_sell_all(self, bar, units = None, amount = None):
        if self.position == 1:
            self.opt_sell_instrument(bar, units = self.units) # if long position, go neutral first
            
    # helper method for when using stop loss        
    def opt_close_all(self, bar, stop_price, units = None, amount = None):
        self.opt_close_instrument(bar, stop_price, units = self.units) # if long position, go neutral first

    def opt_close_all_two(self, bar, stop_price, units = None, amount = None):
        self.opt_close_instrument_two(bar, stop_price, units = self.units) # if long position, go neutral first
        
    def get_data_emamacd_a(self):
        raw = self.data
        raw["MACD"], raw["MACD_Signal"], raw["macdhist"] = ta.MACD(raw["Close"], fastperiod=self.EMA_S, slowperiod=self.EMA_L, signalperiod=self.signal_mw)
        raw["EMA_Trend"] = ta.EMA(raw["Close"], timeperiod=self.EMA_Trend)
        self.data = raw
        
    def get_data_emamacd_b(self):
        raw = self.data_b
        raw["EMA_Trend_Two"] = ta.EMA(raw["Close"], timeperiod=self.EMA_Trend_Two)
        raw = raw.resample("4h").last()
        raw = raw.ffill()
        self.data_b = raw
        
    def set_parameters(self, stop_loss = None, take_profit = None):
        ''' Updates EMA parameters and resp. time series.
        '''
        if stop_loss is not None:
            self.stop_loss = stop_loss
        
        if take_profit is not None:
            self.take_profit = take_profit
               
    def test_emamacd_strategy(self):
        ''' 
        Backtests an EMA+MACD strategy.
        
        Parameters
        ----------
        stop_loss: int
            stop loss, it is actually in percetnage so 10 = 10%
        '''
        
        # nice printout
        self.stop_loss = self.stop_loss/100
        self.take_profit = self.take_profit/100
        
        
        stm = "Testing MACD + EMA strategy | {} | EMA_S = {} & EMA_L = {}, signal_mw = {}, EMA_Trend = {}, stop_loss = {}%, take_profit = {}%, EMA_Trend_Two = {}".format(self.symbol, self.EMA_S, self.EMA_L, self.signal_mw, self.EMA_Trend, self.stop_loss, self.take_profit, self.EMA_Trend_Two)
        print("-" * 75)
        print(stm)
        print("-" * 75)
        
        # reset 
        self.position = 0  # initial neutral position
        self.trades = 0  # no trades yet
        self.current_balance = self.initial_balance  # reset initial capital
        self.stop_loss_level = 0
        self.take_profit_level = 0
        self.data["EMA_Trend_Two"] = self.data_b["EMA_Trend_Two"]
        
        self.data.dropna(inplace = True)

        # macd + ema crossover strategy
        for bar in range(len(self.data)-1): # all bars (except the last bar)
            
            if self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar] and self.data["Close"].iloc[bar] > self.data["EMA_Trend_Two"].iloc[bar] and self.data["macdhist"].iloc[bar] > 0 and self.data["macdhist"].iloc[bar-1] < 0: ### signal to go long ###
                if self.position == 0:
                    self.go_long(bar, amount = self.leverage) # go long
                    self.position = 1  # long position
                    if self.stop_loss_level == 0:
                        self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                        self.take_profit_level = self.data.Close.iloc[bar] + (self.data.Close.iloc[bar] * self.take_profit)
                        
                        if self.data["Low"].iloc[bar+1] < self.stop_loss_level: # stop-loss is hit
                            self.close_instrument(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral
                            self.position = 0 # neutral position
                            self.stop_loss_level = 0
                            self.take_profit_level = 0
                        
                        elif self.data["High"].iloc[bar+1] > self.take_profit_level: # stop-loss is hit
                            self.close_instrument(bar, stop_price = self.take_profit_level, units = self.units, profit = True) # go neutral
                            self.position = 0 # neutral position
                            self.stop_loss_level = 0
                            self.take_profit_level = 0
                            
                            
                elif self.position == 1:
                    if self.data["Low"].iloc[bar] < self.stop_loss_level: # stop-loss is hit
                        self.close_instrument_two(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral
                        self.position = 0 # neutral position
                        self.stop_loss_level = 0
                        self.take_profit_level = 0
                        
                        if self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar] and self.data["Close"].iloc[bar] > self.data["EMA_Trend_Two"].iloc[bar] and self.data["macdhist"].iloc[bar] > 0 and self.data["macdhist"].iloc[bar-1] < 0: ###signal to go long ###
                            if self.position == 0:
                                self.go_long(bar, amount = self.leverage) # go long
                                self.position = 1  # long position
                            if self.stop_loss_level == 0:
                                self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                            if self.take_profit_level == 0:
                                self.take_profit_level = self.data.Close.iloc[bar] + (self.data.Close.iloc[bar] * self.take_profit)
                    
                    elif self.data["High"].iloc[bar] > self.take_profit_level: # stop-loss is hit
                        self.close_instrument_two(bar, stop_price = self.take_profit_level, units = self.units, profit = True) #goutral
                        self.position = 0 # neutral position
                        self.stop_loss_level = 0
                        self.take_profit_level = 0
                        
                        if self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar] and self.data["Close"].iloc[bar] > self.data["EMA_Trend_Two"].iloc[bar] and self.data["macdhist"].iloc[bar] > 0 and self.data["macdhist"].iloc[bar-1] < 0: # signal to go long
                            if self.position == 0:
                                self.go_long(bar, amount = self.leverage) # go long
                                self.position = 1  # long position
                            if self.stop_loss_level == 0:
                                self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                            if self.take_profit_level == 0:
                                self.take_profit_level = self.data.Close.iloc[bar] + (self.data.Close.iloc[bar] * self.take_profit)
                        
                        
            elif self.position == 1:
                if self.data["Low"].iloc[bar] < self.stop_loss_level: # stop-loss is hit
                    self.close_instrument_two(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral
                    self.position = 0 # neutral position
                    self.stop_loss_level = 0
                    self.take_profit_level = 0
                    
                    if self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar] and self.data["Close"].iloc[bar] > self.data["EMA_Trend_Two"].iloc[bar] and self.data["macdhist"].iloc[bar] > 0 and self.data["macdhist"].iloc[bar-1] < 0: # signal to go long
                        if self.position == 0:
                            self.go_long(bar, amount = self.leverage) # go long
                            self.position = 1  # long position
                        if self.stop_loss_level == 0:
                            self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                        if self.take_profit_level == 0:
                            self.take_profit_level = self.data.Close.iloc[bar] + (self.data.Close.iloc[bar] * self.take_profit)
                
                elif self.data["High"].iloc[bar] > self.take_profit_level: # stop-loss is hit
                    self.close_instrument_two(bar, stop_price = self.take_profit_level, units = self.units, profit = True) # go neutral
                    self.position = 0 # neutral position
                    self.stop_loss_level = 0
                    self.take_profit_level = 0
                            
                    if self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar] and self.data["Close"].iloc[bar] > self.data["EMA_Trend_Two"].iloc[bar] and self.data["macdhist"].iloc[bar] > 0 and self.data["macdhist"].iloc[bar-1] < 0: # signal to go long
                        if self.position == 0:
                            self.go_long(bar, amount = self.leverage) # go long
                            self.position = 1  # long position
                        if self.stop_loss_level == 0:
                            self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                        if self.take_profit_level == 0:
                            self.take_profit_level = self.data.Close.iloc[bar] + (self.data.Close.iloc[bar] * self.take_profit)
    
                elif self.data["macdhist"].iloc[bar] < 0 and self.data["macdhist"].iloc[bar-1] > 0: # signal to go neutral
                #elif self.data["MACD"].iloc[bar] < self.data["MACD_Signal"].iloc[bar]: # signal to go neutral
                    if self.position == 1:
                        self.sell_all(bar, amount = self.leverage) # go neutral with full amount
                        self.position = 0 # neutral position
                        self.stop_loss_level = 0
                        self.take_profit_level = 0
                
        if self.position != 0:
            self.close_pos(bar+1) # close position at the last bar
            perf = (self.current_balance - self.initial_balance) / self.initial_balance * 100
        else:
            date, Close, Open = self.get_values(bar+1)
            perf = (self.current_balance - self.initial_balance) / self.initial_balance * 100
            self.trades += 1
            print(75 * "-")
            print("--- NO POSITION TO CLOSE ---")
            self.print_current_balance(bar)
            print("{} | net performance (%) = {}".format(date, round(perf, 2) ))
            print("{} | number of trades executed = {}".format(date, self.trades))
            self.position = 0
            self.units = 0
        
        return perf
        
        
    def opt_emamacd_strategy(self):
        ''' 
        Backtests an EMA+MACD strategy.
        
        Parameters
        ----------
        stop_loss: int
            stop loss, it is actually in percetnage so 10 = 10%
        '''
        
        # nice printout
        self.stop_loss = self.stop_loss/100
        self.take_profit = self.take_profit/100
        self.data["EMA_Trend_Two"] = self.data_b["EMA_Trend_Two"]
        
        stm = "Testing MACD + EMA strategy | {} | EMA_S = {} & EMA_L = {}, signal_mw = {}, EMA_Trend = {}, stop_loss = {}%, take_profit = {}%, EMA_Trend_Two = {}".format(self.symbol, self.EMA_S, self.EMA_L, self.signal_mw, self.EMA_Trend, self.stop_loss, self.take_profit, self.EMA_Trend_Two)
        print("-" * 75)
        print(stm)
        print("-" * 75)
        
        # reset 
        self.position = 0  # initial neutral position
        self.trades = 0  # no trades yet
        self.current_balance = self.initial_balance  # reset initial capital
        self.stop_loss_level = 0
        self.take_profit_level = 0
        
        self.data.dropna(inplace = True)

        # macd + ema crossover strategy
        for bar in range(len(self.data)-1): # all bars (except the last bar)
            
            if self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar] and self.data["Close"].iloc[bar] > self.data["EMA_Trend_Two"].iloc[bar] and self.data["macdhist"].iloc[bar] > 0 and self.data["macdhist"].iloc[bar-1] < 0: ### signal to go long ###
                if self.position == 0:
                    self.opt_go_long(bar, amount = self.leverage) # go long
                    self.position = 1  # long position
                    if self.stop_loss_level == 0:
                        self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                        self.take_profit_level = self.data.Close.iloc[bar] + (self.data.Close.iloc[bar] * self.take_profit)
                        
                        if self.data["Low"].iloc[bar+1] < self.stop_loss_level: # stop-loss is hit
                            self.opt_close_instrument(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral
                            self.position = 0 # neutral position
                            self.stop_loss_level = 0
                            self.take_profit_level = 0
                        
                        elif self.data["High"].iloc[bar+1] > self.take_profit_level: # stop-loss is hit
                            self.opt_close_instrument(bar, stop_price = self.take_profit_level, units = self.units) # go neutral
                            self.position = 0 # neutral position
                            self.stop_loss_level = 0
                            self.take_profit_level = 0
                            
                            
                elif self.position == 1:
                    if self.data["Low"].iloc[bar] < self.stop_loss_level: # stop-loss is hit
                        self.opt_close_instrument_two(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral
                        self.position = 0 # neutral position
                        self.stop_loss_level = 0
                        self.take_profit_level = 0
                        
                        if self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar] and self.data["Close"].iloc[bar] > self.data["EMA_Trend_Two"].iloc[bar] and self.data["macdhist"].iloc[bar] > 0 and self.data["macdhist"].iloc[bar-1] < 0: ###signal to go long ###
                            if self.position == 0:
                                self.opt_go_long(bar, amount = self.leverage) # go long
                                self.position = 1  # long position
                            if self.stop_loss_level == 0:
                                self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                            if self.take_profit_level == 0:
                                self.take_profit_level = self.data.Close.iloc[bar] + (self.data.Close.iloc[bar] * self.take_profit)
                    
                    elif self.data["High"].iloc[bar] > self.take_profit_level: # stop-loss is hit
                        self.opt_close_instrument_two(bar, stop_price = self.take_profit_level, units = self.units) #goutral
                        self.position = 0 # neutral position
                        self.stop_loss_level = 0
                        self.take_profit_level = 0
                        
                        if self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar] and self.data["Close"].iloc[bar] > self.data["EMA_Trend_Two"].iloc[bar] and self.data["macdhist"].iloc[bar] > 0 and self.data["macdhist"].iloc[bar-1] < 0: # signal to go long
                            if self.position == 0:
                                self.opt_go_long(bar, amount = self.leverage) # go long
                                self.position = 1  # long position
                            if self.stop_loss_level == 0:
                                self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                            if self.take_profit_level == 0:
                                self.take_profit_level = self.data.Close.iloc[bar] + (self.data.Close.iloc[bar] * self.take_profit)
                        
                        
            elif self.position == 1:
                if self.data["Low"].iloc[bar] < self.stop_loss_level: # stop-loss is hit
                    self.opt_close_instrument_two(bar, stop_price = self.stop_loss_level, units = self.units) # go neutral
                    self.position = 0 # neutral position
                    self.stop_loss_level = 0
                    self.take_profit_level = 0
                    
                    if self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar] and self.data["Close"].iloc[bar] > self.data["EMA_Trend_Two"].iloc[bar] and self.data["macdhist"].iloc[bar] > 0 and self.data["macdhist"].iloc[bar-1] < 0: # signal to go long
                        if self.position == 0:
                            self.opt_go_long(bar, amount = self.leverage) # go long
                            self.position = 1  # long position
                        if self.stop_loss_level == 0:
                            self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                        if self.take_profit_level == 0:
                            self.take_profit_level = self.data.Close.iloc[bar] + (self.data.Close.iloc[bar] * self.take_profit)
                
                elif self.data["High"].iloc[bar] > self.take_profit_level: # stop-loss is hit
                    self.opt_close_instrument_two(bar, stop_price = self.take_profit_level, units = self.units) # go neutral
                    self.position = 0 # neutral position
                    self.stop_loss_level = 0
                    self.take_profit_level = 0
                            
                    if self.data["Close"].iloc[bar] > self.data["EMA_Trend"].iloc[bar] and self.data["Close"].iloc[bar] > self.data["EMA_Trend_Two"].iloc[bar] and self.data["macdhist"].iloc[bar] > 0 and self.data["macdhist"].iloc[bar-1] < 0: # signal to go long
                        if self.position == 0:
                            self.opt_go_long(bar, amount = self.leverage) # go long
                            self.position = 1  # long position
                        if self.stop_loss_level == 0:
                            self.stop_loss_level = self.data.Close.iloc[bar] - (self.data.Close.iloc[bar] * self.stop_loss)
                        if self.take_profit_level == 0:
                            self.take_profit_level = self.data.Close.iloc[bar] + (self.data.Close.iloc[bar] * self.take_profit)
    
                elif self.data["macdhist"].iloc[bar] < 0 and self.data["macdhist"].iloc[bar-1] > 0: # signal to go neutral
                    if self.position == 1:
                        self.opt_sell_all(bar, amount = self.leverage) # go neutral with full amount
                        self.position = 0 # neutral position
                        self.stop_loss_level = 0
                        self.take_profit_level = 0
                
        if self.position != 0:
            self.close_pos(bar+1) # close position at the last bar
            perf = (self.current_balance - self.initial_balance) / self.initial_balance * 100
        else:
            date, Close, Open = self.get_values(bar+1)
            perf = (self.current_balance - self.initial_balance) / self.initial_balance * 100
            self.trades += 1
            print(75 * "-")
            print("--- NO POSITION TO CLOSE ---")
            self.print_current_balance(bar)
            print("{} | net performance (%) = {}".format(date, round(perf, 2) ))
            print("{} | number of trades executed = {}".format(date, self.trades))
            self.position = 0
            self.units = 0
        
        return perf
    
    def optimize_parameters(self, stop_loss_range, take_profit_range):
        ''' Finds the optimal strategy (global maximum) given the EMA Trend and stop loss parameter ranges.

        Parameters
        ----------
        stop_loss_range, take_profit_range: tuple
            tuples of the form (start, end, step size)
        '''
        
        combinations = list(product(range(*stop_loss_range), range(*take_profit_range)))
        
        # test all combinations
        results = []
        drawdown = []
        for comb in combinations:
            self.set_parameters(comb[0], comb[1])
            results.append(self.opt_emamacd_strategy())
            drawdown.append(self.get_drawdown()[0])
            
            self.buy_price = []
            self.sell_price = []
            self.buy_amount = []
            self.date_buy = []
            self.date_sell = []
            self.balance = []
            self.get_data()
            self.get_data_b()
            self.get_data_emamacd_a()
            self.get_data_emamacd_b()
        
        # create a df with many results
        many_results =  pd.DataFrame(data = combinations, columns = ["stop_loss_range", "take_profit_range"])
        many_results["performance"] = results
        many_results["drawdown"] = drawdown
        self.results_overview = many_results
        
        best_perf = np.max(results) # best performance
        opt = combinations[np.argmax(results)] # optimal parameters
        
        # run/set the optimal strategy
        self.set_parameters(int(opt[0]), int(opt[1]))
        
        self.buy_price = []
        self.sell_price = []
        self.buy_amount = []
        self.date_buy = []
        self.date_sell = []
        self.balance = [self.amount]
        self.get_data()
        self.get_data_b()
        self.get_data_emamacd_a()
        self.get_data_emamacd_b()
            
        self.test_emamacd_strategy()
            
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
        
        # I just need to filter it by making another dataframe that only keeps the results where the drawdown is emaller than the one passed
        if drawdown_optimized.performance.count() > 0:
            self.best_perf = drawdown_optimized["performance"].max()
            best_perf = np.max(drawdown_optimized.performance) # best performance
            opt1, opt2, = drawdown_optimized.iloc[np.argmax(drawdown_optimized.performance), 1:3]
            opt = [opt1, opt2]
        
        # run/set the optimal strategy
            self.set_parameters(opt[0], opt[1])
            
            self.buy_price = []
            self.sell_price = []
            self.buy_amount = []
            self.date_buy = []
            self.date_sell = []
            self.balance = [self.amount]
            self.get_data()
            self.get_data_b()
            self.get_data_emamacd_a()
            self.get_data_emamacd_b()
            
            self.test_emamacd_strategy()
        else:
            print("No strategy under required drawdown could be found")
                                               
        return opt, self.best_perf