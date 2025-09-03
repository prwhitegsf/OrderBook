//
// Created by prw on 9/1/25.
//
#include <benchmark/benchmark.h>

#include "Benchmarks.h"
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
    Instrument instrument;
    std::queue<order::Submitted> order_seq;

    OrderBook<Fifo> ob;

    OrdProcessTest()
        : instrument("demo",400,199,200,4),
        order_seq(bench::make_order_sequence(instrument,iterations)),ob(instrument)
    {
        Iterations(static_cast<long>(iterations));
    }



    void process_next_order()
    {
        if(!order_seq.empty())
        {
            ob.submit_order(order_seq.front());
            ob.evaluate_orders();
            ob.match_orders();
            order_seq.pop();
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
