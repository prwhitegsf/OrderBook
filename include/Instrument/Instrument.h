//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_INSTRUMENT_H
#define ORDERBOOK_INSTRUMENT_H


#include <string>
#include "ValueTypes.h"

/// @brief a set of parameters used to initialize the order book
///
struct Instrument
{
    std::string name_;
    Price num_prices_;
    Price starting_bid_;
    Price starting_ask_;
    Price protection_;

    Instrument() : name_("demo"), num_prices_(100), starting_bid_(49), starting_ask_(50), protection_(2) {}
    Instrument(std::string&& name, const Price num_prices, const Price starting_bid, const Price starting_ask, const Price protection)
        : name_(std::move(name)), num_prices_(num_prices), starting_bid_(starting_bid), starting_ask_(starting_ask), protection_(protection) {}
};
#endif //ORDERBOOK_INSTRUMENT_H