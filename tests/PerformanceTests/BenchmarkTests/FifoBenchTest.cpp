//
// Created by prw on 8/16/25.
//

#include <benchmark/benchmark.h>

#include <queue>
#include "Instrument.h"
#include "Fifo.h"
#include "OrderBook.h"
#include "Record.h"
#include "OrderGenerator.h"
#include "Benchmarks.h"

class FifoBenchTest : public benchmark::Fixture
{
public:
    size_t iterations{1000000};

    Instrument instrument;
    std::queue<order::Submitted> order_seq;
    OrderBook<Fifo> order_book;


    FifoBenchTest()
    : instrument("demo",400,199,200,4),
    order_seq(bench::make_order_sequence(instrument,iterations)),order_book(instrument)
    {
        Iterations(static_cast<long>(iterations));
    }

    void SetUp(const::benchmark::State& state) override
    {
        while (!order_seq.empty())
        {
            order_book.submit_order(order_seq.front());
            order_book.evaluate_orders();
            order_seq.pop();
        }
    }

};


BENCHMARK_DEFINE_F(FifoBenchTest, matcherTest)(benchmark::State &state)
{
    for (auto _ : state)
    {
        order_book.match_next_order();
    }
}

BENCHMARK_REGISTER_F(FifoBenchTest, matcherTest);
BENCHMARK_MAIN();