#ifndef FIFO_H
#define FIFO_H

#include "../../PriceLevels/LevelRequirements.h"


template<typename DOM>
class FifoMatchingStrategy
{

    static constexpr short UP{1};
    static constexpr short DOWN{-1};

    std::vector<OrderUpdate> order_updates_;

public:

    template<class T>
    int idx_from_dom(T ladder, size_t idx)
    {
        return ladder.num_prices_;
    }



    const std::vector<OrderUpdate>& order_updates() {return order_updates_;};
    void clear_updates(){ order_updates_.clear();};

    FifoMatchingStrategy()=default;



    // Supported order matching
    OrderUpdate match(BuyLimit<Order>&& o, DOM& dom)
    {
        dom.limit_ = dom.dom_.begin()+o.order.price_;

        if (dom.limit_ < dom.ask_)
        {
            if (dom.limit_ > dom.bid_)
                dom.bid_ = dom.limit_;

            dom.limit_->append_new(o.order);
            return record_order_update(o.order,OrderState::PENDING,o.order.price_);

        }

        return crossing_spread(std::move(o.order),dom,
            dom.limit_,dom.ask_,dom.bid_,
            UP,std::less_equal<>());
    }

    OrderUpdate match(SellLimit<Order>&& o, DOM& dom)
    {
        dom.limit_ = dom.dom_.begin()+o.order.price_;
        if (dom.limit_ > dom.bid_) {

            if (dom.limit_ < dom.ask_)
                dom.ask_ = dom.limit_;

            dom.limit_->append_new(o.order);
            return record_order_update(o.order,OrderState::PENDING,o.order.price_);
        }

        return crossing_spread(std::move(o.order),dom,
            dom.limit_,dom.bid_,dom.ask_,
            DOWN,std::greater_equal<>());
    }

    OrderUpdate match(BuyMarket<Order>&& o, DOM& dom)
    {
        return fill_orders_at_level(o.order,dom,dom.ask_,fill_levels(o.order,dom,dom.ask_,UP));
    }

    OrderUpdate match(SellMarket<Order>&& o, DOM& dom)
    {
        return fill_orders_at_level(o.order,dom,dom.bid_,fill_levels(o.order,dom,dom.bid_,DOWN));
    }

    OrderUpdate match(Cancel<Order>&& o, DOM& dom)
    {
        dom.limit_ = dom.dom_.begin() + o.order.price_;
        auto&& cancelled = dom.limit_->remove(o.cancel_id_);

        order_updates_.push_back(OrderUpdate(cancelled.id_,cancelled.qty_,o.order.price_,OrderState::CANCELLED,0));
        order_updates_.push_back(OrderUpdate(o.order.id_,0,o.order.price_,OrderState::FILLED,0));

        return  order_updates_.back();

    }

    // Aggressing Orders
    float fill_levels(Order& o, DOM& dom, auto& maker, const int direction)
    {
        const double initial_qty = o.qty_;
        float fill{};
        while (o.qty_ >= maker->depth_)
        {

            fill += dom.idx(maker) * (maker->depth_/initial_qty);
            o.qty_ -= maker->depth_;
            maker->depth_=0;
            record_fills(dom,dom.idx(maker));
            //record_fills(maker,dom.idx(maker));
            //record_fills(maker,dom.idx(maker));
            maker->clear();

            std::advance(maker,direction);
        }

        if (fill == 0)
            fill = dom.idx(maker);
        else
            fill += dom.idx(maker) * (o.qty_/initial_qty);

        return fill;
    }

    OrderUpdate fill_orders_at_level(Order& o, DOM& dom, const auto& maker,const float fill)
    {
        maker->depth_ -= o.qty_;
        while (o.qty_ >= maker->front().qty_)
        {
            o.qty_ -= maker->front().qty_;
            maker->front().qty_=0;

            record_fills(maker->front(),dom.idx(maker));

            maker->pop_front();
        }

        o.qty_ = 0;
        record_fills(maker->front(),dom.idx(maker));

        return record_fills(o,fill);
    }

    // needs a requirement
    template<typename U>
    OrderUpdate crossing_spread(Order&& o,DOM& dom,auto& limit, auto& maker,auto& taker, const int direction, U cmpr)
    {
        const double initial_qty = o.qty_;
        float fill{};

        while (cmpr(maker, limit ) && o.qty_ >= maker->depth_)
        {
            fill += dom.idx(maker) * (maker->depth_/initial_qty);
            o.qty_ -= maker->depth_;
            maker->depth_ = 0;
            record_fills(dom,dom.idx(maker));
            //record_fills(maker,dom.idx(maker));
            maker->clear();

            taker = maker;
            std::advance(maker,direction);
        }

        if (fill == 0)
            fill = dom.idx(maker);
        else
            fill += dom.idx(taker) * (o.qty_/initial_qty);
        maker->depth_ -= o.qty_;

        if (o.qty_)
            taker->append_new(o);

        return record_order_update(o,OrderState::PARTIALLY_FILLED,fill);
    }


    // Order Logging

    OrderUpdate record_order_update(Order o, const OrderState state, const float fill)
    {
        order_updates_.push_back(OrderUpdate(o.id_,o.qty_,o.price_,state,fill));
        return order_updates_.back();
    }

    OrderUpdate record_fills(Order o,const float fill)
    {
        order_updates_.push_back(OrderUpdate(o.id_,o.qty_,o.price_,OrderState::FILLED,fill));
        return order_updates_.back();
    }

    void record_fills(const DOM& dom, const float fill)
    {
        /*auto o = dom.orders()->begin();

        while (o != dom.orders()->end()) {
            order_updates_.push_back(OrderUpdate(o->id_,o->qty_,o->price_,OrderState::FILLED,fill));
            ++o;
        }*/
    }

};


#endif