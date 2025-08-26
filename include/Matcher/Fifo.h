//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_FIFO_H
#define ORDERBOOK_FIFO_H



#include "Level.h"
#include "MatchedOrders.h"
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

    const order::Matched& match(order::BuyMarket o);
    const order::Matched& match(order::SellMarket o);

    const order::Matched& match(order::BuyMarketLimit o);
    const order::Matched& match(order::SellMarketLimit o);

    const order::Matched& match(order::BuyLimit o);
    const order::Matched& match(order::SellLimit o);

    const order::Matched& match(order::Cancel o);
    const order::Matched& match(order::Rejected o);

    [[nodiscard]] const Level& level(size_t idx)const;


private:
    std::vector<Level> level_;
    order::Matched matched_;
    const order::Matched& limit(auto o);
    const order::Matched& market(auto o, auto&& dir);
    const order::Matched& market_limit(auto o, auto&& dir);
    int fill_level(auto& o);
    int fill_orders(auto& o);
    int fill_remaining(auto& o);
    double fill_price(Price price, Qty filled_qty, Qty full_qty);
};


#endif //ORDERBOOK_FIFO_H