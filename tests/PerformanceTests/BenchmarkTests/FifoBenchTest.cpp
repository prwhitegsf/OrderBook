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

class FifoBenchTest : public benchmark::Fixture
{
public:
    size_t iterations{10000000};
    OrderBook<Fifo> order_book;
    RecordDepot<order::Record> record_depot;
    std::queue<order::Pending> pq;

    gen::OrderGenerator order_gen;


    int count{};
    FifoBenchTest() : order_book(Instrument{"demo",100,49,50,2})
    {
        Iterations(static_cast<long>(iterations));
    }

    void SetUp(const::benchmark::State& state) override
    {
        order_gen.initialize_orderbook(order_book,record_depot,1000,10);

        for (int i{}; i < iterations; ++i)
        {
            order_book.submit_order(order_gen.make_pending_order(order_book,record_depot,100));
            order_book.evaluate_orders();

            if (order_book.bid() <= order_book.min_price() || order_book.ask() >= order_book.max_price())
            {
                break;
            }
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





static void BM_FifoMatching(benchmark::State& state)
{
    OrderBook<Fifo> order_book(Instrument{});
    RecordDepot<order::Record> record_depot;
    gen::OrderGenerator order_gen;

    order_gen.initialize_orderbook(order_book,record_depot,1000,10);

    for (auto _ : state)
    {
        state.PauseTiming();
        order_book.submit_order(order_gen.make_pending_order(order_book,record_depot,100));
        order_book.evaluate_orders();
        state.ResumeTiming();

        order_book.match_orders();

    }
}

BENCHMARK_REGISTER_F(FifoBenchTest, matcherTest);
BENCHMARK(BM_FifoMatching);

BENCHMARK_MAIN();