//
// Created by prw on 9/3/25.
//

#ifndef ORDERBOOK_BENCHMARKS_H
#define ORDERBOOK_BENCHMARKS_H

#include "Instrument.h"
#include "Fifo.h"
#include "OrderBook.h"
#include "Record.h"
#include "OrderGenerator.h"

namespace bench
{
    inline std::queue<order::Submitted> make_order_sequence(const Instrument& instrument, size_t length)
    {
        while (true)
        {
            OrderBook<Fifo> ob(instrument);
            RecordDepot<order::Record> rd;
            gen::OrderGenerator order_gen;

            order_gen.initialize_orderbook(ob,rd,1000,10);

            int made = length;
            while (made--)
            {
                ob.submit_order(
                         order_gen.record_order(
                             order_gen.make_random_order(ob,rd,100)));

                ob.evaluate_orders();
                ob.match_orders();
                rd.record_matched_orders(ob.get_matched_orders());
                rd.update_order_records();

                if (ob.bid() <= ob.min_price() || ob.ask() >= ob.max_price())
                {
                    break;
                }
            }

            if (made <= 0)
                return std::move(order_gen.update_recorded_cancel_order_prices(rd));
        }
    }


    void cache_warmup(auto& ob, auto& orders, size_t count)
    {
        while (count-- && !orders.empty())
        {
            ob.submit_order(orders.front());
            ob.evaluate_orders();
            ob.match_orders();
            orders.pop();
        }
    }



}


#endif //ORDERBOOK_BENCHMARKS_H