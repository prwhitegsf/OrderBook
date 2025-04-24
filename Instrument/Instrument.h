#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <string>
#include <chrono>
#include "OrderLists/ClientOrderList.h"


using Expiry = std::chrono::year_month_day;

class Instrument {


private:
    static constexpr  std::string name_{"ES"};
    static constexpr double max_price_{100};
    static constexpr double min_price_{0};
    static constexpr double price_increment_{1};

    size_t bid_idx_{49};
    size_t ask_idx_{51};

    double bid_price_{0};
    double ask_price_{0};



    const std::chrono::year_month_day contract_expiry_;

public:
    ClientOrderList client_order_list_;

    Instrument()
        : contract_expiry_(std::chrono::December / std::chrono::day(15)/ std::chrono::year(2025)) {}

    static std::string name() { return name_; }

    static double max_price(){ return max_price_; }

    static  double min_price() { return min_price_; }

    static double price_increment() { return price_increment_; }

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


    static size_t idx_from_price(double const price)
    { return static_cast<size_t>((price - min_price_)/(price_increment_)); }


    template<typename T>
    requires std::is_arithmetic_v<T>
    static double price_from_idx(T const index)
    { return (static_cast<double>(index) * price_increment_) + min_price_; }
};





#endif