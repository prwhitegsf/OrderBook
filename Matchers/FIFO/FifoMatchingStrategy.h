#ifndef FIFO_H
#define FIFO_H

#include <ranges>
#include "../../PriceLevels/DequeLevel/DequeLevel.h"
using Level = DequeLevel;
using DomIter = std::vector<Level>::iterator;

class FifoMatchingStrategy
{

public:
    static constexpr short UP{1};
    static constexpr short DOWN{-1};


    size_t num_prices_;

    std::vector<Level> levels_;

    DomIter bid_{ levels_.begin() };
    DomIter ask_{ levels_.end() };

    DomIter level_{ levels_.begin() };


    std::vector<OrderUpdate> order_updates_;
    size_t ask_idx(){return std::distance(levels_.begin(), ask_);};
    size_t bid_idx(){return std::distance(levels_.begin(), bid_);};
    size_t lev_idx(){return std::distance(levels_.begin(), level_);};
    size_t idx(const DomIter ptr){return std::distance(levels_.begin(), ptr);};
    size_t num_prices() const {return num_prices_;};

    DomIter ladder_first() {return levels_.begin();};
    DomIter ladder_last() {return levels_.end();};

    void set_bid(const DomIter ptr){bid_ = ptr;};
    void set_ask(const DomIter ptr){ask_ = ptr;};

    friend void print_all_orders(FifoMatchingStrategy& fifo);

    FifoMatchingStrategy()=default;

    explicit FifoMatchingStrategy(const size_t num_prices)
        : num_prices_(num_prices),levels_(num_prices){}



    OrderUpdate match(BuyLimit&& o)
    {
        level_ = levels_.begin()+o.order.price_;
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

    OrderUpdate match(SellLimit&& o)
    {
        level_ = levels_.begin()+o.order.price_;
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


        order_updates_.push_back(OrderUpdate(o.id_,o.price_,o.qty_,OrderState::PARTIALLY_FILLED));

        return order_updates_.back();

    }


    OrderUpdate match(BuyMarket&& o)
    {
        fill_levels(o.order,ask_,UP);
        return fill_orders_at_level(o.order,ask_);
    }

    OrderUpdate match(SellMarket&& o)
    {
        fill_levels(o.order,bid_,DOWN);
        return fill_orders_at_level(o.order,bid_);
    }

    OrderUpdate match(Cancel&& o)
    {
        level_ = levels_.begin() + o.order.price_;

        auto&& cancelled = level_->remove_order(o.cancel_id_);

        order_updates_.push_back(OrderUpdate(cancelled.id_,o.order.price_,cancelled.qty_,OrderState::FILLED));

        order_updates_.push_back(OrderUpdate(o.order.id_,o.order.price_,0,OrderState::FILLED));

        return {0,0,0,OrderState::FILLED};

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
        while (o.qty_ >= maker->limit_orders_.front().qty_)
        {
            o.qty_ -= maker->limit_orders_.front().qty_;

            record_fills(maker->limit_orders_.front());

            maker->limit_orders_.pop_front();
        }

        o.qty_=0;
        record_fills(maker->limit_orders_.front());
        return record_fills(o);
    }


    OrderUpdate& record_order_update(const Order& o, OrderState state)
    {
        order_updates_.emplace_back(OrderUpdate(o.id_,o.price_,o.qty_,state));
        return order_updates_.back();
    }

    OrderUpdate& record_fills(const Order& o)
    {
        order_updates_.emplace_back(OrderUpdate(o.id_,o.price_,o.qty_,OrderState::FILLED));
        return order_updates_.back();
    }

    void record_fills(const DomIter& level)
    {
        for (const auto& order : level->limit_orders_)
            order_updates_.push_back(OrderUpdate(order.id_,order.price_,order.qty_,OrderState::FILLED));
    }



};



#endif