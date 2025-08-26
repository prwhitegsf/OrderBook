
#include <thread>

#include "Printer.h"
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

    auto order_book_processing = [&](auto&& o)
    {
        order_book.submit_order(o);
        order_book.evaluate_orders();
        order_book.match_orders();

    };


    for (int i{}; i < iterations; ++i)
    {
        order_book_processing(order_gen.make_random_order(order_book,record_depot,50));

        record_depot.record_matched_orders(order_book.get_matched_orders());
        record_depot.update_order_records();

        printer::print_trade_records(record_depot);
        printer::print_bid_ask(std::cout,order_book);
        printer::print_dom(std::cout,order_book,dom_window);

        if (order_book.bid() <= order_book.min_price() || order_book.ask() >= order_book.max_price())
        {
            std::cout<< "Circuit breaker hit"<<std::endl;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(wait));
    }

    order_gen.print_submitted_stats();

    //printer::print_accepted_orders(record_depot);
    //printer::print_completed_orders(record_depot);


    return 0;
}
