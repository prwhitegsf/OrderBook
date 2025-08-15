//
// Created by prw on 8/14/25.
//
#include <chrono>
#include <ranges>
#include "Fifo.h"
#include "Evaluator.h"
#include "OrderBook.h"
#include "OrderGenerator.h"
#include "Record.h"



double avg_per_trade(size_t runs, size_t iterations)
{

    std::vector<size_t> times;
    while (runs--)
    {
        OrderBook<Fifo> order_book(Instrument{});
        RecordDepot<order::Record> record_depot;

        gen::OrderGenerator order_gen;

        order_gen.initialize_orderbook(order_book,record_depot,1000,50);
        auto start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();

        for (int i{}; i < iterations; ++i)
        {
            order_book.submit_order(order_gen.make_random_order(order_book,record_depot,50));
            order_book.accept_order();

            start = std::chrono::high_resolution_clock::now();
            order_book.match_order();
            end = std::chrono::high_resolution_clock::now();
            times.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());

            record_depot.record_processed_orders(std::move(order_book.get_processed_orders()));
            record_depot.update_order_records();



            if (order_book.bid() <= order_book.min_price() || order_book.ask() >= order_book.max_price())
            {
                break;
            }
        }
    }

        size_t total_dur = std::accumulate(times.begin(), times.end(), size_t(0));
        auto average_dur = static_cast<double>(total_dur) / static_cast<double>(times.size());

        return average_dur;
}

double avg_to_clear_q(size_t runs, size_t iterations)
{
    std::vector<size_t> times;
    while (runs--)
    {

        OrderBook<Fifo> order_book(Instrument{});
        RecordDepot<order::Record> record_depot;

        gen::OrderGenerator order_gen;

        order_gen.initialize_orderbook(order_book,record_depot,1000,50);
        auto start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();

        for (int i{}; i < iterations; ++i)
        {
            order_book.submit_order(order_gen.make_random_order(order_book,record_depot,50));
            order_book.accept_order();




            if (order_book.bid() <= order_book.min_price() || order_book.ask() >= order_book.max_price())
            {
                break;
            }

        }
        start = std::chrono::high_resolution_clock::now();
        order_book.match_order();



        end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

        //record_depot.record_processed_orders(std::move(order_book.get_processed_orders()));
        //record_depot.update_order_records();
    }
    size_t total_dur = std::accumulate(times.begin(), times.end(), size_t(0));
    auto average_dur = static_cast<double>(total_dur) / static_cast<double>(times.size());

    return average_dur;
}

int main(){


    size_t runs{10}, iterations{100000};
    auto per_trade = avg_per_trade(runs,iterations);
    //auto clear_q = avg_to_clear_q(runs,iterations);

    std::cout<<"us / trade: "<<per_trade<<std::endl;;
   // std::cout<<"us to clear 10000: "<<clear_q<<std::endl;;
   // std::cout<<"clear_q/per_trade: "<<clear_q/per_trade<<std::endl;;

    return 0;

}