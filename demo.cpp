//#include <iostream>

#include <ranges>

#include "Printer.h"

#include <thread>

#include "OrderGenerator.h"
#include "Record.h"



int main()
{

    constexpr size_t iterations{10000}; // number of trades to simulate after initialization
    constexpr size_t dom_window{5}; // number of prices on either side of bid/ask to display depth for
    constexpr size_t wait{0}; // pause (in ms) between trades

    OrderBook<Fifo> order_book(Instrument{});
    RecordDepot<order::Record> record_depot;

    gen::OrderGenerator order_gen;
    order_gen.initialize_orderbook(order_book,record_depot,1000,50);


    for (int i{}; i < iterations; ++i)
    {
        order_book.submit_order(
            order_gen.record_order(order_gen.make_random_order(order_book,record_depot,50)));
        order_book.accept_order();
        order_book.match_order();

        record_depot.record_processed_orders(std::move(order_book.get_processed_orders()));
        record_depot.update_order_records();


        printer::print_trade_records(record_depot);
   //     printer::print_bid_ask(std::cout,order_book);
     //   printer::print_dom(std::cout,order_book,dom_window);

        if (order_book.bid() <= order_book.min_price() || order_book.ask() >= order_book.max_price())
        {
            break;
        }

       // std::this_thread::sleep_for(std::chrono::milliseconds(wait));
    }

    order_gen.write_recorded_to_csv();
    order_gen.read_recorded_orders_from_csv();

    std::cout<<order_gen.order_playback_q.size()<<std::endl;
    while (!order_gen.order_playback_q.empty())
    {

        std::visit([&](auto& ord)
        {
                std::cout<<order::order_types[std::type_index(typeid(ord))]<<"," << ord.id<<","<<ord.qty<<","<<ord.price<<"\n";

        }, order_gen.order_playback_q.front());

        order_gen.order_playback_q.pop();
    }

    /*printer::print_accepted_orders(record_depot);
    printer::print_completed_orders(record_depot);*/


    return 0;
}
