//
// Created by prw on 4/25/25.
//
#include <random>
#include <vector>
#include "gtest/gtest.h"

#include "../src/OrderTypes/SubmittedOrderTypes.h"
#include "../src/Levels/DequeLevel/DequeLevel.h"
#include "../src/DOMS/MidLadder/MidLadder.h"
#include "../src/Matchers/FIFO/FifoStrategy.h"
#include "../src/Orderbook/Orderbook.h"

#include "../src/Generators/OrderFactories.h"
#include "../src/Printer/Printer.h"


using Dom = MidLadder<DequeLevel>;
using Fifo = FifoStrategy<Dom>;

class OrderBookAndRecordsTest : public ::testing::Test
{

public:
    std::shared_ptr<Instrument> inst;
    Fifo f;
    Orderbook<Dom, Fifo> ob;

    OrderBookAndRecordsTest()
        : inst(std::make_shared<Instrument>("Instr",0,100,1,50,51 )),
          ob(inst,f){}

    Orderbook<Dom, Fifo> empty_orderbook(size_t min,size_t max,size_t bid, size_t ask) const
    {
        return Orderbook<Dom, Fifo>(std::make_shared<Instrument>("Empty",min, max, 1, 50, 51),f);
    }


};

TEST_F(OrderBookAndRecordsTest,EmptyBook)
{
    EXPECT_EQ(ob.num_prices(),100);

    auto obk = empty_orderbook(0,500,50,51);
    EXPECT_EQ(obk.num_prices(),500);

    for (auto& lev: ob.dom())
    {
        EXPECT_EQ(lev.depth_,0);
    }

}

TEST_F(OrderBookAndRecordsTest,SubmitOrder)
{

    SubmittedBuyLimit<Order> buy_limit(5,50,Duration::DAY);
    ob.inst_->order_records_.append_order(buy_limit);
    OrderUpdate bl = ob.SubmitOrder(buy_limit.make_queued_order());

    EXPECT_EQ(ob.depth(50),5);

    SubmittedSellMarket<Order> sell_market(3);
    ob.inst_->order_records_.append_order(sell_market);
    ob.SubmitOrder(sell_market.make_queued_order());

    EXPECT_EQ(ob.depth(50),2);

    SubmittedSellLimit<Order> sell_limit(5,51,Duration::DAY);
    ob.inst_->order_records_.append_order(sell_limit);
    ob.SubmitOrder(sell_limit.make_queued_order());

    EXPECT_EQ(ob.depth(51),5);

    SubmittedBuyMarket<Order> buy_market(3);
    ob.inst_->order_records_.append_order(buy_market);
    ob.SubmitOrder(buy_market.make_queued_order());

    EXPECT_EQ(ob.depth(51),2);


    SubmittedCancel<Order> cancel(bl.order.id_,bl.order.price_);
    ob.inst_->order_records_.append_order(cancel);
    ob.SubmitOrder(cancel.make_queued_order());
    EXPECT_EQ(ob.depth(50),0);
}

TEST_F(OrderBookAndRecordsTest,RecordList1)
{
    OrderUpdate sell_limit =
        gen::CreateOrder(SubmittedSellLimit<Order>(10,51,Duration::DAY),ob);

    // get order record by id
    //auto res_sell = ob.inst_->order_records_.get_order(sell_limit.order.id_);
    auto res_sell = inst->order_records_.get_order(sell_limit.order.id_);

    if (const SubmittedSellLimit<Order>* ord = std::get_if<SubmittedSellLimit<Order>>(&res_sell))
    {
        EXPECT_EQ(ord->order.id_,sell_limit.order.id_);
        EXPECT_EQ(ord->order.price_,sell_limit.order.price_);
        EXPECT_EQ(ord->state_,sell_limit.state_);
    }

    OrderUpdate buy_limit =
        gen::CreateOrder(SubmittedBuyLimit<Order>(10,50,Duration::DAY),ob);

    // get order record by id
    auto res_buy = inst->order_records_.get_order(sell_limit.order.id_);

    if (const SubmittedBuyLimit<Order>* ord = std::get_if<SubmittedBuyLimit<Order>>(&res_buy))
    {
        EXPECT_EQ(ord->order.id_,buy_limit.order.id_);
        EXPECT_EQ(ord->order.price_,buy_limit.order.price_);
        EXPECT_EQ(ord->state_,buy_limit.state_);
    }

}