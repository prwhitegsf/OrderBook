#include <iostream>
#include <variant>

#include "Instrument/Instrument.h"
#include "OrderTypes/SubmittedOrderTypes.h"
#include "Levels/DequeLevel/DequeLevel.h"
#include "DOMS/MidLadder/MidLadder.h"
#include "Matchers/FIFO/FifoStrategy.h"
#include "Orderbook/Orderbook.h"
#include "Printer/Printer.h"
#include "Generators/OrderFactories.h"



using Dom = MidLadder<DequeLevel>;
using Fifo = FifoStrategy<Dom>;

void spcr()
{
    std::cout<<"======================="<<std::endl;
}

void lb()
{
    std::cout<<std::endl;
}

int main()
{
    Print print;

    std::shared_ptr<Instrument> instrument = std::make_shared<Instrument>
    ("Instr",0,500,1,50,51 );

    Fifo fifo;
    Orderbook<Dom,Fifo> ob(instrument, fifo);

    gen::GenerateOrderBook(50,51,ob,{5,4,3,2});
    print.ob_all(ob);


    lb();
    std::cout<<"====Client Orders======"<<std::endl;
    spcr();
    std::cout<<std::endl;

    OrderUpdate sell_limit = gen::CreateOrder(SubmittedSellLimit<Order>(10,58,Duration::DAY),ob);
    print(sell_limit);
    lb();
    print.ob_all(ob);



    lb();
    print.submitted_order(sell_limit.order.id_, instrument);
    lb();





    OrderUpdate cancel =gen::CreateOrder(SubmittedCancel<Order>(sell_limit.order.id_,sell_limit.order.price_),ob);
    spcr();
    print(cancel);

    print(sizeof(OrderState::SUBMITTED), "float: ");

    OrderUpdate bm = gen::CreateOrder(SubmittedBuyMarket<Order>(4),ob);
    print(bm);
    lb();
    print.ob_all(ob);

    //print.all_submitted(instrument);

    auto res = instrument->order_records_.get_order(bm.order.id_);

    if (const SubmittedBuyMarket<Order>* ord = std::get_if<SubmittedBuyMarket<Order>>(&res))
    {
        print(bm.order.id_,"Placed id: ");
        std::cout<<"Retrieved id: " << ord->order.id_<<std::endl;
    }




}