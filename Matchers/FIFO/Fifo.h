#ifndef FIFO_H
#define FIFO_H

//#include "../OrderTypes/ClientOrders/ClientOrders.h"
#include "../Orderbook/Orderbook.h"
#include "../PriceLadders/FixedSizeLadder/FixedSizeLadder.h"



template<typename PriceLadder>
class Fifo
{

    public:
        Fifo()=default;

    void match(BuyLimitOrder&& order, PriceLadder& dom)
    {
        // Add checks for lifting bid and crossing ask
        //std::cout<< "Buy Limit Triggered"<<std::endl;
        dom.level(dom.idx_from_price(order.price()))
            .append_limit_order(order.make_limit_order());

        // Update State in Client Order List

    }

    void match(SellLimitOrder&& order, PriceLadder& dom)
    {
        // Add checks for dropping ask and crossing bid

            dom.level(dom.idx_from_price(order.price()))
            .append_limit_order(order.make_limit_order());

        //std::cout << "Sell ID: "<< order.id() << std::endl;
        // Update State in Client Order List

    }

    void match(BuyStopOrder&& order, PriceLadder& dom)
    {

        dom.level(dom.idx_from_price(order.price()))
        .append_stop_order(order.make_stop_order());

    }

    void match(SellStopOrder&& order, PriceLadder& dom)
    {
        dom.level(dom.idx_from_price(order.price()))
        .append_stop_order(order.make_stop_order());
    }

    void match(BuyMarketOrder&& market_order, PriceLadder& dom)
    {
        buy_market(market_order.make_market_order(), dom);

        for (auto& ord : dom.pending_market_orders_)
        {
            if (ord.qty_)
                buy_market(ord, dom);
        }

    }

    void match(SellMarketOrder&& market_order, PriceLadder& dom)
    {
        sell_market(market_order.make_market_order(), dom);

        for (auto& ord : dom.pending_market_orders_)
        {
            if (ord.qty_)
                sell_market(ord, dom);
        }
    }

    void match(CancelOrders& order, Orderbook<Fifo,PriceLadder>& ob)
    {

    }


private:
    void buy_market(QueuedMarketOrder market_order, PriceLadder& dom)
    {
        fill_levels(market_order,dom,dom.ask_idx_,std::plus<size_t>());

        fill_orders_at_level(dom.ask(),market_order,dom);

    }

    void sell_market(QueuedMarketOrder market_order, PriceLadder& dom)
    {
        fill_levels(market_order,dom,dom.bid_idx_,std::minus<size_t>());

        fill_orders_at_level(dom.bid(),market_order,dom);

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

        const double initial_qty = market_order.qty_;

        while (market_order.qty_ >= dom.level(idx).depth()*-1)
        {
            market_order.avg_fill_price_ += dom.price_from_idx(idx) * dom.level(idx).depth()/initial_qty;
            take_all_level_liquidity(market_order,dom.level(idx));

            record_filled_limit_orders(dom.level(idx).limit_orders_, dom.order_updates_);

            idx = iterate_idx(idx, 1);


            queue_triggered_stops(dom.level(idx).stop_orders_, dom.pending_market_orders_);
            dom.level(idx).reset_stop_orders();

        }

        // finish setting fill price
        if (market_order.avg_fill_price_ == 0)
            market_order.avg_fill_price_  = dom.price_from_idx(dom.ask_idx_);
        else
            market_order.avg_fill_price_  += dom.price_from_idx(dom.ask_idx_) * dom.ask().depth()/initial_qty;
    }




    void fill_orders_at_level(Level& level, QueuedMarketOrder& market_order, PriceLadder& dom)
    {
        level.depth() += market_order.qty_;
        while (market_order.qty_ >= level.limit_orders_.front().qty_*-1)
        {

            market_order.qty_ += level.limit_orders_.front().qty_;

            dom.order_updates_.push_back(level.limit_orders_.front());
            level.fully_fill_next_order();
        }

        level.limit_orders_.front().qty_ += market_order.qty_;
        // push partial fill
        market_order.qty_=0;
        // push filled market order
    }

    void take_all_level_liquidity (QueuedMarketOrder& market_order, Level& level)
    {
        market_order.qty_ += level.depth();
        level.zero_out_limit_orders();
        level.depth()=0;

    }

    void record_filled_limit_orders(auto& orders, auto& update_queue)
    {
        update_queue.insert(update_queue.end(), orders.begin(), orders.end());

    }

    void queue_triggered_stops(auto& triggered_stops, auto& market_order_queue)
    {
        market_order_queue.insert(market_order_queue.end(),triggered_stops.begin(),triggered_stops.end());
    }
};



#endif