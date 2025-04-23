#include <iostream>
#include <variant>
#include <algorithm>

#include "Printer.h"
#include "OrderTypes/SubmittedOrderTypes.h"
#include "OrderQueue/OrderQueue.h"
#include "PriceLadders/FixedSizeLadder/FixedSizeLadder.h"
#include "Orderbook/Orderbook.h"
#include "Matchers/FIFO/FifoMatchingStrategy.h"
#include "Instrument/Instrument.h"

using Ladder = FixedSizeLadder;
using Fifo = FifoMatchingStrategy<Ladder>;
using Q = OrderQueue<Orderbook<Fifo,Ladder>>;


template<typename T>
OrderUpdate CreateOrder(T submitted_order,Orderbook<Fifo, Ladder>& ob)
{
    ob.instrument_->client_order_list_.append_order(submitted_order);
    return ob.SubmitOrder(submitted_order.make_queued_order());
}


int main()
{
    Print print;



    auto instrument = std::make_shared<Instrument>();

    Fifo fifo;
    Orderbook<Fifo,Ladder> ob(instrument,Ladder(instrument),fifo);
    Q q{};

    std::cout<<"---------------------"<<std::endl;

    //OrderQueue<Orderbook<FifoMatchingStrategy<FixedSizeLadder>,FixedSizeLadder>> q{};
    q.generate_orders(10,11,{5,4,3,2},ob);
    q.SubmitAll(ob);
    ob.print_all_orders();

    std::cout<<"---------------------"<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");

    std::cout<<"---------------------"<<std::endl;

    OrderUpdate sell_limit = CreateOrder(SubmittedSellLimit(20,10,Duration::DAY),ob);
    print(sell_limit);
    ob.print_all_orders();

    std::cout<<"---------------------"<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");

    OrderUpdate buy_limit = CreateOrder(SubmittedBuyLimit(21,11,Duration::DAY),ob);
    print(buy_limit);
    ob.print_all_orders();





}