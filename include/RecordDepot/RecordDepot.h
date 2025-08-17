//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_RECORDDEPOT_H
#define ORDERBOOK_RECORDDEPOT_H


//
// Created by prw on 7/30/25.
//

#ifndef ORDERRECORDS_H
#define ORDERRECORDS_H


#include <unordered_set>
#include <unordered_map>
#include <variant>
#include <chrono>



#include "ProcessedOrders.h"
#include "OrderTypes.h"


/**
 * @brief class for holding and querying orders
 * @tparam R Record
 */
template<typename R>
class RecordDepot
{

    std::unordered_map<ID,R> accepted_;
    std::unordered_map<ID,R> completed_;
    std::vector<order::OrderFills> order_fills_;
    std::vector<order::StateUpdate> order_states_;
    std::unordered_set<ID> last_processed_;

    Time ts_;

    void timestamp();
    void record_states(std::vector<order::StateUpdate>&& state_updates);
    void record_fills(std::vector<order::OrderFills>&& fills);

    void update_states();
    void update_fills();

    ID process_limit_fills(ID id);
    ID process_partial_fill(order::PartialFill o);
    ID process_market_fill(order::MarketFill o);

public:
    ///
    /// @return reference to table of unfilled, but accepted orders, includes partial fills
    const std::unordered_map<ID,R>& accepted() const;
    /// @return reference to table of filled / completed orders, includes cancelled and rejected orders
    const std::unordered_map<ID,R>& completed() const;

    /// @return a list with the IDs of all orders changed in the last transaction
    std::unordered_set<ID> last_processed();


    /// @param id desired order id
    /// @return a copy of the order record
    R find_order_record(ID id);

    /// @brief creates record from incoming order and adds to accepted
    /// @param o incoming order
    void make_order_record(order::Submitted o);
    /// @brief gets the OrderFills and StateUpdates from Orderbook and moves them into local containers for processing
    /// @param processed_orders returned by order_book.get_processed_orders()
    void record_processed_orders(order::Processed&& processed_orders);
    /// @brief update the records of the orders pulled in by record_processed_orders
    void update_order_records();



};


template<typename R>
const std::unordered_map<ID,R>& RecordDepot<R>::accepted() const {return accepted_;}

template<typename R>
const std::unordered_map<ID,R>& RecordDepot<R>::completed() const {return completed_;}

template<typename R>
R RecordDepot<R>::find_order_record(const ID id) {

    if (accepted_.contains(id)) return accepted_[id];
    if (completed_.contains(id)) return completed_[id];
    return  {};
}

template<typename R>
void RecordDepot<R>::timestamp()
{
    ts_ = Clock::now();
}

template <typename R>
void RecordDepot<R>::record_processed_orders(order::Processed&&  processed_orders)
{
    if (!processed_orders.second.empty() && processed_orders.second.back().id != 0)
        record_states(std::move(processed_orders.second));

    if (!processed_orders.first.empty() && processed_orders.first.back().market_fill.id != 0)
        record_fills(std::move(processed_orders.first));
}


template<typename R>
void RecordDepot<R>::make_order_record(order::Submitted o) {

    std::visit([this](auto& ord)
   {
       accepted_.emplace(ord.id,R(ord));

   },o);
}

template<typename R>
void RecordDepot<R>::record_states(std::vector<order::StateUpdate>&& state_updates) {

    order_states_ = std::move(state_updates);
}

template<typename R>
void RecordDepot<R>::record_fills(std::vector<order::OrderFills>&& fills) {

    order_fills_ = std::move(fills);
}



template<typename R>
void RecordDepot<R>::update_order_records() {

    timestamp();
    update_states();
    update_fills();

}


template<typename R>
void RecordDepot<R>::update_states()
{
    while (!order_states_.empty())
    {
        auto [id,state] = order_states_.back();
        order_states_.pop_back();

        last_processed_.insert(id);

        accepted_[id].update(state,ts_);

        if (state == order::OrderState::CANCELLED || state == order::OrderState::REJECTED)
        {
            completed_.insert(std::move(accepted_.extract(id)));
        }
    }
}

template<typename R>
void RecordDepot<R>::update_fills()
{
    while (!order_fills_.empty())
    {
        auto [limit, partial,market] = order_fills_.back();
        order_fills_.pop_back();

        for (ID id : limit)
            last_processed_.insert(process_limit_fills(id));

        if (partial.id)
            last_processed_.insert(process_partial_fill(std::move(partial)));

        if (market.id)
            last_processed_.insert(process_market_fill(std::move(market)));
    }
}

template<typename R>
std::unordered_set<ID> RecordDepot<R>::last_processed()
{
    auto x = last_processed_;
    last_processed_.clear();
    return x;
}

template<typename R>
ID RecordDepot<R>::process_partial_fill(order::PartialFill o)
{
    accepted_[o.id].update(o,ts_);
    return o.id;
}

template<typename R>
ID RecordDepot<R>::process_limit_fills(ID id) {

    completed_.insert(std::move(accepted_.extract(id)));
    completed_[id].update(ts_);
    return id;
}

template<typename R>
ID RecordDepot<R>::process_market_fill(order::MarketFill o)
{
    if (!accepted_[o.id].is_market_to_limit(o.qty))
    {
        completed_.insert(std::move(accepted_.extract(o.id)));
        completed_[o.id].update(o,ts_);
    }
    else
    {
        accepted_[o.id].update_market_limit(o,ts_);
    }
    return o.id;
}


#endif //ORDERRECORDS_H


#endif //ORDERBOOK_RECORDDEPOT_H