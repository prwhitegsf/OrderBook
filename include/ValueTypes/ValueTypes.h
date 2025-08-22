//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_VALUETYPES_H
#define ORDERBOOK_VALUETYPES_H

#include <chrono>

using ID = uint_least32_t;
using Qty = uint_least16_t;
using Price = uint_least16_t;

using Time = std::chrono::time_point<std::chrono::high_resolution_clock>;
using Clock = std::chrono::high_resolution_clock;


#endif //ORDERBOOK_VALUETYPES_H