//
// Created by prw on 4/23/25.
//

#ifndef ORDERGENERATOR_H
#define ORDERGENERATOR_H

#include "../OrderTypes/SubmittedOrderTypes.h"

namespace gen {

// Bypass Queue, Write directly to OB

    template<typename Ord, typename Book> // Bypasses order queue
    OrderUpdate CreateOrder(Ord submitted_order,Book& ob)
    {
        ob.instrument_->client_order_list_.append_order(submitted_order);
        return ob.SubmitOrder(submitted_order.make_queued_order());
    }

    template<typename Book>
    void GenerateOrderBook(const size_t bid, const size_t ask,
        Book& ob,const std::vector<int>& depths)
    {
        ob.set_bid(bid);
        ob.set_ask(ask);

        size_t i = bid;
        for(const int depth : depths)
        {
            int j{depth};
            while (j > 0)
            {
                SubmittedBuyLimit<Order> buy(1,i,Duration::DAY);
                ob.instrument_->client_order_list_.append_order(buy);
                ob.SubmitOrder(buy.make_queued_order());
                --j;
            }
            --i;
        }

        i = ask;
        for (const int depth : depths)
        {
            int j{depth};
            while (j > 0) {
                SubmittedSellLimit<Order>sell(1,i,Duration::DAY);
                ob.instrument_->client_order_list_.append_order(sell);
                ob.SubmitOrder(sell.make_queued_order());
                --j;
            }

            ++i;
        }

    }





} // generators

#endif //ORDERGENERATOR_H
