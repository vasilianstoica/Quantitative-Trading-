// Logger.h
#pragma once
#include <fstream>
#include <iostream>
#include <sstream>

class CSVLogger : public std::streambuf {
public:
    explicit CSVLogger(const std::string& file_name)
        : file_(file_name, std::ios::app),     // append-mode
          old_buf_(std::cout.rdbuf())         // remember old buffer
    {
        std::cout.rdbuf(this);                // redirect cout → this
    }
    ~CSVLogger() override {                   // restore on destruction
        std::cout.rdbuf(old_buf_);
        file_.flush();
        file_.close();
    }

    // every << to std::cout ends up here -------------
    int overflow(int ch) override {
        if (ch == EOF) return EOF;
        file_.put(static_cast<char>(ch));     // write to CSV
        return old_buf_->sputc(ch);           // write to console
    }

    // helper for logging user input ------------------
    template<typename T>
    void logInput(const std::string& prompt, T& var) {
        std::cout << prompt;          // goes to both console & csv
        std::cin  >> var;
        file_ << ",\"" << var << "\"\n"; // comma-separated entry
    }

private:
    std::ofstream     file_;
    std::streambuf*   old_buf_;       // original cout buffer
};
