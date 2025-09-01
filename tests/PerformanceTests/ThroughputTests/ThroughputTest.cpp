//
// Created by prw on 9/1/25.
//
#include <chrono>

#include "Instrument.h"
#include "Fifo.h"
#include "OrderBook.h"
#include "Record.h"
#include "OrderGenerator.h"



int main()
{

    // Initialize
    // Run 100,000 sim trades and record
    // Warm the cache: run the first 1000 trades
    // then fall into timed window that counts trades
    size_t iterations{100000};
    OrderBook<Fifo> ob1(Instrument{"demo",100,49,50,2});
    RecordDepot<order::Record> record_depot;
    gen::OrderGenerator order_gen;

    order_gen.initialize_orderbook(ob1,record_depot,1000,10);

    for (int i{}; i < iterations; ++i)
    {
        ob1.submit_order(
                 order_gen.record_order(
                     order_gen.make_pending_order(ob1,record_depot,100)));

        if (ob1.bid() <= ob1.min_price() || ob1.ask() >= ob1.max_price())
        {
            break;
        }
    }

    OrderBook<Fifo> ob2(Instrument{"demo",100,49,50,2});


    int count{1000};
    while (count-- && !order_gen.recorded_orders.empty())
    {
        ob2.submit_order(order_gen.recorded_orders.front());
        ob2.evaluate_orders();
        ob2.match_orders();
        order_gen.recorded_orders.pop();
    }

    const auto start = std::chrono::high_resolution_clock::now();
    std::cout<<count<<" orders/second"<<std::endl;
    while (std::chrono::duration_cast<std::chrono::microseconds>(
         std::chrono::high_resolution_clock::now() - start).count() < 1000
        && !order_gen.recorded_orders.empty())
    {
        ob2.submit_order(order_gen.recorded_orders.front());
        ob2.evaluate_orders();
        ob2.match_orders();
        order_gen.recorded_orders.pop();
        ++count;
    }

    std::cout<<count<<" orders/ms"<<std::endl;

    return 0;
}