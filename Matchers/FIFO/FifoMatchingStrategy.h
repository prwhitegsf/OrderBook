#ifndef FIFO_H
#define FIFO_H

#include "../../PriceLevels/LevelRequirements.h"


template<typename Order, template<Is_Level>typename Lev>
class FifoMatchingStrategy
{

    using Level = Lev<Order>;
    using Dom = std::vector<Level>;
    using DomIter = typename std::vector<Level>::iterator;


    static constexpr short UP{1};
    static constexpr short DOWN{-1};


    Dom dom_;
    size_t num_prices_;

    DomIter bid_{ dom_.begin() };
    DomIter ask_{ dom_.end() };
    DomIter level_{ dom_.begin() };

    std::vector<OrderUpdate> order_updates_;

public:
    const Dom& dom() {return dom_;};

    DomIter begin() {return dom_.begin();};
    DomIter end() {return dom_.end();};

    void set_bid(const size_t idx){bid_ = begin()+idx;};
    void set_ask(const size_t idx){ask_ = begin()+idx;};

    size_t ask_idx(){return std::distance(dom_.begin(), ask_);};
    size_t bid_idx(){return std::distance(dom_.begin(), bid_);};
    size_t idx(const DomIter ptr){return std::distance(dom_.begin(), ptr);};

    size_t num_prices() const {return num_prices_;};

    const std::vector<OrderUpdate>& order_updates() {return order_updates_;};
    void clear_updates(){ order_updates_.clear();};

    FifoMatchingStrategy()
    : dom_(100),num_prices_(dom.size()){}

    explicit FifoMatchingStrategy(const size_t num_prices)
        : dom_(num_prices), num_prices_(dom.size()){}


    // Supported order matching
    OrderUpdate match(BuyLimit<Order>&& o)
    {
        level_ = dom_.begin()+o.order.price_;

        if (level_ < ask_)
        {
            if (level_ > bid_)
                bid_ = level_;

            level_->append_new(o.order);
            return record_order_update(o.order,OrderState::PENDING,o.order.price_);

        }

        return crossing_spread(std::move(o.order),
            ask_,bid_,UP,
            std::less_equal<>());
    }

    OrderUpdate match(SellLimit<Order>&& o)
    {
        level_ = dom_.begin()+o.order.price_;
        if (level_ > bid_) {

            if (level_ < ask_)
                ask_ = level_;

            level_->append_new(o.order);
            return record_order_update(o.order,OrderState::PENDING,o.order.price_);
        }

        return crossing_spread(std::move(o.order),
            bid_,ask_,DOWN,
            std::greater_equal<>());
    }

    OrderUpdate match(BuyMarket<Order>&& o)
    {
        return fill_orders_at_level(o.order,ask_,fill_levels(o.order,ask_,UP));
    }

    OrderUpdate match(SellMarket<Order>&& o)
    {
        return fill_orders_at_level(o.order,bid_,fill_levels(o.order,bid_,DOWN));
    }

    OrderUpdate match(Cancel<Order>&& o)
    {
        level_ = begin() + o.order.price_;
        auto&& cancelled = level_->remove(o.cancel_id_);

        order_updates_.push_back(OrderUpdate(cancelled.id_,cancelled.qty_,o.order.price_,OrderState::CANCELLED,0));
        order_updates_.push_back(OrderUpdate(o.order.id_,0,o.order.price_,OrderState::FILLED,0));

        return  order_updates_.back();

    }

    // Aggressing Orders

    float fill_levels(Order& o, DomIter& maker, const int direction)
    {
        const double initial_qty = o.qty_;
        float fill{};
        while (o.qty_ >= maker->depth_)
        {

            fill += idx(maker) * (maker->depth_/initial_qty);
            o.qty_ -= maker->depth_;
            maker->depth_=0;

            record_fills(maker,idx(maker));
            maker->clear();

            std::advance(maker,direction);
        }

        if (fill == 0)
            fill = idx(maker);
        else
            fill += idx(maker) * (o.qty_/initial_qty);

        return fill;
    }

    OrderUpdate fill_orders_at_level(Order& o, const DomIter& maker,float fill)
    {
        maker->depth_ -= o.qty_;
        while (o.qty_ >= maker->front().qty_)
        {
            o.qty_ -= maker->front().qty_;
            maker->front().qty_=0;

            record_fills(maker->front(),idx(maker));

            maker->pop_front();
        }

        o.qty_ = 0;
        record_fills(maker->front(),idx(maker));

        return record_fills(o,fill);
    }

    template<class U>
    OrderUpdate crossing_spread(Order&& o,DomIter& maker,DomIter& taker, const int direction, U cmpr)
    {
        const double initial_qty = o.qty_;
        float fill{};

        while (cmpr(maker, level_ ) && o.qty_ >= maker->depth_)
        {

            fill += idx(maker) * (maker->depth_/initial_qty);
            o.qty_ -= maker->depth_;
            maker->depth_ = 0;

            record_fills(maker,idx(maker));
            maker->clear();

            taker = maker;
            std::advance(maker,direction);
        }

        if (fill == 0)
            fill = idx(maker);
        else
            fill += idx(taker) * (o.qty_/initial_qty);
        maker->depth_ -= o.qty_;

        if (o.qty_)
            taker->append_new(o);

        return record_order_update(o,OrderState::PARTIALLY_FILLED,fill);

    }


    // Order Logging

    OrderUpdate record_order_update(Order o, OrderState state, float fill)
    {

        order_updates_.push_back(OrderUpdate(o.id_,o.qty_,o.price_,state,fill));

        return order_updates_.back();
    }

    OrderUpdate record_fills(Order o,float fill)
    {

        order_updates_.push_back(OrderUpdate(o.id_,o.qty_,o.price_,OrderState::FILLED,fill));
        return order_updates_.back();
    }

    void record_fills(const DomIter& level, float fill)
    {
        auto o = level->begin();

        while (o != level->end()) {
            order_updates_.push_back(OrderUpdate(o->id_,o->qty_,o->price_,OrderState::FILLED,fill));
            ++o;
        }

    }



};

/*    if (maker->front().qty_)
            {
                if (maker->front().qty_ < maker->front().qty_)
                    maker->front().qty_ = maker->front().qty_;

                record_order_update(maker->front(),OrderState::PARTIALLY_FILLED);
                maker->copy_to_back(maker->front());
                maker->pop_front();
            }
*/

#endif