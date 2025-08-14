//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_LEVEL_H
#define ORDERBOOK_LEVEL_H


#include <deque>
#include "OrderTypes.h"

struct Level {

    Qty depth{};
    std::deque<order::Limit> orders;
    size_t count()const{ return orders.size();};
};


#endif //ORDERBOOK_LEVEL_H