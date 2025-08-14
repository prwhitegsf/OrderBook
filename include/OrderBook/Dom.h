//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_DOM_H
#define ORDERBOOK_DOM_H

#include <vector>
#include "ValueTypes.h"


struct Dom
{

    std::vector<Qty> dom;
    Price ask;
    Price bid;
    Price protection;


    Dom(const size_t num_prices_t, const Price ask_t, const Price bid_t, const Price protection_t)
        : dom(num_prices_t), ask(ask_t), bid(bid_t), protection(protection_t) {}

};

#endif //ORDERBOOK_DOM_H