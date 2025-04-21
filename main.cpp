#include <iostream>
#include <variant>
#include <algorithm>

#include "Printer.h"
#include "OrderTypes/ClientOrders/ClientOrders.h"
#include "PriceLadders/FixedSizeLadder/FixedSizeLadder.h"
#include "Orderbook/Orderbook.h"
#include "Matchers/FIFO/Fifo.h"
#include "Instrument/Instrument.h"

int main()
{
    Print print;
    auto instrument = std::make_shared<Instrument>();
    Fifo<FixedSizeLadder> fifo;
    Orderbook<Fifo<FixedSizeLadder>,FixedSizeLadder> ob(instrument,FixedSizeLadder(instrument),fifo);


    ob.generate_orders(10.00,10.25,{2,2,5,3});
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");

    ob.print_all_orders();

    std::cout<<"---------------------"<<std::endl;

    OrderUpdate lm = ob.SubmitOrder(BuyLimitOrder(4,4,9.25,Duration::DAY));
    //print(lm, "New Limit Order: ");

    instrument->client_order_list_.print_order(lm.id_);
    ob.print_all_orders();

    OrderUpdate co = ob.SubmitOrder(CancelOrder(lm.id_,lm.price_));
    instrument->client_order_list_.print_order(lm.id_);
    ob.print_all_orders();
    //ob.SubmitOrder(BuyStopOrder(11,10.50,Duration::DAY));
   //OrderUpdate buy = ob.SubmitOrder(BuyMarketOrder(3));

    //print(buy, "Buy Order:");


    /*
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");

    ob.print_all_orders();
    */

    //FixedSizeLadder& dom = ob.price_ladder_;

    //print(dom.level(dom.idx_from_price(9.25)).limit_orders_, "Limit Orders: ");

}