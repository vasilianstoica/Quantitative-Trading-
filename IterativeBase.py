from pathlib import Path
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
plt.style.use("seaborn")


class IterativeBase():
    ''' Base class for iterative (event-driven) backtesting of trading strategies.
    '''

    def __init__(self, symbol, start, end, amount, fee):
        '''
        Parameters
        ----------
        symbol: str
            ticker symbol (instrument) to be backtested
        start: str
            start date for data import
        end: str
            end date for data import
        amount: float
            initial amount to be invested per trade
        fee: boolean (default = True) 
            the fee as a percentage, default is 0.1% like on Binance
        '''
        self.symbol = symbol
        self.start = start
        self.end = end
        self.initial_balance = amount
        self.current_balance = amount
        self.units = 0
        self.trades = 0
        self.position = 0
        self.fee = fee
        self.stop_loss_level = None
        self.get_data()
        self.get_data_b()
        self.stop_loss_level = 0
        self.buy_amount = []
        self.buy_price = []
        self.sell_price = []
        self.date_buy = []
        self.date_sell = []
        self.balance = [amount]

    
    def get_data(self):
        ''' Imports the data from detailed.csv (source can be changed).
        '''
        path_1 = Path(r'C:\Users\stoic\Desktop\money\coding\2. Crypto\Backtesting\zzzzz Remaking Data')
        raw = pd.read_csv(path_1.joinpath("BTC_4H_OHLC_UTC.csv"), parse_dates = ["Timestamp"], index_col = "Timestamp").dropna()
        raw = raw.loc[self.start:self.end].copy()
        raw["returns"] = np.log(raw.Close / raw.Close.shift(1))
        self.data = raw
        
    def get_data_b(self):
        ''' Imports the data from detailed.csv (source can be changed).
        '''
        path_1 = Path(r'C:\Users\stoic\Desktop\money\coding\2. Crypto\Backtesting\zzzzz Remaking Data')
        raw = pd.read_csv(path_1.joinpath("BTC_12H_OHLC_UTC.csv"), parse_dates = ["Timestamp"], index_col = "Timestamp").dropna()
        raw = raw.loc[self.start:self.end].copy()
        raw["returns"] = np.log(raw.Close / raw.Close.shift(1))
        self.data_b = raw 
    
    def get_values(self, bar):
        ''' Returns the date, and the price for the given bar.
        '''
        date = str(self.data.index[bar])
        Close = round(self.data.Close.iloc[bar], 5)
        Open = round(self.data.Open.iloc[bar], 5)
        return date, Close, Open
    
    def print_current_balance(self, bar):
        ''' Prints out the current (cash) balance.
        '''
        date, Close, Open = self.get_values(bar)
        print("{} | Current Balance: {}".format(date, round(self.current_balance, 2)))
        
    def buy_instrument(self, bar, units = None, amount = None):
        ''' Places and executes a buy order (market order).
        '''
        
        date, Close, Open = self.get_values(bar+1)
        self.date_buy.append(date)
        self.buy_price.append(round(Open, 5))

        if self.fee != 0:
            cost = amount * self.fee # ask price
        amount -= cost
        self.current_balance -= cost
        
        if amount is not None: # use units if units are passed, otherwise calculate units
            units = amount / Open
        
        self.buy_amount.append(units)
        self.current_balance -= units * Open # reduce cash balance by "purchase price"
        self.units += units
        self.trades += 1
                
        print("{} |  Buying {} for {}".format(date, units, round(Open, 5)))
        
        
    def opt_buy_instrument(self, bar, units = None, amount = None):
        ''' Places and executes a buy order (market order).
        '''
        
        date, Close, Open = self.get_values(bar+1)
        self.date_buy.append(date)
        self.buy_price.append(round(Open, 5))

        if self.fee != 0:
            cost = amount * self.fee # ask price
        amount -= cost
        self.current_balance -= cost
        
        if amount is not None: # use units if units are passed, otherwise calculate units
            units = amount / Open
        
        self.buy_amount.append(units)
        self.current_balance -= units * Open # reduce cash balance by "purchase price"
        self.units += units
        self.trades += 1
                    
    def sell_instrument(self, bar, units = None, amount = None):
        ''' Places and executes a sell order (market order).
        '''
        date, Close, Open = self.get_values(bar+1)
        self.date_sell.append(date)
        self.sell_price.append(round(Open, 5))
        
        if self.fee != 0:
            cost = (units * Open) * self.fee # fees involved
        self.current_balance -= cost
        
        if amount is not None: # use units if units are passed, otherwise calculate units
            units = amount / Open
        self.current_balance += units * Open # increases cash balance by "purchase price"
        
        self.balance.append(self.current_balance)
        
        self.units -= units
        self.trades += 1
        print("{} |  Selling {} for {}. Trigger TA".format(date, units, round(Open, 5)))
        
    def close_instrument(self, bar, stop_price, units = None, amount = None, profit = False):
        ''' Places and executes a sell order (market order).
        '''
        date, Close, Open = self.get_values(bar+1)
        self.date_sell.append(date)
        self.sell_price.append(round(stop_price, 5))
        
        if self.fee != 0:
            cost = (units * stop_price) * self.fee # fees involved
        self.current_balance -= cost
        
        if amount is not None: # use units if units are passed, otherwise calculate units
            units = amount / stop_price
        self.current_balance += units * stop_price # increases cash balance by "purchase price"
        
        self.balance.append(self.current_balance)
        
        self.units -= units
        self.trades += 1
        
        if profit == True:
            print("{} |  Selling {} for {}. Trigger take-profit".format(date, units, round(stop_price, 5)))
        else:    
            print("{} |  Selling {} for {}. Trigger stop-loss".format(date, units, round(stop_price, 5)))
        
    def close_instrument_two(self, bar, stop_price, units = None, amount = None, profit = False):
        ''' Places and executes a sell order (market order).
        '''
        date, Close, Open = self.get_values(bar)
        self.date_sell.append(date)
        self.sell_price.append(round(stop_price, 5))
        
        if self.fee != 0:
            cost = (units * stop_price) * self.fee # fees involved
        self.current_balance -= cost
        
        if amount is not None: # use units if units are passed, otherwise calculate units
            units = amount / stop_price
        self.current_balance += units * stop_price # increases cash balance by "purchase price"
        
        self.balance.append(self.current_balance)
        
        self.units -= units
        self.trades += 1
        
        if profit == True:
            print("{} |  Selling {} for {}. Trigger take-profit".format(date, units, round(stop_price, 5)))
        else:    
            print("{} |  Selling {} for {}. Trigger stop-loss".format(date, units, round(stop_price, 5)))
        
        
    def opt_sell_instrument(self, bar, units = None, amount = None):
        ''' Places and executes a sell order (market order).
        '''
        date, Close, Open = self.get_values(bar+1)
        self.date_sell.append(date)
        self.sell_price.append(round(Open, 5))
        
        if self.fee != 0:
            cost = (units * Open) * self.fee # fees involved
        self.current_balance -= cost
        
        if amount is not None: # use units if units are passed, otherwise calculate units
            units = amount / Open
        self.current_balance += units * Open # increases cash balance by "purchase price"
        
        self.balance.append(self.current_balance)
        
        self.units -= units
        self.trades += 1
        
    def opt_close_instrument(self, bar, stop_price, units = None, amount = None):
        ''' Places and executes a sell order (market order).
        '''
        date, Close, Open = self.get_values(bar+1)
        self.date_sell.append(date)
        self.sell_price.append(round(stop_price, 5))
        
        if self.fee != 0:
            cost = (units * stop_price) * self.fee # fees involved
        self.current_balance -= cost
        
        if amount is not None: # use units if units are passed, otherwise calculate units
            units = amount / stop_price
        self.current_balance += units * stop_price # increases cash balance by "purchase price"
        
        self.balance.append(self.current_balance)
        
        self.units -= units
        self.trades += 1
        
    def opt_close_instrument_two(self, bar, stop_price, units = None, amount = None):
        ''' Places and executes a sell order (market order).
        '''
        date, Close, Open = self.get_values(bar)
        self.date_sell.append(date)
        self.sell_price.append(round(stop_price, 5))
        
        if self.fee != 0:
            cost = (units * stop_price) * self.fee # fees involved
        self.current_balance -= cost
        
        if amount is not None: # use units if units are passed, otherwise calculate units
            units = amount / stop_price
        self.current_balance += units * stop_price # increases cash balance by "purchase price"
        
        self.balance.append(self.current_balance)
        
        self.units -= units
        self.trades += 1
    
    def print_current_position_value(self, bar):
        ''' Prints out the current position value.
        '''
        date, Close, Open = self.get_values(bar)
        cpv = self.units * Close
        print("{} |  Current Position Value = {}".format(date, round(cpv, 2)))
    
    def print_current_nav(self, bar):
        ''' Prints out the current net asset value (nav).
        '''
        date, Close, Open = self.get_values(bar)
        nav = self.current_balance + self.units * Close
        print("{} |  Net Asset Value = {}".format(date, round(nav, 2)))
        
    def close_pos(self, bar):
        ''' Closes out a long or short position (go neutral).
        '''
        date, Close, Open = self.get_values(bar)
        print(75 * "-")
        print("{} | +++ CLOSING FINAL POSITION +++".format(date))
        
        self.date_sell.append(date)
        self.sell_price.append(round(Close, 5))
        
        self.current_balance += self.units * Close # closing final position (works with short and long!)
        self.current_balance -= (abs(self.units) * self.fee ) # substract the fees
        self.balance.append(self.current_balance)
        
        print("{} | closing position of {} for {}".format(date, self.units, Close))
        self.units = 0 # setting position to neutral
        self.trades += 1
        perf = (self.current_balance - self.initial_balance) / self.initial_balance * 100
        self.print_current_balance(bar)
        print("{} | net performance (%) = {}".format(date, round(perf, 2) ))
        print("{} | number of trades executed = {}".format(date, self.trades))
        print(75 * "-")
        
    def annual_returns(self):
        ''' Gives annual returns.
        '''
        
        data = self.data.copy()
        data_daily = data.resample("1D").sum().dropna()
        time = data.index[-1] - data.index[0] 
        perf = (self.current_balance)/self.initial_balance
        n = time.days/365.25
        annual_retun = (perf)**(1/n) -1
        AP = ((self.initial_balance + self.current_balance)/self.initial_balance)
        
        return annual_retun
    
    def get_trades(self):
        balance = self.balance
            
        df = pd.DataFrame(balance, columns = ["Balance"])
        while df.Balance.count() > len(self.buy_price):
            self.buy_price.append(np.nan)
            self.buy_amount.append(np.nan)
            self.date_buy.append(np.nan)
            self.sell_price.append(np.nan)
            self.date_sell.append(np.nan)
        
        #while df.Balance.count() > len(self.sell_price):

        
        df["Buy_price"] = self.buy_price
        df["Amount"] = self.buy_amount
        df["Sell_price"] =  self.sell_price
        df["Profit_$"] = (df["Sell_price"] - df["Buy_price"]) * df["Amount"]
        df["Date_buy"] = self.date_buy
        df["Date_sell"] = self.date_sell
        
        self.results = df

        return self.results
    
    
    def get_drawdown(self):
        ''' Returns the drawdown.
        '''
        self.get_trades()
        data = self.results.copy()
        data["cummax"] = data.Balance.cummax()
        drawdown_col = data["cummax"] - data["Balance"]
        drawdown_pct = drawdown_col / data["cummax"]  # calculates the maximum drawdown
        if drawdown_pct.count() > 0:
            drawdown_max = drawdown_pct.max()
            drawdown_id = drawdown_pct.idxmax()  # gives the date for the maximum drawdown
            drawdown_date = self.results.copy().iloc[drawdown_id, -1]
            if type(drawdown_date) == type(1.0):
                drawdown_date = self.results.copy().dropna().Date_sell.iloc[-1]

        else:
            drawdown_max = 0
            drawdown_date = self.data.index[0]

        return drawdown_max, drawdown_date
    
    def get_time_no_profit(self):
        ''' Returns the max amount of days without profit.
        '''
        df = self.get_trades()
        df = df.set_index(pd.to_datetime(df.Date_sell)).drop("Date_sell", axis = 1)
        all_time_numbers = 0
        all_time_highs = []
        for num in df.Balance:
            if num > all_time_numbers:
                all_time_numbers = num
                all_time_highs.append(all_time_numbers)
            else:
                pass
        
        time_list = []
        
        for num_high in all_time_highs:
            time_list.append(df[df["Balance"]==num_high].index.values[0])
        
        difference_lst = [t - s for s, t in zip(time_list, time_list[1:])]
        difference_lst.sort()
        days = difference_lst[-1].astype('timedelta64[D]')
        days = days / np.timedelta64(1, 'D')
        
        return days
    
    def sortino(self):
        """Calculates the sortino ratio"""
        data = self.results.copy().dropna()
        data["stategy_return"] = self.results["Profit_$"]/self.results["Balance"]
        data["negative_return"] = np.where(data["stategy_return"]<0,data["stategy_return"],0)
        negative_volume = data["negative_return"].std() * np.sqrt(365)
        sortino = (self.annual_returns())/negative_volume
        
        return sortino
    
    def sharpe(self):
        """Calculates the sortino ratio"""
        data = self.results.copy().dropna()
        data["stategy_return"] = self.results["Profit_$"]/self.results["Balance"]
        negative_volume = data["stategy_return"].std() * np.sqrt(365)
        sharpe = (self.annual_returns())/negative_volume
        
        return sharpe
    
    def plot_data(self):  
        ''' Plots the closing price for the symbol.
        '''
        returns = self.data["Close"]/self.data["Close"][0]
        returns.to_frame().plot(figsize = (12, 8), title = "Buy and Hold Regular", logy = False)
        returns.to_frame().plot(figsize = (12, 8), title = "Buy and Hold Logarithmic", logy = True)
    
    def plot_strategy(self):  
        ''' Plots the Strategy for the symbol.
        '''        
        plot_data = pd.DataFrame({"Timestamp": self.results['Date_sell'], "Strategy": self.results.Balance/self.initial_balance})
        plot_data['Timestamp'] = pd.to_datetime(plot_data['Timestamp'])
        plot_data = plot_data.set_index(pd.to_datetime(plot_data.Timestamp)).drop("Timestamp", axis = 1)
        plot_data.plot(figsize = (12, 8), title = "Strategy Returns Regular", logy = False)
        plot_data.plot(figsize = (12, 8), title = "Strategy Returns Logarithmic", logy = True)
        
    def plot_all(self):  
        ''' Plots the Strategy for the symbol.
        '''
        data_plot = self.data.copy()
        plot_data = pd.DataFrame({"Timestamp": self.results['Date_sell'], "Software Returns": self.results.Balance})
        plot_data['Timestamp'] = pd.to_datetime(plot_data['Timestamp'])
        plot_data = plot_data.set_index(pd.to_datetime(plot_data.Timestamp)).drop("Timestamp", axis = 1)
        data_plot["Buy and Hold"] = data_plot["Close"]/data_plot["Close"][0]*3000
        
        if pd.isnull(plot_data.index[0]):
            plot_data = plot_data.iloc[1:]
        plot_data = plot_data[~plot_data.index.duplicated(keep='first')]
        
        data_plot["Software Returns"] = plot_data["Software Returns"]
        data_plot = data_plot.ffill()
        data_plot = data_plot.fillna(1)
        data_plot[["Buy and Hold", "Software Returns"]].plot(title="Buy and Hold Vs. Software Returns", fontsize=32, figsize=(50, 35), logy = False, linewidth=5)
        plt.title("Buy and Hold Vs. Software Returns", fontsize=57)
        plt.legend(["Buy and Hold", "Software Returns"], fontsize=42)
        data_plot[["Buy and Hold", "Software Returns"]].plot(title="Software Returns Vs. Buy and Hold Logarithmic", figsize=(12, 8), logy = True) 
        