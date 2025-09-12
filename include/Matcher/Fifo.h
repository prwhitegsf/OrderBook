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

    /// @brief match BuyMarket
    /// @return matched orders resulting from market order
    const order::Matched& match(order::BuyMarket o);
    /// @brief match SellMarket
    /// @return matched orders resulting from market order
    const order::Matched& match(order::SellMarket o);
    /// @brief match BuyMarketLimit
    /// @return matched orders resulting from market limit order
    const order::Matched& match(order::BuyMarketLimit o);
    /// @brief match SellMarketLimit
    /// @return matched orders resulting from market limit order
    const order::Matched& match(order::SellMarketLimit o);

    /// @brief match BuyLimit
    /// @return matched orders with state updated limit order
    const order::Matched& match(order::BuyLimit o);
    /// @brief match SellLimit
    /// @return matched orders with state updated limit order
    const order::Matched& match(order::SellLimit o);

    /// @brief find resting order and remove
    /// @return matched with state updated cancelled limit order
    const order::Matched& match(order::Cancel o);
    /// @brief rejected order
    /// @return matched with state updated rejected order
    const order::Matched& match(order::Rejected o);

    [[nodiscard]] const Level& level(size_t idx)const;


private:

    std::vector<Level> level_;
    order::Matched matched_;

    void reset_matched(ID id, Qty qty);
    void reset_matched(ID id, order::OrderState state);

    const order::Matched& limit(auto o);
    const order::Matched& market(auto o, auto&& dir);
    const order::Matched& market_limit(auto o, auto&& dir);
    int fill_level(auto& o);
    int fill_orders(auto& o);
    int fill_remaining(auto& o);
    static double fill_price(Price price, Qty filled_qty, Qty full_qty);
};


#endif //ORDERBOOK_FIFO_H