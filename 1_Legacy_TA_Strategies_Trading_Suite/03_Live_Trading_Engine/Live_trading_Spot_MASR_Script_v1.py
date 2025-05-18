import websocket, json, time
import talib as ta
import pandas as pd
import numpy as np
from datetime import datetime, timedelta
import requests
import math as ma
import traceback
from binance import Client

import threading

class Trader():
    '''Live trading module. Parameters include;
    timeframe_a = str (ex. "5m", "1h", "1d")
    timeframe_a_resample = str (ex. "5min", "1h", "1d")
    timeframe_b = str (ex. "5m", "1h", "1d")
    timeframe_b_resample = str (ex. "5min", "1h", "1d")
    symbol = str (ex btcusdt)
    EMA_S = integer
    EMA_L = integer
    signal_mw = integer
    WMA_Trend = integer
    stop_loss = float
    api_key = string
    api_secret = string
    testnet =  bool
    '''
    
    def __init__(self, timeframe_a, timeframe_a_resample, symbol_Binance, SMA_Trend, EMA_Trend, PCT_BUY, PCT_SELL, stop_loss, take_profit, leverage, api_key_Mihai_Binance, api_secret_Mihai_Binance, testnet, holder_Mihai_Binance, position = 0, stop_loss_level = 0, take_profit_level = 0):
        
        self.timeframe_a = timeframe_a
        self.timeframe_a_resample = timeframe_a_resample
        self.symbol_Binance = symbol_Binance
        
        self.position = position
        self.tick_data = pd.DataFrame()
        self.stop_loss_level = stop_loss_level
        self.stop_loss = stop_loss
        self.take_profit_level = take_profit_level
        self.take_profit = take_profit
        self.leverage = leverage
        self.testnet = testnet
        
        self.bar_length_a = pd.to_timedelta(self.timeframe_a)
        
        self.api_key_Mihai_Binance = api_key_Mihai_Binance
        self.api_secret_Mihai_Binance = api_secret_Mihai_Binance
        self.client_Mihai_Binance = Client(self.api_key_Mihai_Binance, self.api_secret_Mihai_Binance, testnet=self.testnet)
        
        self.holder_Mihai_Binance = holder_Mihai_Binance

        #*****************add strategy-specific attributes here******************
        self.SMA_Trend = SMA_Trend
        self.EMA_Trend = EMA_Trend
        self.PCT_BUY = PCT_BUY
        self.PCT_SELL = PCT_SELL
        #************************************************************************
        self.sell_index = datetime.now() - timedelta(minutes=1000)
    
        
    def system_status_Binance(self):
        status = self.client_Mihai_Binance.get_system_status()
        return status
    
    
    def round_decimals_down(self, number:float, decimals:int=2):
        """
        Returns a value rounded down to a specific number of decimal places.
        """
        if not isinstance(decimals, int):
            raise TypeError("decimal places must be an integer")
        elif decimals < 0:
            raise ValueError("decimal places has to be 0 or more")
        elif decimals == 0:
            return ma.floor(number)

        factor = 10 ** decimals
        return ma.floor(number * factor) / factor
    
    def get_avg_price(self, ticker):
        price = float(self.client_Mihai_Binance.get_avg_price(symbol=ticker.upper()).get("price"))
        
        return price
    
    
    def get_binance_hist_data_simple_a(self, symbol):

        url = "https://api.binance.com/api/v3/klines"

        #startTime = str(int(startTime.timestamp() * 1000))
        #endTime = str(int(endTime.timestamp() * 1000))
        limit = "1000"
        
        interval = self.timeframe_a
        req_params = {"symbol" : symbol, 'interval' : interval, 'limit' : limit}

        df = pd.DataFrame(json.loads(requests.get(url, params= req_params).text))
        if (len(df.index) == 0):
            return None

        df = df.iloc[:, 0:6]
        df.columns = ['datetime', 'open', 'high', 'low', 'close', 'volume']
        df.index = [datetime.fromtimestamp(x / 1000.0) for x in df.datetime]
        df = df.rename(columns={"datetime": "Timestamp", "open": "Open", "high" : "High", "low": "Low", "close": "Close", "volume": "Volume"})
        df.index.name = "Timestamp"
        df = df[['Open', 'High', 'Low', 'Close', 'Volume']]
        return df
    

    def buy_strat_Mihai_Binance(self, ticker, reason = None):
        price = float(self.client_Mihai_Binance.get_avg_price(symbol=ticker.upper()).get("price"))
        amount = self.round_decimals_down(number = float(self.get_busd_balance_Binance("Mihai"))/price*0.99, decimals = 5)
        order = self.client_Mihai_Binance.order_market_buy(symbol=ticker.upper(), quantity=amount)
        
        df_order = pd.DataFrame(order)
        df_order = df_order[["symbol","orderId","transactTime", "executedQty","cummulativeQuoteQty","status","type","side"]]
        fills = pd.DataFrame(order["fills"])
        df_order["price"] = fills["price"]
        df_order["commission"] = fills["commission"]
        df_order["transactTimeBinance"] = pd.to_datetime(df_order["transactTime"], unit='ms')
        df_order["BucharestTime"] = datetime.now()
        df_order = df_order[["symbol","orderId","transactTimeBinance", "BucharestTime","executedQty","cummulativeQuoteQty","price","commission", "status","type","side"]]
        df_order["reason"] = reason
        df_order = df_order.set_index(pd.to_datetime(df_order.BucharestTime)).drop("BucharestTime", axis = 1)
        
        print("Buying {} of {} at {} on {} UTC time, {}.".format(df_order.executedQty.iloc[-1], df_order.symbol.iloc[-1], df_order.price.iloc[-1], df_order.transactTimeBinance.iloc[-1], reason))
        
        try:
            df = pd.read_csv("{}_orders.csv".format(self.holder_Mihai_Binance),  index_col = "BucharestTime")
            df = df.append(df_order.iloc[-1])
            df.to_csv("{}_orders.csv".format(self.holder_Mihai_Binance))
        except:
            df_order.to_csv("{}_orders.csv".format(self.holder_Mihai_Binance))
    
    def sell_strat_Mihai_Binance(self, ticker, reason = None):
        amount = self.round_decimals_down(number = float(self.get_btc_balance_Binance("Mihai")), decimals = 5)
        order = self.client_Mihai_Binance.order_market_sell(symbol=ticker.upper(), quantity=amount)
        
        df_order = pd.DataFrame(order)
        df_order = df_order[["symbol","orderId","transactTime", "executedQty","cummulativeQuoteQty","status","type","side"]]
        fills = pd.DataFrame(order["fills"])
        df_order["price"] = fills["price"]
        df_order["commission"] = fills["commission"]
        df_order["transactTimeBinance"] = pd.to_datetime(df_order["transactTime"], unit='ms')
        df_order["BucharestTime"] = datetime.now()
        df_order = df_order[["symbol","orderId","transactTimeBinance", "BucharestTime","executedQty","cummulativeQuoteQty","price","commission", "status","type","side"]]
        df_order["reason"] = reason
        df_order = df_order.set_index(pd.to_datetime(df_order.BucharestTime)).drop("BucharestTime", axis = 1)
        
        print("Selling {} of {} at {} on {} UTC time, {}.".format(df_order.executedQty.iloc[-1], df_order.symbol.iloc[-1], df_order.price.iloc[-1], df_order.transactTimeBinance.iloc[-1], reason))
            
        try:
            df = pd.read_csv("{}_orders.csv".format(self.holder_Mihai_Binance),  index_col = "BucharestTime")
            df = df.append(df_order.iloc[-1])
            df.to_csv("{}_orders.csv".format(self.holder_Mihai_Binance))
        except:
            df_order.to_csv("{}_orders.csv".format(self.holder_Mihai_Binance))
              
    def get_busd_balance_Binance(self, client):
        return self.client_Mihai_Binance.get_asset_balance(asset='BUSD')['free']
        
    def get_balances_Binance(self, client):
        balances = self.client_Mihai_Binance.get_account().get("balances")
        for item in balances:
            if float(item.get("free")) > 0:
                print(item)
                    
    
    def get_btc_balance_Binance(self, client):
        return self.client_Mihai_Binance.get_asset_balance(asset='BTC')['free']
    
    def stream_data_new(self):
        print("Bot started at {}".format(datetime.now()))
        while True:
            try:
                self.hist_data = self.get_binance_hist_data_simple_a("btcusdt".upper())
                self.closes = list(np.float64(self.hist_data.Close))
                self.highs = list(np.float64(self.hist_data.High))
                self.lows = list(np.float64(self.hist_data.Low))
                self.opens = list(np.float64(self.hist_data.Open))
                self.volumes = list(np.float64(self.hist_data.Volume))
                self.times = list(self.hist_data.index)

                self.data = pd.DataFrame({"Timestamp": self.times, "Open": self.opens, "High": self.highs, "Low": self.lows, "Close": self.closes, "Volume": self.volumes})
                self.data['Timestamp'] = pd.to_datetime(self.data['Timestamp'], unit = "ms")
                self.data = self.data.set_index(pd.to_datetime(self.data.Timestamp)).drop("Timestamp", axis = 1)

################################# Defining indicators ########################################################################
                raw = self.data
                raw["SMA_Trend"] = ta.SMA(raw["Close"], timeperiod=self.SMA_Trend)
                raw["EMA_Trend"] = ta.EMA(raw["Close"], timeperiod=self.EMA_Trend)
                self.data = raw                        #####################################################################################################################

                ################################ Defining trade conditions on open ######################################################

                if (self.data["Close"].iloc[-2] > self.data["SMA_Trend"].iloc[-2] and self.data["Low"].iloc[-2] < (self.data["SMA_Trend"].iloc[-2]*(1+self.PCT_BUY))) and (self.data["Close"].iloc[-2] > self.data["EMA_Trend"].iloc[-2]): ### signal to go long ###
                    if self.data.index[-1] > self.sell_index:
                        if self.position != 1:
                            self.buy_strat_Mihai_Binance(self.symbol_Binance, reason = "TA")
                            #print("buying", datetime.now())
                            self.position = 1

                            if self.stop_loss_level == 0:
                                self.stop_loss_level = self.data.Close.iloc[-1] - (self.data.Close.iloc[-1] * self.stop_loss)

                            if self.take_profit_level == 0:
                                self.take_profit_level = self.data.Close.iloc[-1] + (self.data.Close.iloc[-1] * self.take_profit)

                if self.data["High"].iloc[-1] > (self.data["SMA_Trend"].iloc[-1]*(1+self.PCT_SELL)): # signal to go neutral
                        if self.position != 0:
                            self.sell_strat_Mihai_Binance(self.symbol_Binance, reason = "SMA Profit")
                            #print("selling Trigger SMA Profit Close", datetime.now(), self.data)
                            self.position = 0
                            self.stop_loss_level = 0
                            self.take_profit_level = 0
                            self.sell_index = self.data.index[-1]

                if self.data["Close"].iloc[-2] < self.data["SMA_Trend"].iloc[-2]: # signal to go neutral
                        if self.position != 0:
                            self.sell_strat_Mihai_Binance(self.symbol_Binance, reason = "SMA Close")
                            #print("selling Trigger SMA Close", datetime.now(), self.data)
                            self.position = 0
                            self.stop_loss_level = 0
                            self.take_profit_level = 0
                            self.sell_index = self.data.index[-1]
                            
                            
                if self.data["Close"].iloc[-1] < self.stop_loss_level: # if stop-loss level is hit #
                    if self.position != 0:
                        self.sell_strat_Mihai_Binance(self.symbol_Binance, reason = "stop-loss")
                        #print("selling Trigger stop-loss", datetime.now(), self.data)
                        self.position = 0
                        self.stop_loss_level = 0
                        self.take_profit_level = 0
                        self.sell_index = self.data.index[-1]
                        
            except Exception as e:
                print(traceback.format_exc())
                continue
            
