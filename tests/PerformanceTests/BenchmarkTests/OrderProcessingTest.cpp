//
// Created by prw on 9/1/25.
//
#include <benchmark/benchmark.h>

#include <queue>
#include "Instrument.h"
#include "Fifo.h"
#include "OrderBook.h"
#include "Record.h"
#include "OrderGenerator.h"

class OrdProcessTest : public benchmark::Fixture
{
    public:
    size_t iterations{1000000};
    OrderBook<Fifo> ob1;
    RecordDepot<order::Record> rd1;
    gen::OrderGenerator order_gen;

    OrderBook<Fifo> ob2;
    RecordDepot<order::Record> rd2;


    int count{};
    OrdProcessTest()
    :   ob1(Instrument{"demo",100,49,50,2}),
        ob2(Instrument{"demo",100,49,50,2})
    {
        Iterations(static_cast<long>(iterations));
    }

    void SetUp(const::benchmark::State& state) override
    {
        order_gen.initialize_orderbook(ob1,rd1,1000,10);

        for (int i{}; i < iterations; ++i)
        {
            ob1.submit_order(
                order_gen.record_order(
                    order_gen.make_pending_order(ob1,rd1,100)));

            ob1.evaluate_orders();

            if (ob1.bid() <= ob1.min_price() || ob1.ask() >= ob1.max_price())
            {
                break;
            }
        }
    }

    void process_next_order()
    {
        if(!order_gen.recorded_orders.empty())
        {
            ob2.submit_order(order_gen.recorded_orders.front());
            ob2.evaluate_orders();
            ob2.match_orders();
            order_gen.recorded_orders.pop();
        }
    }
};

BENCHMARK_DEFINE_F(OrdProcessTest,op_test)(benchmark::State &state)
{
    for (auto _  : state)
    {
        process_next_order();
    }
}
BENCHMARK_REGISTER_F(OrdProcessTest,op_test);
