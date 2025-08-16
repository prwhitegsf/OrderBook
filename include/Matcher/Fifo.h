//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_FIFO_H
#define ORDERBOOK_FIFO_H



#include "Level.h"
#include "ProcessedOrders.h"
#include "OrderTypes.h"


/**
 * First In, First Out Matching Strategy
 *
 * @brief orders are filled in the order they are received
 * Holds a vector of Level's representing the orders at each price and a set of functions to
 * perform operations on the Levels
 *
 * Orders are routed to the appropriate match() function based on their type
 *
 * Constructor requires the number of prices to use in Level
 */
class Fifo {


public:
    Fifo() = delete;
    explicit Fifo(const int num_prices) : level_(num_prices){}

    order::OrderFills match(order::BuyMarket o);
    order::OrderFills match(order::SellMarket o);

    order::OrderFills match(order::BuyMarketLimit o);
    order::OrderFills match(order::SellMarketLimit o);



    order::StateUpdate match(order::BuyLimit o);
    order::StateUpdate match(order::SellLimit o);

    order::StateUpdate match(order::Cancel o);

    [[nodiscard]] const Level& level(size_t idx)const;


private:
    std::vector<Level> level_;
    order::StateUpdate limit(auto o);
    order::OrderFills market(auto o, auto&& dir);
    order::OrderFills market_limit(auto o, auto&& dir);
    int fill_level(auto& o, order::OrderFills& fills);
    int fill_orders(auto& o, order::OrderFills& fills);
    int fill_remaining(auto& o, order::OrderFills& fills);
    static float fill_price(Price price, Qty filled_qty, Qty full_qty);
};


#endif //ORDERBOOK_FIFO_H