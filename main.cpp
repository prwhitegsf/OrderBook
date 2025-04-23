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
using Fifo = FifoMatchingStrategy;
using Q = OrderQueue<Orderbook<Fifo>>;


template<typename T>
OrderUpdate CreateOrder(T submitted_order,Orderbook<Fifo>& ob)
{
    ob.instrument_->client_order_list_.append_order(submitted_order);
    return ob.SubmitOrder(submitted_order.make_queued_order());
}

//template<typename Matcher>
void print_all_orders(FifoMatchingStrategy& matcher)
{
    std::cout << std::format("{:7}","   Price");
    std::cout << std::format("{:7}","   Depth")<<std::endl;

    int idx = matcher.num_prices()-1;

    while (idx >= 0)
    {

        if (idx + 1 == matcher.ask_idx())
            std::cout << "......................"<<std::endl;

        if(matcher.levels_[idx].depth_ != 0)
        {
            std::cout << std::format("{:7}", idx);
            std::cout << std::format("{:7}",matcher.levels_[idx].depth_);
            for (auto const& order : matcher.levels_[idx].limit_orders_)
            {
                std::cout << std::format("{:4}", order.qty_);
            }

            std::cout <<std::endl;
        }

        --idx;

    }
}
int main()
{
    Print print;



    auto instrument = std::make_shared<Instrument>();

    Fifo fifo(100);
    Orderbook<Fifo> ob(instrument,fifo);
    Q q{};

    std::cout<<"==================="<<std::endl;

    //OrderQueue<Orderbook<FifoMatchingStrategy<FixedSizeLadder>,FixedSizeLadder>> q{};
    q.generate_orders(49,50,{5,4,3,2},ob);
    q.SubmitAll(ob);
    //ob.print_all_orders(fifo);
    std::cout<<"==================="<<std::endl;
    print_all_orders(ob.matcher_);

    /*std::cout<<"==================="<<std::endl;
    print(instrument->ask(),"Ask: ");
    print(instrument->bid(),"Bid: ");
    std::cout<<"==================="<<std::endl;
    //ob.price_ladder_.ask_ = ob.price_ladder_.levels_.begin()+ob.price_ladder_.ask_idx_;
    //std::cout <<"Ptr to Depth: " <<ob.matcher_.ask_->depth_ << std::endl;



   std::cout<<"==================="<<std::endl;

    OrderUpdate sell_limit2 = CreateOrder(SubmittedSellLimit(6,49,Duration::DAY),ob);
    print(sell_limit2);*/


    std::cout<<"==================="<<std::endl;
    print(instrument->ask(),"Ask: ");
    print(instrument->bid(),"Bid: ");
    std::cout<<"==================="<<std::endl;





    /*
    OrderUpdate sell_limit = CreateOrder(SubmittedSellLimit(10,52,Duration::DAY),ob);
    print(sell_limit);
    ob.print_all_orders();

    std::cout<<"==================="<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");
    std::cout<<"==================="<<std::endl;
    std::cout <<"Ptr to Depth: " <<ob.matcher_.ask_->depth_ << std::endl;


    OrderUpdate cancel = CreateOrder(SubmittedCancel(sell_limit._.id_,sell_limit._.price_),ob);
    std::cout<<"==================="<<std::endl;
    print(sell_limit);
    ob.print_all_orders();

    std::cout<<"==================="<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");
    std::cout<<"==================="<<std::endl;
    std::cout <<"Ptr to Depth: " <<ob.matcher_.ask_->depth_ << std::endl;

    OrderUpdate buy_m = CreateOrder(SubmittedSellMarket(6),ob);
    print(buy_m);
    ob.print_all_orders();

    std::cout<<"==================="<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");
    std::cout<<"==================="<<std::endl;
    std::cout <<"Ptr to Depth: " <<ob.price_ladder_.ask_->depth_ << std::endl;


    OrderUpdate sell_limit2 = CreateOrder(SubmittedSellLimit(6,49,Duration::DAY),ob);
    print(sell_limit2);
    ob.print_all_orders();

    std::cout<<"==================="<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");
    std::cout<<"==================="<<std::endl;
    std::cout <<"Ptr to Depth: " <<ob.price_ladder_.ask_->depth_ << std::endl;


    OrderUpdate buy_limit = CreateOrder(SubmittedBuyLimit(6,49,Duration::DAY),ob);
    print(buy_limit);
    ob.print_all_orders();

    std::cout<<"==================="<<std::endl;
    print(instrument->bid(),"Bid: ");
    print(instrument->ask(),"Ask: ");
    std::cout<<"==================="<<std::endl;
    std::cout <<"Ptr to Depth: " <<ob.price_ladder_.ask_->depth_ << std::endl;


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