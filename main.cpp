#include <iostream>
#include <variant>
#include <algorithm>

#include "DequeLevel.h"
#include "Printer.h"
#include "OrderTypes/SubmittedOrderTypes.h"
#include "OrderQueue/OrderQueue.h"
#include "Matchers/FIFO/FifoMatchingStrategy.h"
#include "Orderbook/Orderbook.h"

#include "Instrument/Instrument.h"
#include "Generators/OrderFactories.h"


using Fifo = FifoMatchingStrategy<MidLadder<Order,DequeLevel>>;
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



    auto instrument = std::make_shared<Instrument>
    ("Instr",1,100,1,50,51 );

    Fifo fifo;
    Orderbook<Fifo> ob(instrument,fifo);
    //Q q{};



    gen::GenerateOrderBook(55,56,ob,{5,4,3,2});
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





}