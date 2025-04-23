#ifndef FIFO_H
#define FIFO_H

#include <ranges>
#include "../../PriceLevels/DequeLevel/DequeLevel.h"
using Level = DequeLevel;
using DomIter = std::vector<Level>::iterator;
template<typename PriceLadder>
class FifoMatchingStrategy
{
public:
    size_t num_prices_;

    std::vector<Level> levels_;

    DomIter bid_{ levels_.begin() };
    DomIter ask_{ levels_.end() };

    DomIter level_{ levels_.begin() };

    std::vector<OrderUpdate> order_updates_;
    size_t ask_idx_{};
    size_t bid_idx_{};
    size_t lev_idx_{};


    FifoMatchingStrategy()=default;

    explicit FifoMatchingStrategy(const size_t num_prices)
        : num_prices_(num_prices),levels_(num_prices){}

    Level& level(size_t idx)
    {

        return levels_[idx];
    }


    OrderUpdate match(BuyLimit&& order, PriceLadder& dom)
    {

        level_ = levels_.begin()+order._.price_;
        // Not crossing the spread
        if (level_ < ask_)
        {   // lifting the bid
            if (level_ > bid_)
                bid_ = level_;

            level_->append_limit_order(order._);
            order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,order._.qty_,OrderState::PENDING));
            return order_updates_.back();

        }
        order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,order._.qty_,OrderState::PENDING));
        return order_updates_.back();

        //return crossing_spread(std::move(order._),dom,dom.ask_idx_,dom.bid_idx_,std::plus<>(),std::less_equal<>());
    }
    /*OrderUpdate match(BuyLimit&& order, PriceLadder& dom)
    {

        level_ = levels_.begin()+order._.price_;
        // Not crossing the spread
        if (order._.price_ < ask_idx_)
        {   // lifting the bid
            if (order._.price_ > bid_idx_)
                bid_idx_ = order._.price_;

            levels_.at(order._.price_).append_limit_order(order._);
            order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,order._.qty_,OrderState::PENDING));
            return order_updates_.back();

        }
        order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,order._.qty_,OrderState::PENDING));
        return order_updates_.back();

        //return crossing_spread(std::move(order._),dom,dom.ask_idx_,dom.bid_idx_,std::plus<>(),std::less_equal<>());
    }*/

    OrderUpdate match(SellLimit&& order, PriceLadder& dom)
    {
        level_ = levels_.begin()+order._.price_;
        if (level_ > bid_)
        {   // lifting the bid
            if (level_ < ask_)
                ask_ = level_;

            level_->append_limit_order(order._);
            order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,order._.qty_,OrderState::PENDING));
            return order_updates_.back();

        }
        order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,order._.qty_,OrderState::PENDING));
        return order_updates_.back();
        /*
        if (order._.price_ > dom.price_from_idx(dom.bid_idx_))
        {

            dom.level(dom.idx_from_price(order._.price_))
                         .append_limit_order(order._);


            order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,order._.qty_,OrderState::PENDING));

            if (order._.price_ < dom.price_from_idx(dom.ask_idx_))
                dom.ask_idx_ = dom.idx_from_price(order._.price_);

            return order_updates_.back();



        }
        std::cout << "Crossing Spread! "<<std::endl;
        return crossing_spread(std::move(order._),dom,dom.bid_idx_,dom.ask_idx_,std::minus<>(),std::greater_equal<>());
        */

    }


    /*OrderUpdate match(SellLimit&& order, PriceLadder& dom)
    {

        if (order._.price_ > bid_idx_)
        {   // lifting the bid
            if (order._.price_ < ask_idx_)
                ask_idx_ = order._.price_;

            levels_.at(order._.price_).append_limit_order(order._);
            order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,order._.qty_,OrderState::PENDING));
            return order_updates_.back();

        }
        order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,order._.qty_,OrderState::PENDING));
        return order_updates_.back();
        /*
        if (order._.price_ > dom.price_from_idx(dom.bid_idx_))
        {

            dom.level(dom.idx_from_price(order._.price_))
                         .append_limit_order(order._);


            order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,order._.qty_,OrderState::PENDING));

            if (order._.price_ < dom.price_from_idx(dom.ask_idx_))
                dom.ask_idx_ = dom.idx_from_price(order._.price_);

            return order_updates_.back();



        }
        std::cout << "Crossing Spread! "<<std::endl;
        return crossing_spread(std::move(order._),dom,dom.bid_idx_,dom.ask_idx_,std::minus<>(),std::greater_equal<>());
        #1#

    }*/

    OrderUpdate match(BuyMarket&& order, PriceLadder& dom)
    {
        std::cout <<"Ptr to depth in function: " << dom.ask_->depth_ << std::endl;
        fill_levels(order._,dom,dom.ask_idx_,std::plus<>());
        fill_orders_at_level(dom.ask(),order._,dom);
        std::cout <<"Ptr to depth in function: " << dom.ask_->depth_ << std::endl;


        return {order._.id_,order._.price_,order._.qty_,OrderState::FILLED};


    }

    OrderUpdate match(SellMarket&& order, PriceLadder& dom)
    {

        fill_levels(order._,dom,dom.bid_idx_,std::minus<>());
        fill_orders_at_level(dom.bid(),order._,dom);

        return {order._.id_,order._.price_,order._.qty_,OrderState::FILLED};

    }

    OrderUpdate match(Cancel&& order, PriceLadder& dom)
    {
        ID id(order.cancel_id_);
        double price = order._.price_;

        auto&& cancelled = dom.level(dom.idx_from_price(price)).remove_order(id);
        order_updates_.push_back(OrderUpdate(cancelled.id_,price,cancelled.qty_,OrderState::FILLED));

        order_updates_.push_back(OrderUpdate(order._.id_,order._.price_,0,OrderState::FILLED));


        return {0,0,0,OrderState::FILLED};

    }


    template<class T, class U>
    OrderUpdate crossing_spread(Order&& order, PriceLadder& dom, size_t& maker_idx,size_t& taker_idx, T iterate_idx, U cmpr)
    {


        const double initial_qty = order.qty_;

        size_t order_price_idx = dom.idx_from_price(order.price_) ;
        order.price_ = 0;
        //std::cout << "idx: "<<idx<<" Order Price Idx: "<<order_price_idx<<std::endl;
        while (cmpr(maker_idx, order_price_idx ) && order.qty_ >= dom.level(maker_idx).depth_)
        {

            order.price_ += dom.price_from_idx(maker_idx) * (dom.level(maker_idx).depth_/initial_qty);
            order.qty_ -= dom.level(maker_idx).depth_;

            dom.level(maker_idx).depth_ = 0;
            record_filled_orders(dom.level(maker_idx).limit_orders_);
            dom.level(maker_idx).clear();


            taker_idx= maker_idx;
            maker_idx = iterate_idx(maker_idx, 1);
            std::cout << "idx: "<<maker_idx<<" Price: "<<dom.price_from_idx(maker_idx)<<std::endl;
        }
        // This is where it should eventually fill...not great though
        order.price_  += dom.price_from_idx(taker_idx) * (order.qty_/initial_qty);
        if (order.qty_)
            dom.level(order_price_idx).append_limit_order(order);

        order_updates_.push_back(OrderUpdate(order.id_,order.price_,order.qty_,OrderState::PARTIALLY_FILLED));

        return order_updates_.back();

    }



    // Don't love that the fill price calc is coupled with the other stuff here
    template<class T>
    void fill_levels(Order& order, PriceLadder& dom, size_t& idx, T iterate_idx)
    {
        /*Checks order qty against total depth at level
         *  if there's more order qty than depth
         *  we take it in one fell swoop, record the limit orders,
         *  increment/decrement, then trigger the stops at the next level
         *
         *  Records avg fill price along the way and finishes the
         *  calculation after the loop
         */

        const double initial_qty = order.qty_;

        while (order.qty_ >= dom.level(idx).depth_)
        {
            order.price_ += dom.price_from_idx(idx) * (dom.level(idx).depth_/initial_qty);
           // take_all_level_liquidity(order,dom.level(idx));
            order.qty_ -= dom.level(idx).depth_;

            dom.level(idx).depth_=0;
            record_filled_orders(dom.level(idx).limit_orders_);
            dom.level(idx).clear();
            idx = iterate_idx(idx, 1);
        }

        if (order.price_ == 0)
            order.price_  = dom.price_from_idx(idx);
        else
            order.price_  += dom.price_from_idx(idx) * (order.qty_/initial_qty);

    }



// price parameter is temporary, want to move this over to pointer access
    void fill_orders_at_level(Level& level, Order& order, PriceLadder& dom)
    {

        level.depth_ -= order.qty_;
        while (order.qty_ >= level.limit_orders_.front().qty_)
        {
            order.qty_ -= level.limit_orders_.front().qty_;


            record_filled_order(level.limit_orders_.front());
            level.fully_fill_next_order();
        }

        order.qty_=0;

        record_filled_order(level.limit_orders_.front());
        record_filled_order(order);


    }

    void take_all_level_liquidity (Order& order, Level& level)
    {
        order.qty_ -= level.depth_;
        level.clear();
        level.depth_=0;

    }

    template<typename Ords>
    void record_filled_orders(Ords& orders)
    {

        for (auto& order : orders)
            order_updates_.push_back(OrderUpdate(order.id_,order.price_,order.qty_,OrderState::FILLED));


    }


    void record_filled_order(const Order& order)
    {
        order_updates_.push_back(OrderUpdate(order.id_,order.price_,order.qty_,OrderState::FILLED));

    }



    void queue_triggered_stops(auto& triggered_stops, auto& market_order_queue)
    {
        market_order_queue.insert(market_order_queue.end(),triggered_stops.begin(),triggered_stops.end());
    }
};



#endif