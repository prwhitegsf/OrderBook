#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <string>
#include <vector>
#include <chrono>

#include "OrderLists/ClientOrderList.h"


using Expiry = std::chrono::year_month_day;
//using namespace std::chrono;
class Instrument {


private:
    std::string name_{"ES"};
    double max_price_{20};
    double min_price_{1};
    double price_increment_{0.25};

    double bid_{10.75};
    double ask_{11.0};




    const std::chrono::year_month_day contract_expiry_;

public:
    ClientOrderList client_order_list_;

    Instrument()
        : contract_expiry_(std::chrono::December / std::chrono::day(15)/ std::chrono::year(2025)) {}

    [[nodiscard]] std::string name() const
    {
        return name_;
    }

    [[nodiscard]] double max_price() const
    {
        return max_price_;
    }

    [[nodiscard]] double min_price() const
    {
        return min_price_;
    }

    [[nodiscard]] double price_increment() const
    {
        return price_increment_;
    }

    [[nodiscard]] double bid() const
    {
        return bid_;
    }

    [[nodiscard]] double ask() const
    {
        return ask_;
    }


    void update_bid(double price)
    {
        bid_ = price;
    }

    void update_ask(double price)
    {
        ask_ = price;
    }

    void update_client_orders(auto&& order_updates)
    {
        client_order_list_.update_order_list(std::forward<decltype(order_updates)>(order_updates));
    }
};





#endif