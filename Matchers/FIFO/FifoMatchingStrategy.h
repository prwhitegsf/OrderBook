#ifndef FIFO_H
#define FIFO_H

#include <ranges>
//#include "../../OrderTypes/Orders.h"
template<template<typename>class Lev>
class FifoMatchingStrategy
{

    using Level = Lev<Order>;
    using Dom = std::vector<Level>;
    using DomIter = typename std::vector<Level>::iterator;


    static constexpr short UP{1};
    static constexpr short DOWN{-1};


    size_t num_prices_;

    Dom dom_;

    DomIter bid_{ dom_.begin() };
    DomIter ask_{ dom_.end() };
    DomIter level_{ dom_.begin() };

public:

    std::vector<OrderUpdate> order_updates_;
    size_t ask_idx(){return std::distance(dom_.begin(), ask_);};
    size_t bid_idx(){return std::distance(dom_.begin(), bid_);};

    size_t idx(const DomIter ptr){return std::distance(dom_.begin(), ptr);};
    size_t num_prices() const {return num_prices_;};

    DomIter dom_begin() {return dom_.begin();};
    DomIter dom_end() {return dom_.end();};

    void set_bid(const DomIter ptr){bid_ = ptr;};
    void set_ask(const DomIter ptr){ask_ = ptr;};

    Level& get_level(size_t idx){ return dom_[idx];};

    const Dom& get_dom() {return dom_;};


    FifoMatchingStrategy()=default;

    explicit FifoMatchingStrategy(const size_t num_prices)
        : num_prices_(num_prices),dom_(num_prices){}



    OrderUpdate match(BuyLimit<Order>&& o)
    {
        level_ = dom_.begin()+o.order.price_;
        if (level_ < ask_) // Not crossing the spread
        {
            if (level_ > bid_) // lifting the bid
                bid_ = level_;

            level_->append_limit_order(o.order);
            return record_order_update(o.order,OrderState::PENDING);

        }

        return crossing_spread(std::move(o.order),
            ask_,bid_,UP,
            std::less_equal<>());
    }

    OrderUpdate match(SellLimit<Order>&& o)
    {
        level_ = dom_.begin()+o.order.price_;
        if (level_ > bid_)
        {
            if (level_ < ask_)
                ask_ = level_;

            level_->append_limit_order(o.order);
            return record_order_update(o.order,OrderState::PENDING);

        }

        return crossing_spread(std::move(o.order),
            bid_,ask_,DOWN,
            std::greater_equal<>());
    }

    template<class U>
    OrderUpdate crossing_spread(Order&& o,DomIter& maker,DomIter& taker, const int direction, U cmpr)
    {


        const double initial_qty = o.qty_;

        o.price_ = 0;
        while (cmpr(maker, level_ ) && o.qty_ >= maker->depth_)
        {

            o.price_ += idx(maker) * (maker->depth_/initial_qty);
            o.qty_ -= maker->depth_;
            maker->depth_ = 0;

            record_fills(maker);
            maker->clear();

            taker = maker;
            std::advance(maker,direction);


        }
        // This is where it should eventually fill...not great though
        o.price_  += idx(taker) * (o.qty_/initial_qty);
        if (o.qty_)
            taker->append_limit_order(o);


        order_updates_.push_back(OrderUpdate(o.id_,o.qty_,o.total_,o.price_,OrderState::PARTIALLY_FILLED));

        return order_updates_.back();

    }


    OrderUpdate match(BuyMarket<Order>&& o)
    {
        fill_levels(o.order,ask_,UP);
        return fill_orders_at_level(o.order,ask_);
    }

    OrderUpdate match(SellMarket<Order>&& o)
    {
        fill_levels(o.order,bid_,DOWN);
        return fill_orders_at_level(o.order,bid_);
    }

    OrderUpdate match(Cancel<Order>&& o)
    {
        level_ = dom_.begin() + o.order.price_;

        auto&& cancelled = level_->remove_order(o.cancel_id_);

        order_updates_.push_back(OrderUpdate(cancelled.id_,cancelled.qty_,cancelled.qty_,o.order.price_,OrderState::CANCELLED));

        order_updates_.push_back(OrderUpdate(o.order.id_,0,0,o.order.price_,OrderState::FILLED));

        return  order_updates_.back();

    }



    void fill_levels(Order& o, DomIter& maker, const int direction)
    {

        const double initial_qty = o.qty_;

        while (o.qty_ >= maker->depth_)
        {
            o.price_ += idx(maker) * (maker->depth_/initial_qty);
            o.qty_ -= maker->depth_;
            maker->depth_=0;

            record_fills(maker);
            maker->clear();
            std::advance(maker,direction);

        }

        if (o.price_ == 0)
            o.price_  = idx(maker);
        else
            o.price_  += idx(maker) * (o.qty_/initial_qty);

    }

    OrderUpdate fill_orders_at_level(Order& o, const DomIter& maker)
    {
        maker->depth_ -= o.qty_;
        while (o.qty_ >= maker->front().qty_)
        {
            o.qty_ -= maker->front().qty_;

            record_fills(maker->front());

            maker->pop_front();
        }

        o.qty_=0;
        record_fills(maker->front());
        return record_fills(o);
    }


    OrderUpdate& record_order_update(const Order& o, OrderState state)
    {
        order_updates_.emplace_back(OrderUpdate(o.id_,o.qty_,o.qty_,o.price_,state));
        return order_updates_.back();
    }

    OrderUpdate& record_fills(const Order& o)
    {
        order_updates_.emplace_back(OrderUpdate(o.id_,o.qty_,o.qty_,o.price_,OrderState::FILLED));
        return order_updates_.back();
    }

    void record_fills(const DomIter& level)
    {
        for (const auto& o : level->limit_orders_)
            order_updates_.push_back(OrderUpdate(o.id_,o.qty_,o.qty_,o.price_,OrderState::FILLED));
    }



};



#endif