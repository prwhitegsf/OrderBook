//#include <iostream>

#include <algorithm>
#include <ranges>

#include "Printer.h"

#include <thread>

#include "OrderGenerator.h"
#include "Record.h"


void accept_and_match(auto& order_book)
{

    order_book.accept_order();
    order_book.match_order();

}
int main()
{

    constexpr size_t iterations{100000}; // number of trades to simulate after initialization
    constexpr size_t dom_window{5}; // number of prices on either side of bid/ask to display depth for
    constexpr size_t wait{0}; // pause (in ms) between trades

    OrderBook<Fifo> order_book(Instrument{});
    RecordDepot<order::Record> record_depot;

    gen::OrderGenerator order_gen;
    order_gen.initialize_orderbook(order_book,record_depot,1000,50);


    for (int i{}; i < iterations; ++i)
    {
        order_book.submit_order(
            order_gen.record_order(order_gen.make_pending_order(order_book,record_depot,50)));

        accept_and_match(order_book);
        record_depot.record_processed_orders(std::move(order_book.get_processed_orders()));
        record_depot.update_order_records();


      //  printer::print_trade_records(record_depot);
   //     printer::print_bid_ask(std::cout,order_book);
     //   printer::print_dom(std::cout,order_book,dom_window);

        if (order_book.bid() <= order_book.min_price() || order_book.ask() >= order_book.max_price())
        {
            break;
        }

       // std::this_thread::sleep_for(std::chrono::milliseconds(wait));
    }

    for (int i{}; i < order_book.num_prices()-1; ++i)
    {
        std::cout<<i<<' '<<order_book.depth(i)<<' '<<order_book.count(i)<<std::endl;
    }
    std::cout<<std::endl;
    /*for (auto& c : order_book.order_counts())
        std::cout<<c<<' '<<std::endl;*/
    //printer::print_dom(std::cout,order_book,dom_window);
    /*printer::print_accepted_orders(record_depot);
    printer::print_completed_orders(record_depot);
    */


    printer::print_dom(std::cout,order_book,23);
    auto key_view = order_gen.submitted_stats | std::views::keys;
    std::vector<std::string> sorted_keys;
    std::ranges::copy(key_view, std::back_inserter(sorted_keys));
    std::ranges::sort(sorted_keys);

    for (auto& k : sorted_keys)
        std::cout<<k<<' '<< order_gen.submitted_stats.at(k)<<std::endl;
    /*order_gen.write_recorded_to_csv();
    order_gen.read_recorded_orders_from_csv();

    std::cout<<order_gen.order_playback_q.size()<<std::endl;
    while (!order_gen.order_playback_q.empty())
    {

        std::visit([&](auto& ord)
        {
                std::cout<<order::order_types[std::type_index(typeid(ord))]<<"," << ord.id<<","<<ord.qty<<","<<ord.price<<"\n";

        }, order_gen.order_playback_q.front());

        order_gen.order_playback_q.pop();
    }*/

    /*printer::print_accepted_orders(record_depot);
    printer::print_completed_orders(record_depot);*/


    return 0;
}
