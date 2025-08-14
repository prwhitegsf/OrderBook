//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_VALUETYPES_H
#define ORDERBOOK_VALUETYPES_H

#include <chrono>

using ID = unsigned int;
using Qty = unsigned short int;
using Price = unsigned short int;

using Time = std::chrono::time_point<std::chrono::high_resolution_clock>;
using Clock = std::chrono::high_resolution_clock;


#endif //ORDERBOOK_VALUETYPES_H