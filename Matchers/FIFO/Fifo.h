#ifndef FIFO_H
#define FIFO_H

//#include "../OrderTypes/ClientOrders/ClientOrders.h"
#include "../Orderbook/Orderbook.h"
#include "../PriceLadders/FixedSizeLadder/FixedSizeLadder.h"
#include <ranges>


template<typename PriceLadder>
class Fifo
{

    public:
        Fifo()=default;

    OrderUpdate match(BuyLimitOrder&& order, PriceLadder& dom)
    {
        // Add checks for lifting bid and crossing ask
        //std::cout<< "Buy Limit Triggered"<<std::endl;
        dom.level(dom.idx_from_price(order.price()))
            .append_limit_order(order.make_limit_order());

        dom.order_updates_.push_back(OrderUpdate(order.id(),order.price(),order.qty(),OrderState::PENDING));

        return dom.order_updates_.back();

    }

    OrderUpdate  match(SellLimitOrder&& order, PriceLadder& dom)
    {
        // Add checks for dropping ask and crossing bid

        dom.level(dom.idx_from_price(order.price()))
        .append_limit_order(order.make_limit_order());

        dom.order_updates_.push_back(OrderUpdate(order.id(),order.price(),order.qty(),OrderState::PENDING));
        return dom.order_updates_.back();

    }

     OrderUpdate  match(BuyStopOrder&& order, PriceLadder& dom)
    {
        dom.level(dom.idx_from_price(order.price()))
        .append_stop_order(order.make_stop_order());

        dom.order_updates_.push_back(OrderUpdate(order.id(),order.price(),order.qty(),OrderState::PENDING));

        return dom.order_updates_.back();
    }

    OrderUpdate  match(SellStopOrder&& order, PriceLadder& dom)
    {
        dom.level(dom.idx_from_price(order.price()))
        .append_stop_order(order.make_stop_order());

        dom.order_updates_.push_back(OrderUpdate(order.id(),order.price(),order.qty(),OrderState::PENDING));
        return dom.order_updates_.back();
    }

    OrderUpdate match(BuyMarketOrder&& market_order, PriceLadder& dom)
    {
        buy_market(market_order.make_market_order(), dom);

        for (auto& ord : dom.pending_market_orders_)
        {
            if (ord.qty_)
                buy_market(ord, dom);
        }

        return {market_order.id(),market_order.price(),market_order.qty(),OrderState::PENDING};

    }

    OrderUpdate match(SellMarketOrder&& market_order, PriceLadder& dom)
    {
        sell_market(market_order.make_market_order(), dom);

        for (auto& ord : dom.pending_market_orders_)
        {
            if (ord.qty_)
                sell_market(ord, dom);
        }
        return {market_order.id(),market_order.price(),market_order.qty(),OrderState::PENDING};
    }

    OrderUpdate match(CancelOrder&& order, PriceLadder& dom)
    {
        // cancel requested order and push
        auto&& [id, price] = order.cancel();
        auto&& cancelled = dom.level(dom.idx_from_price(price)).remove_order(id);
        dom.order_updates_.push_back(OrderUpdate(cancelled.id_,price,cancelled.qty_,OrderState::FILLED));

        // push this order which did the canceling
        dom.order_updates_.push_back(OrderUpdate(order.id(),order.price(),0,OrderState::FILLED));

        return dom.order_updates_.back();

    }


private:
    void buy_market(QueuedMarketOrder market_order, PriceLadder& dom)
    {
        fill_levels(market_order,dom,dom.ask_idx_,std::plus<>());

        fill_orders_at_level(dom.ask(),market_order,dom, dom.price_from_idx(dom.ask_idx_));

    }

    void sell_market(QueuedMarketOrder market_order, PriceLadder& dom)
    {
        fill_levels(market_order,dom,dom.bid_idx_,std::minus<>());

        fill_orders_at_level(dom.bid(),market_order,dom,dom.price_from_idx(dom.bid_idx_));

    }





    // Don't love that the fill price calc is coupled with the other stuff here
    template<class T>
    void fill_levels(QueuedMarketOrder& market_order, PriceLadder& dom, size_t& idx, T iterate_idx)
    {
        /*Checks order qty against total depth at level
         *  if there's more order qty than depth
         *  we take it in one fell swoop, record the limit orders,
         *  increment/decrement, then trigger the stops at the next level
         *
         *  Records avg fill price along the way and finishes the
         *  calculation after the loop
         */

        const double initial_qty = abs(market_order.qty_);

        while (abs(market_order.qty_) >= abs(dom.level(idx).depth()))
        {
            market_order.price_ += dom.price_from_idx(idx) * (abs(dom.level(idx).depth())/initial_qty);
            take_all_level_liquidity(market_order,dom.level(idx));

            record_filled_orders(dom.level(idx).limit_orders_, dom.order_updates_,dom.price_from_idx(idx));

            idx = iterate_idx(idx, 1);


            queue_triggered_stops(dom.level(idx).stop_orders_, dom.pending_market_orders_);
            dom.level(idx).reset_stop_orders();

        }

        market_order.price_  += dom.price_from_idx(idx) * (abs(market_order.qty_)/initial_qty);

        // finish setting fill price
        /*if (market_order.price_ == 0)
            market_order.price_  = dom.price_from_idx(idx);
        else
            market_order.price_  += dom.price_from_idx(idx) * (abs(dom.level(idx).depth())/initial_qty);*/
    }



// price parameter is temporary, want to move this over to pointer access
    void fill_orders_at_level(Level& level, QueuedMarketOrder& market_order, PriceLadder& dom,double price)
    {
        level.depth() += market_order.qty_;
        while (abs(market_order.qty_) >= abs(level.limit_orders_.front().qty_))
        {
            market_order.qty_ += level.limit_orders_.front().qty_;

            record_filled_orders(level.limit_orders_.front(),dom.order_updates_,price);
            level.fully_fill_next_order();
        }

        //level.limit_orders_.front().qty_ += market_order.qty_;
        record_filled_orders(level.limit_orders_.front(), dom.order_updates_,price);

        market_order.qty_=0;
        record_filled_orders(market_order, dom.order_updates_);

    }

    void take_all_level_liquidity (QueuedMarketOrder& market_order, Level& level)
    {
        market_order.qty_ += level.depth();
        level.zero_out_limit_orders();
        level.depth()=0;

    }

    template<typename R>
    requires (std::ranges::range<R>)
    void record_filled_orders(R& orders, auto& update_queue,double price)
    {

        for (auto& order : orders)
            update_queue.push_back(OrderUpdate(order.id_,price,order.qty_,OrderState::FILLED));


    }


    void record_filled_orders(auto& order, auto& update_queue)
    {
        update_queue.push_back(OrderUpdate(order.id_,order.price_,order.qty_,OrderState::FILLED));
    }

    /*template<typename T>
    requires (!std::ranges::range<T>)*/
    void record_filled_orders(auto& order, auto& update_queue, double price)
    {
        update_queue.push_back(OrderUpdate(order.id_,price,order.qty_,OrderState::FILLED));
    }

    void queue_triggered_stops(auto& triggered_stops, auto& market_order_queue)
    {
        market_order_queue.insert(market_order_queue.end(),triggered_stops.begin(),triggered_stops.end());
    }
};



#endif