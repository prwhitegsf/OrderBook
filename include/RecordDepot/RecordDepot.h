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



#include "MatchedOrders.h"
#include "OrderTypes.h"
#include "OverwritingVector.h"


/**
 * @brief class for holding and querying orders
 * @tparam R Record
 */
template<typename R>
class RecordDepot
{

    std::unordered_map<ID,R> accepted_;
    std::unordered_map<ID,R> completed_;

    OverwritingVector<order::Matched> matched_;

    //std::unordered_set<ID> last_processed_;
    std::vector<ID> last_processed_;
    Time ts_;

    void timestamp();

    void update_matched();

    ID process_limit_fills(ID id);
    ID process_partial_fill(order::PartialFill o);
    ID process_market_fill(order::MarketFill o);
    ID process_state_update(order::StateUpdate o);

public:

    /// @return reference to table of unfilled, but accepted orders, includes partial fills
    const std::unordered_map<ID,R>& accepted() const;

    /// @return reference to table of filled / completed orders, includes cancelled and rejected orders
    const std::unordered_map<ID,R>& completed() const;

    /// @return a list with the IDs of all orders changed in the last transaction
   // std::unordered_set<ID> last_processed();
    const std::vector<ID>& last_processed();
    /// @param id desired order id
    /// @return a copy of the order record
    R find_order_record(ID id);

    /// @brief creates record from incoming order and adds to accepted
    /// @param o incoming order
    void make_order_record(order::Submitted o);

    /// @brief copies matched orders from orderbook to local vector
    /// @param matched container in orderbook
    void record_matched_orders(const OverwritingVector<order::Matched>& matched);

    /// @brief update the records of the orders pulled in by record_matched_orders
    void update_order_records();

    void clear_matched(){ matched_.clear(); }
};


template<typename R>
const std::unordered_map<ID,R>& RecordDepot<R>::accepted() const {return accepted_;}

template<typename R>
const std::unordered_map<ID,R>& RecordDepot<R>::completed() const {return completed_;}

template<typename R>
R RecordDepot<R>::find_order_record(const ID id)
{
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
void RecordDepot<R>::record_matched_orders(const OverwritingVector<order::Matched>&  matched)
{
    for (const auto& m : matched)
        matched_.push_back(m);
}


template<typename R>
void RecordDepot<R>::make_order_record(order::Submitted o) {

    std::visit([this](auto& ord)
   {
       accepted_.emplace(ord.id,R(ord));
   },o);
}


template<typename R>
void RecordDepot<R>::update_order_records()
{
    timestamp();
    update_matched();
    matched_.clear();
}



template<typename R>
void RecordDepot<R>::update_matched()
{

    last_processed_.clear();
    for (const auto& [limit, partial,market,state] : matched_)
    {
        for (const ID id : limit)
            last_processed_.push_back(process_limit_fills(id));

        if (partial.id)
            last_processed_.push_back(process_partial_fill(std::move(partial)));

        if (market.id)
            last_processed_.push_back(process_market_fill(std::move(market)));

        if (state.id)
            last_processed_.push_back(process_state_update(std::move(state)));

    }
}

template<typename R>
const std::vector<ID>& RecordDepot<R>::last_processed()
{
    return last_processed_;
}

template<typename R>
ID RecordDepot<R>::process_partial_fill(order::PartialFill o)
{
    accepted_[o.id].update(o,ts_);
    return o.id;
}

template<typename R>
ID RecordDepot<R>::process_limit_fills(ID id) {
    //accepted_[id].update(ts_);
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

template<typename R>
ID RecordDepot<R>::process_state_update(order::StateUpdate o)
{
    accepted_[o.id].update(o.state,ts_);

    if (o.state == order::OrderState::CANCELLED || o.state == order::OrderState::REJECTED)
    {
        completed_.insert(std::move(accepted_.extract(o.id)));
    }

    return o.id;
}


#endif //ORDERRECORDS_H


#endif //ORDERBOOK_RECORDDEPOT_H