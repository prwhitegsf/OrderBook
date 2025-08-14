//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_EVALUATOR_H
#define ORDERBOOK_EVALUATOR_H

#include <queue>


#include "OrderTypes.h"
#include "ProcessedOrders.h"
#include "Dom.h"

/**
 * @brief accepts, rejects and splits orders as needed, updates dom
 * Split and accepted orders get pushed to accepted_q_ where they will
 * get matched by the matcher
 * Rejected orders have their state updated to Rejected and do not get sent to the matcher
 */
class Evaluator
{
    Dom& d_;
    std::queue<order::Pending>& accepted_q_;
    std::vector<order::StateUpdate>& state_update_q_;

    void set_depth(Price price, Qty depth) const;
    void add_depth(Price price, Qty depth) const;
    void subtract_depth(Price price, Qty depth) const;

    /// @brief take all the orders at this price and set the dom depth to 0
    Price fill_level(Price price, Qty& qty) const;

    /// @brief increments ask while depth = 0
    void next_non_zero_ask() const;
    /// @brief decrements bid while depth = 0
    void next_non_zero_bid() const;

    void place_limit(order::BuyLimit o) const;
    void place_limit(order::SellLimit o) const;

    void place_limit_inside_spread(order::BuyLimit o, Price& update_price) const;
    void place_limit_inside_spread(order::SellLimit o, Price& update_price) const;

    void push_accepted (order::Pending&& o)const;

public:
    ///
    /// @param dom reference to dom
    /// @param accepted_q reference to accepted
    /// @param state_updates reference to state_updates
    Evaluator(Dom& dom, std::queue<order::Pending>& accepted_q, std::vector<order::StateUpdate>& state_updates);

    void evaluate_order(order::BuyLimit o) const;
    void evaluate_order(order::BuyMarket o) const;
    void evaluate_order(order::SellLimit o) const;
    void evaluate_order(order::SellMarket o) const;
    void evaluate_order(order::Cancel o) const;

};


#endif //ORDERBOOK_EVALUATOR_H