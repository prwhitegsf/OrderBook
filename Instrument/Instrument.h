#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <string>
#include <chrono>
#include "OrderLists/ClientOrderList.h"


using Expiry = std::chrono::year_month_day;

class Instrument {


private:
    std::string name_;
    double max_price_;
    double min_price_;
    double price_increment_;

    size_t bid_idx_;
    size_t ask_idx_;

    double bid_price_{0};
    double ask_price_{0};



    const std::chrono::year_month_day contract_expiry_;

public:
    ClientOrderList client_order_list_;

    Instrument(std::string name, double max_price, double min_price,double price_increment,size_t bid, size_t ask)
        :   name_(name),max_price_(max_price),min_price_(min_price),
            price_increment_(price_increment),bid_idx_(bid),ask_idx_(ask),
            contract_expiry_(std::chrono::December / std::chrono::day(15)/ std::chrono::year(2025)) {}

    std::string name() { return name_; }

    double max_price() const { return max_price_; }

    double min_price() const { return min_price_; }

    double price_increment() const { return price_increment_; }

    [[nodiscard]] double bid() const { return bid_idx_; }

    [[nodiscard]] double ask() const { return ask_idx_; }


    void update_bid(const int price_idx)
    {
        bid_idx_ = price_idx;
        bid_price_ = price_from_idx(price_idx);
    }

    void update_ask(const int price_idx)
    {
        ask_idx_ = price_idx;
        ask_price_ = price_from_idx(price_idx);
    }

    void update_client_orders(auto&& order_updates)
    { client_order_list_.update_order_list(std::forward<decltype(order_updates)>(order_updates)); }


    size_t idx_from_price(double const price)const
    { return static_cast<size_t>((price - min_price_)/(price_increment_)); }


    template<typename T>
    requires std::is_arithmetic_v<T>
     double price_from_idx(T const index)const
    { return (static_cast<double>(index) * price_increment_) + min_price_; }
};





#endif