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
    size_t iterations{100000};
    OrderBook<Fifo> order_book;
    RecordDepot<order::Record> record_depot;

    size_t counter;
     gen::OrderGenerator order_gen;


    int count{};
    FifoBench() : order_book(Instrument{"demo",100,49,50,2}), counter(iterations+1000)
    {
       // MinTime(0.05);
     Iterations(100000);
    }

    void SetUp(const::benchmark::State& state) override
    {
       order_gen.initialize_orderbook(order_book,record_depot,1000,50);
        ++count;

        for (int i{}; i < iterations; ++i)
        {
            order_book.submit_order(order_gen.make_random_order(order_book,record_depot,50));
            order_book.accept_order();
            //   order_book.match_order();

            // record_depot.record_processed_orders(std::move(order_book.get_processed_orders()));
            //record_depot.update_order_records();



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

        if (!order_book.pending_q_.empty())
        {
            std::visit([this](auto&& o)
              {
                  order_book.push_matched(order_book.matcher_.match(o));

              },order_book.pending_q_.front());

            order_book.pending_q_.pop();

           // auto x = std::move(order_book.get_processed_orders());
        }
        else
        {
            break;
         //   order_gen.initialize_orderbook(order_book,record_depot,1000,50);
            //std::cout<<count<<std::endl;
            //order_book.submit_order(order::BuyLimit(++counter, 10, order_book.bid()));
           // order_book.submit_order(order::SellLimit(++counter, 10, order_book.ask()));
        }
       // std::cout<<order_book.pending_q_.size()<<std::endl;
      //  order_book.match_order();

    }


}

BENCHMARK_REGISTER_F(FifoBench, matcherTest);

BENCHMARK_MAIN();

/*BENCHMARK(BENCHMARK_demo)->Unit(benchmark::kMillisecond)->Setup(DoSetup)->Teardown(DoTeardown);

BENCHMARK_MAIN();*/