# 📈 Legacy TA Strategies Trading Suite

This folder contains a collection of scripts and notebooks for building, optimizing, and deploying rule-based trading strategies using legacy technical indicators such as moving averages, support/resistance levels, MACD, and more.

The suite is organized into the following components:

---

## 01_Strategy_Backtests

Contains notebooks for **testing fixed-parameter strategies** on historical data.

### Files:
- `Testing_MASR_V1_it_12H.ipynb` — Tests MASR strategy on 12h candles.
- `Testing_EMApMACDextraTPSLdiff_it_4h.ipynb` — Tests an EMA/MACD strategy with TP/SL logic on 4h data.

These notebooks allow for rapid iteration, result visualization, and performance evaluation using hardcoded strategy parameters.

---

## 02_Strategy_Optimization

Contains code for **hyperparameter tuning** using iterative search loops.

### Files:
- `Optimizing_MASR_V1_it_12H.ipynb`
- `Optimizing_EMApMACDextraTPSLdiff_it_4h.ipynb`

Supporting utilities:
- `IterativeBase.py` — Shared base for optimization workflows.
- `IterativeBaseMASR.py`, `IterativeOPT_MASR_V1.py`, `IterativeOPTfilterEMAmacdproEXTRAtpslDiff.py` — Strategy-specific iterative optimization runners.

These scripts execute systematic tuning across multiple dimensions (e.g., MA windows, TP/SL levels) to maximize performance metrics.

---

## 03_Live_Trading

Includes a functional prototype for **live execution** on Binance.

### Files:
- `Spot_MASR_V1.ipynb` — Jupyter interface to configure and deploy live trades.
- `Live_trading_Spot_MASR_Script_v1.py` — Backend class for streaming data, placing orders, and managing open positions.

The system supports real-time trade triggers based on signal confluence, position management, and logs order history locally.

---

## Notes

- These strategies are for **educational and research** purposes. They represent simplified rule-based trading logic, which may no longer perform in production markets.
- API keys and sensitive credentials should be stored securely and **not hardcoded** in production deployments.
- Real capital trading should only be done with sufficient safeguards, risk controls, and robust infrastructure.

---

## License

This project is shared for non-commercial and educational purposes.
