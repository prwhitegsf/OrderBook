//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_ORDERBOOK_H
#define ORDERBOOK_ORDERBOOK_H


#include <numeric>
#include <iostream>

#include "Instrument.h"
#include "MatcherConcept.h"
#include "ProcessedOrders.h"
#include "Dom.h"
#include "Evaluator.h"


/**
 * @brief Receives and executes orders, then outputs the results of each trade
* Consists of:
    - Depth of market (Dom)
        - a container with the total number of available contracts at each price
        - the best prices participants are willing to buy (bid) or sell (ask)
        - the maximum number of prices a market order can execute over at once (protection)
    - An interface for querying the Dom
    - Evaluator
        - a set of functions for
            - updating the Dom when new orders are excepted
            - rejecting orders
            - splitting market and limit orders into market limit orders where appropriate
    - A Matching strategy
        - different markets use different algorithms to match orders
        - so we inject our matcher into the order book via a template
        - we use our Matcher Concept file to define the required interface a matching strategy must have
        - our strategy is First In, First Out (Fifo), see above
    - Containers for holding orders in various states of process
 *  Initialized with an Instrument object
 * @tparam Matcher Inject Matching engine via strategy pattern
 */
template<Is_Matcher Matcher>
class OrderBook {

    Instrument inst_;
    Price num_prices_;
    Matcher matcher_;

    Dom d_;
    Evaluator evaluator_;

    std::queue<order::Submitted> submitted_q_;
    std::queue<order::Pending> pending_q_;

    std::vector<order::OrderFills> order_fills_;
    std::vector<order::StateUpdate> order_states_;



public:
    explicit OrderBook(const Instrument& inst)
    :   inst_(inst), num_prices_(inst_.num_prices_), matcher_(num_prices_),
        d_(num_prices_, inst_.starting_ask_, inst_.starting_bid_,inst_.protection_),
        evaluator_(d_,pending_q_,order_states_){}

    explicit OrderBook(Instrument&& inst)
    :   inst_(std::move(inst)), num_prices_(inst_.num_prices_), matcher_(num_prices_),
        d_(num_prices_, inst_.starting_ask_, inst_.starting_bid_,inst_.protection_),
        evaluator_(d_,pending_q_,order_states_){}

    /// @brief receive order and push to submit queue
    /// @param o incoming order from client api or order generator
    void submit_order(order::Submitted o);;

    /// @ brief determine order validity, update dom, and split order if needed
    void accept_order();

    /// @brief connects ids and quantities with orders to record transactions/fills
    void match_order();

    /// @brief move processed orders out of OrderBook, typically to Record Depot
    /// @return pair of vectors containing all OrderFills and StateUpdates from last transaction
    order::Processed get_processed_orders();

    //Dom Interface
    /// @return best bid price
    [[nodiscard]] Price bid() const;
    /// @return best ask price
    [[nodiscard]] Price ask() const;
    /// @return bid + (ask-bid)/2
    [[nodiscard]] Price mid() const;
    /// @return max number of prices a market order can take at once
    [[nodiscard]] Price protection() const;
    /// @return total number of prices in the dom
    [[nodiscard]] Price num_prices() const;
    /// @return highest tradable price: num_prices - protection x 2
    [[nodiscard]] Price max_price() const;
    /// @return lowest tradable price: protection x 2
    [[nodiscard]] Price min_price() const;
    /// @return number of contracts available for buy/sell at given price
    [[nodiscard]] Qty depth(Price idx) const;
    /// @return number of contracts available for buy/sell at each price, over range of prices
    [[nodiscard]] std::vector<Qty> depth(Price first, Price last) const;
    /// @brief get the maximum number of contracts available to buy at market
    /// @return the number of contracts between the ask and ask+protection
    [[nodiscard]] Qty max_qty_buy() const;
    /// @brief get the maximum number of contracts available to sell at market
    /// @return the number of contracts between the bid and bid-protection
    [[nodiscard]] Qty max_qty_sell() const;
    /// @return a vector containing the number of orders at each price
    std::vector<int> order_counts();

private:

    void push_matched(order::OrderFills&& fills);
    void push_matched(order::StateUpdate&& updates);

};




template <Is_Matcher Matcher>
void OrderBook<Matcher>::submit_order(order::Submitted o)
{
    submitted_q_.emplace(o);
}

template <Is_Matcher Matcher>
void OrderBook<Matcher>::accept_order()
{
    while(!submitted_q_.empty())
    {
        std::visit([this](auto&& o)
        {
            evaluator_.evaluate_order(o);

        },submitted_q_.front());

        submitted_q_.pop();
    }
}


template <Is_Matcher Matcher>
void OrderBook<Matcher>::match_order()
{
    while (!pending_q_.empty())
    {
        std::visit([this](auto&& o)
        {
            push_matched(matcher_.match(o));

        },pending_q_.front());

        pending_q_.pop();
    }
}


template <Is_Matcher Matcher>
order::Processed OrderBook<Matcher>::get_processed_orders()
{
    return std::move(std::make_pair(std::move(order_fills_), std::move(order_states_)));
}


template <Is_Matcher Matcher>
void OrderBook<Matcher>::push_matched(order::OrderFills&& fills)
{
    order_fills_.emplace_back(fills);
}

template <Is_Matcher Matcher>
void OrderBook<Matcher>::push_matched(order::StateUpdate&& updates)
{
    order_states_.emplace_back(updates);
}




template <Is_Matcher Matcher>
Price OrderBook<Matcher>::bid() const {return d_.bid;}

template <Is_Matcher Matcher>
Price OrderBook<Matcher>::ask() const {return d_.ask;}

template <Is_Matcher Matcher>
Price OrderBook<Matcher>::mid() const {return d_.bid + ((d_.ask - d_.bid)/2);}

template <Is_Matcher Matcher>
Price OrderBook<Matcher>::protection() const {return d_.protection;}

template <Is_Matcher Matcher>
Price OrderBook<Matcher>::num_prices() const {return num_prices_;}

template <Is_Matcher Matcher>
Price OrderBook<Matcher>::max_price() const {return num_prices_ - (d_.protection*2);}

template <Is_Matcher Matcher>
Price OrderBook<Matcher>::min_price() const {return d_.protection*2;}

template <Is_Matcher Matcher>
Qty OrderBook<Matcher>::depth(const Price idx) const {return d_.dom.at(idx);}

template <Is_Matcher Matcher>
std::vector<Qty> OrderBook<Matcher>::depth(const Price first, const Price last) const
{
    return {d_.dom.begin() + first, d_.dom.begin() + last};
}


template <Is_Matcher Matcher>
Qty OrderBook<Matcher>::max_qty_buy() const {
    return std::accumulate(
        d_.dom.begin()+d_.ask,
        d_.dom.begin()+d_.ask+d_.protection+1,
        0);
}

template <Is_Matcher Matcher>
Qty OrderBook<Matcher>::max_qty_sell() const {
    return std::accumulate(
        d_.dom.begin()+d_.bid - d_.protection,
        d_.dom.begin()+d_.bid,
        0);
}

template <Is_Matcher Matcher>
std::vector<int> OrderBook<Matcher>::order_counts() {
    auto result = std::vector<int>();
    for (int i{}; i < num_prices_ - 1; ++i)
        result.push_back(matcher_.level(i).count());

    return result;
}


#endif //ORDERBOOK_ORDERBOOK_H