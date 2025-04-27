#ifndef FIFO_H
#define FIFO_H


#include "../../DOMS/DomRequirements.h"

template<Is_Dom DOM>
class FifoStrategy
{

    static constexpr short UP{1};
    static constexpr short DOWN{-1};

    std::vector<OrderUpdate> order_updates_;

public:


    const std::vector<OrderUpdate>& order_updates() {return order_updates_;};
    void clear_updates(){ order_updates_.clear();};

    FifoStrategy()=default;



    // Supported order matching
    OrderUpdate match(BuyLimit<Order>&& o, DOM& dom)
    {
        dom.limit_ = dom.dom_.begin()+o.order.price_;

        if (dom.limit_ < dom.ask_)
        {
            if (dom.limit_ > dom.bid_)
                dom.bid_ = dom.limit_;

            dom.limit_->append(o.order);
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

            dom.limit_->append(o.order);
            return record_order_update(o.order,OrderState::PENDING,o.order.price_);
        }

        return crossing_spread(std::move(o.order),dom,
            dom.limit_,dom.bid_,dom.ask_,
            DOWN,std::greater_equal<>());
    }

    OrderUpdate match(BuyMarket<Order>&& o, DOM& dom)
    {
        return execute_market(o.order,dom,dom.ask_,UP);

    }

    OrderUpdate match(SellMarket<Order>&& o, DOM& dom)
    {
        return execute_market(o.order,dom,dom.bid_,DOWN);

    }

    OrderUpdate match(Cancel<Order>&& o, DOM& dom)
    {
        dom.limit_ = dom.dom_.begin() + o.order.price_;
        auto&& cancelled = dom.limit_->remove(o.cancel_id_);

        order_updates_.push_back(OrderUpdate(cancelled.id_,cancelled.qty_,o.order.price_,OrderState::CANCELLED,0));
        order_updates_.push_back(OrderUpdate(o.order.id_,0,o.order.price_,OrderState::FILLED,0));

        return  order_updates_.back();

    }

    OrderUpdate execute_market(Order& o, DOM& dom, auto& maker, const int direction)
    {
        const double initial_qty = o.qty_;
        float avg_fill{};

        // Market order takes all the available liquidity at current the level at once
        while (o.qty_ >= maker->depth_)
        {
            // updating aggressing order
            avg_fill += dom.idx(maker) * (maker->depth_/initial_qty);
            o.qty_ -= maker->depth_;
            // updating resting orders
            maker->depth_=0;
            record_fills(maker,dom.idx(maker));
            maker->clear();
            // next price
            std::advance(maker,direction);
        }

        // Any remaining orders are executed at the current maker price
        avg_fill += avg_fill == 0 ? dom.idx(maker) : dom.idx(maker) * (o.qty_/initial_qty);
        maker->depth_ -= o.qty_;

        // Market order takes the liquidity at this level order by order
        while (o.qty_ >= maker->front().qty_)
        {
            o.qty_ -= maker->front().qty_;
            maker->front().qty_=0;

            record_fills(maker->front(),dom.idx(maker));

            maker->pop_front();
        }

        // Partial
        if (o.qty_){
            maker->front().qty_ -= o.qty_;
            o.qty_ = 0;
            record_fills(maker->front(),dom.idx(maker));
        }

        // push and return the market order
        return record_fills(o,avg_fill);
    }


    // needs a requirement
    template<typename U>
    OrderUpdate crossing_spread(Order&& o,DOM& dom,auto& limit, auto& maker,auto& taker, const int direction, U cmpr)
    {
        const double initial_qty = o.qty_;
        float avg_fill{};

        while (cmpr(maker, limit ) && o.qty_ >= maker->depth_)
        {

            // updating aggressing order
            avg_fill += dom.idx(maker) * (maker->depth_/initial_qty);
            o.qty_ -= maker->depth_;

            // updating resting orders
            maker->depth_ = 0;
            record_fills(maker,dom.idx(maker));
            maker->clear();

            // next price, both sides move

            std::advance(maker,direction);
        }

        // Occurs when remaining order qty < maker depth, but we're still below the limit price
        // So we fill the remainder at market
        if (o.qty_ < maker->depth_ && cmpr(maker, limit))
        {
            avg_fill += avg_fill == 0 ? dom.idx(maker) : dom.idx(maker) * (o.qty_/initial_qty);
            maker->depth_ -= o.qty_;

            while (o.qty_ >= maker->front().qty_)
            {

                o.qty_ -= maker->front().qty_;
                maker->front().qty_=0;

                record_fills(maker->front(),dom.idx(maker));

                maker->pop_front();
            }
        }
        // Occurs when remaining qty is not zero, but we're at the limit price
        else if (o.qty_ != 0)
        {
            taker = maker-direction;
            avg_fill += avg_fill == 0 ? dom.idx(taker) : dom.idx(taker) * (o.qty_/initial_qty);
            taker->append(o);

        }

        return record_order_update(o,OrderState::PARTIALLY_FILLED,avg_fill);
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



    void record_fills(auto& maker,const int idx)
    {
        auto o = maker->limit_orders_.begin();

        while (o != maker->limit_orders_.end()) {
            order_updates_.push_back(OrderUpdate(o->id_,o->qty_,o->price_,OrderState::FILLED,idx));
            ++o;
        }
    }

};


#endif