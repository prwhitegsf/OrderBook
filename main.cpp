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

    Fifo fifo(100);
    Orderbook<Fifo,Ladder> ob(instrument,Ladder(instrument),fifo);
    Q q{};

    std::cout<<"==================="<<std::endl;

    //OrderQueue<Orderbook<FifoMatchingStrategy<FixedSizeLadder>,FixedSizeLadder>> q{};
    q.generate_orders(49,50,{5,4,3,2},ob);
    q.SubmitAll(ob);
    ob.print_all_orders();

    std::cout<<"==================="<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");
    std::cout<<"==================="<<std::endl;
    //ob.price_ladder_.ask_ = ob.price_ladder_.levels_.begin()+ob.price_ladder_.ask_idx_;
    std::cout <<"Ptr to Depth: " <<ob.matcher_.ask_->depth_ << std::endl;


   /*
   std::cout<<"==================="<<std::endl;

    OrderUpdate sell_limit = CreateOrder(SubmittedSellLimit(20,11.75,Duration::DAY),ob);
    print(sell_limit);
    ob.print_all_orders();

    std::cout<<"==================="<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");
    std::cout<<"==================="<<std::endl;
    std::cout <<"Ptr to Depth: " <<ob.price_ladder_.ask_->depth_ << std::endl;

    OrderUpdate buy_m = CreateOrder(SubmittedBuyMarket(1),ob);
    print(buy_m);
    ob.print_all_orders();

    std::cout<<"==================="<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");
    std::cout<<"==================="<<std::endl;
    std::cout <<"Ptr to Depth: " <<ob.price_ladder_.ask_->depth_ << std::endl;*/


    /*
    std::cout<<"==================="<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");
    std::cout<<"==================="<<std::endl;

    OrderUpdate buy_limit = CreateOrder(SubmittedBuyLimit(21,11,Duration::DAY),ob);
    print(buy_limit);
    ob.print_all_orders();



    std::cout<<"==================="<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");

    print(ob.price_ladder_.ask_idx_,"AskIdx: ");
    print(ob.price_ladder_.price_from_idx(41.2),"AvgPrice: ");*/




}