//
// Created by prw on 8/14/25.
//

#include <benchmark/benchmark.h>
#include <ranges>
#include "Fifo.h"
#include "Evaluator.h"
#include "OrderBook.h"
#include "OrderGenerator.h"
#include "Record.h"


class FifoBench : public benchmark::Fixture
{
public:
    size_t iterations{4000000};
    OrderBook<Fifo> order_book;
    RecordDepot<order::Record> record_depot;
    std::queue<order::Pending> pq;

     gen::OrderGenerator order_gen;


    int count{};
    FifoBench() : order_book(Instrument{"demo",100,49,50,2})
    {
        Iterations(iterations);
    }

    void SetUp(const::benchmark::State& state) override
    {
        order_gen.initialize_orderbook(order_book,record_depot,1000,10);

        for (int i{}; i < iterations; ++i)
        {
            order_book.submit_order(order_gen.make_pending_order(order_book,record_depot,100));
            order_book.accept_order();

            if (order_book.bid() <= order_book.min_price() || order_book.ask() >= order_book.max_price())
            {
                break;
            }
        }
    }

};





BENCHMARK_DEFINE_F(FifoBench, matcherTest)(benchmark::State &state)
{

    for (auto _ : state)
    {
        order_book.match();
    }
}

BENCHMARK_REGISTER_F(FifoBench, matcherTest);

BENCHMARK_MAIN();




/*
static void BENCHMARK_demo(benchmark::State &state)
{

    OrderBook<Fifo> order_book(Instrument{"demo",100,49,50,2});
    RecordDepot<order::Record> record_depot;
    std::queue<order::Pending> pq;
    gen::OrderGenerator order_gen;


    order_gen.initialize_orderbook(order_book,record_depot,1000,10);

    size_t iterations{4000000};
    for (int i{}; i < iterations; ++i)
    {
        order_book.submit_order(order_gen.make_pending_order(order_book,record_depot,100));
        order_book.accept_order();

        if (order_book.bid() <= order_book.min_price() || order_book.ask() >= order_book.max_price())
        {
            break;
        }

    }
    while (!order_book.pending_q_.empty())
    {
        pq.push(order_book.pending_q_.front());
        order_book.pending_q_.pop();
    }



    for (auto _ : state)
    {
        if (!pq.empty())
        {
            order_book.pending_q_.push(pq.front());
            pq.pop();
            order_book.match_order();
        }

    }
}

BENCHMARK(BENCHMARK_demo)->Iterations(4000000);//->Unit(benchmark::kMillisecond)

BENCHMARK_MAIN();*/