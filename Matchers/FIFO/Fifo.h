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

    void match(BuyLimitOrder& order, Orderbook<Fifo,PriceLadder>& ob)
    {
        // Add checks for lifting bid and crossing ask
        ob.price_ladder_
            .get_level(ob.get_index_from_price(order.price()))
            .append_limit_order(order.make_limit_order());

        // Update State in Client Order List

    }

    void match(SellLimitOrder& order, Orderbook<Fifo,PriceLadder>& ob)
    {
        // Add checks for dropping ask and crossing bid
        ob.price_ladder_
            .get_level(ob.get_index_from_price(order.price()))
            .append_limit_order(order.make_limit_order());

        // Update State in Client Order List

    }

    void match(BuyStopOrder& order, Orderbook<Fifo,PriceLadder>& ob)
    {

    }

    void match(SellStopOrder& order, Orderbook<Fifo,PriceLadder>& ob)
    {

    }

    void match(BuyMarketOrder& market_order, Orderbook<Fifo,PriceLadder>& ob)
    {

        /*const double initial_qty = market_order.qty();
        while (market_order.qty() >= abs(ob.ask.depth()))
        {
            market_order.update_qty(market_order.qty() + ob.ask.depth());
            market_order.update_fill_price(market_order.fill_price() + (ob.get_price_from_index(ob.ask()) * ob.ask.depth()/initial_qty));
            ob.ask.update_depth(0);
            ob.ask.orders().clear_level_quantities();


            ++ob.ask;
        }

        if (market_order.fill_price_ == 0)
            market_order.fill_price_  = ob.get_price_from_index(ob.ask());
        else
            market_order.fill_price_  += ob.get_price_from_index(ob.ask()) * ob.ask.depth()/initial_qty;

        ob.ask.update_depth(ob.ask.depth() + market_order.qty_);
        fill_remaining(ob.ask.orders(), market_order);*/
    }

    void match(SellMarketOrder& order, Orderbook<Fifo,PriceLadder>& ob)
    {

    }

    void match(CancelOrders& order, Orderbook<Fifo,PriceLadder>& ob)
    {

    }
};



#endif