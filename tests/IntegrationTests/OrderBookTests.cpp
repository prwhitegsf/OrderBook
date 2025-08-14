//
// Created by prw on 8/12/25.
//

#include "gtest/gtest.h"

#include "OrderBook.h"
#include "Fifo.h"
#include "Record.h"
#include "RecordDepot.h"
#include "OrderGenerator.h"
#include "Instrument.h"

using namespace order;

class OrderBookTest : public ::testing::Test
{


public:
    OrderBook<Fifo>ob;
    RecordDepot<Record> rd;
    OrderBookTest() : ob(Instrument("inst",100,49,50,3)) {}

    void process_order(auto&& o)
    {
        rd.make_order_record(o);
        ob.submit_order(o);
        ob.accept_order();
        ob.match_order();
        rd.record_processed_orders(std::move(ob.get_processed_orders()));
        rd.update_order_records();

    }
};

TEST_F(OrderBookTest, BuyLimit)
{
    ID id{42};

    process_order(BuyLimit(id,1,49));

    EXPECT_EQ(ob.depth(49),1);
    EXPECT_EQ(rd.accepted().at(id).id, id);
    EXPECT_EQ(rd.accepted().at(id).limit_price, 49);
    EXPECT_EQ(rd.accepted().at(id).quantities.back(), 1);
    EXPECT_EQ(rd.accepted().at(id).states.back(), OrderState::ACCEPTED);
}


TEST_F(OrderBookTest, SellLimit)
{
    ID id{42};

    process_order(SellLimit(id,1,55));

    EXPECT_EQ(ob.depth(55),1);
    EXPECT_EQ(rd.accepted().at(id).id, id);
    EXPECT_EQ(rd.accepted().at(id).limit_price, 55);
    EXPECT_EQ(rd.accepted().at(id).quantities.back(), 1);
    EXPECT_EQ(rd.accepted().at(id).states.back(), OrderState::ACCEPTED);
}

TEST_F(OrderBookTest, BuyLimitReject)
{
    ID id{42};
    process_order(BuyLimit(id,1,75));

    EXPECT_EQ(rd.accepted().size(),0);
    EXPECT_EQ(rd.completed().size(),1);

    EXPECT_EQ(rd.completed().at(id).id, id);
    EXPECT_EQ(rd.completed().at(id).limit_price, 75);
    EXPECT_EQ(rd.completed().at(id).quantities.back(), 1);
    EXPECT_EQ(rd.completed().at(id).states.back(), OrderState::REJECTED);

}



TEST_F(OrderBookTest, SellLimitReject)
{

    ID id{42};
    process_order(SellLimit(id,1,25));

    EXPECT_EQ(rd.accepted().size(),0);
    EXPECT_EQ(rd.completed().size(),1);

    EXPECT_EQ(rd.completed().at(id).id, id);
    EXPECT_EQ(rd.completed().at(id).limit_price, 25);
    EXPECT_EQ(rd.completed().at(id).quantities.back(), 1);
    EXPECT_EQ(rd.completed().at(id).states.back(), OrderState::REJECTED);

}

TEST_F(OrderBookTest, BuyMarket)
{
    // Limit orders to execute market order against
    process_order(SellLimit(42,5,50));
    EXPECT_EQ(ob.depth(50),5);

    // market order
    process_order(BuyMarket(43,3,55));
    EXPECT_EQ(ob.depth(50),2);

    EXPECT_EQ(rd.accepted().size(),1);
    EXPECT_EQ(rd.completed().size(),1);

    // Market Execution
    EXPECT_EQ(rd.completed().at(43).id, 43);
    EXPECT_EQ(rd.completed().at(43).limit_price, 55);
    EXPECT_EQ(rd.completed().at(43).filled_price, 50);
    EXPECT_EQ(rd.completed().at(43).quantities.back(), 0);
    EXPECT_EQ(rd.completed().at(43).states.back(),OrderState::FILLED);

   // Limit order partial fill
    EXPECT_EQ(rd.accepted().at(42).id,42);
    EXPECT_EQ(rd.accepted().at(42).limit_price,50);
    EXPECT_EQ(rd.accepted().at(42).quantities.back(), 2);
    EXPECT_EQ(rd.accepted().at(42).states.back(),OrderState::PARTIAL);
}

TEST_F(OrderBookTest, SellMarket)
{
    // Limit orders to execute market order against
    process_order(BuyLimit(42,5,49));
    EXPECT_EQ(ob.depth(49),5);

    // market order
    process_order(SellMarket(43,3,45));
    EXPECT_EQ(ob.depth(49),2);

    EXPECT_EQ(rd.accepted().size(),1);
    EXPECT_EQ(rd.completed().size(),1);

    // Market Execution
    EXPECT_EQ(rd.completed().at(43).id, 43);
    EXPECT_EQ(rd.completed().at(43).limit_price, 45);
    EXPECT_EQ(rd.completed().at(43).filled_price, 49);
    EXPECT_EQ(rd.completed().at(43).quantities.back(), 0);
    EXPECT_EQ(rd.completed().at(43).states.back(),OrderState::FILLED);

    // Limit order partial fill
    EXPECT_EQ(rd.accepted().at(42).id,42);
    EXPECT_EQ(rd.accepted().at(42).limit_price,49);
    EXPECT_EQ(rd.accepted().at(42).quantities.back(), 2);
    EXPECT_EQ(rd.accepted().at(42).states.back(),OrderState::PARTIAL);
}


TEST_F(OrderBookTest, BuyMarketToBuyMarketLimit)
{

    // make sell orders to buy
    process_order(SellLimit(42,5,50));
    process_order(SellLimit(43,5,51));
    process_order(SellLimit(44,5,52));

    EXPECT_EQ(ob.depth(50),5);
    EXPECT_EQ(ob.depth(51),5);
    EXPECT_EQ(ob.depth(52),5);

    process_order(BuyMarket(45,18,51));
    EXPECT_EQ(ob.depth(50),0); // took all orders at 50
    EXPECT_EQ(ob.depth(51),8); // take all at 51, then place buy limit for remaining
    EXPECT_EQ(ob.depth(52),5); // no executions, 52 > limit price

    // has the sell order at 52 and remaining buy limit at 51
    EXPECT_EQ(rd.accepted().size(),2);
    // sell limit at 52 still there
    EXPECT_EQ(rd.accepted().at(44).quantities.back(),5); //still there
    EXPECT_EQ(rd.accepted().at(44).states.back(),OrderState::ACCEPTED); //still there

    // market order is now a partially filled limit order
    EXPECT_EQ(rd.accepted().at(45).quantities.back(),8);
    EXPECT_EQ(rd.accepted().at(45).states.back(),OrderState::PARTIAL);
    EXPECT_EQ(rd.accepted().at(45).type,"BuyMarketLimit");

    // Filled limit orders are in executions
    EXPECT_EQ(rd.completed().size(),2);
    EXPECT_EQ(rd.completed().at(42).quantities.back(),0);
    EXPECT_EQ(rd.completed().at(42).states.back(),OrderState::FILLED);

    EXPECT_EQ(rd.completed().at(43).quantities.back(),0);
    EXPECT_EQ(rd.completed().at(43).states.back(),OrderState::FILLED);

    //Now fill the remainder
    process_order(SellMarket(48,8,49));
    EXPECT_EQ(rd.completed().size(),4);
    EXPECT_EQ(rd.accepted().size(),1);
    EXPECT_EQ(rd.completed().at(45).quantities.back(),0);
    EXPECT_EQ(rd.completed().at(45).states.back(),OrderState::FILLED);
    EXPECT_EQ(rd.completed().at(45).type,"BuyMarketLimit");

}

TEST_F(OrderBookTest, SellMarketToSellMarketLimit)
{

    // make sell orders to buy
    process_order(BuyLimit(42,5,49));
    process_order(BuyLimit(43,5,48));
    process_order(BuyLimit(44,5,47));

    EXPECT_EQ(ob.depth(49),5);
    EXPECT_EQ(ob.depth(48),5);
    EXPECT_EQ(ob.depth(47),5);

    process_order(SellMarket(45,18,48));
    EXPECT_EQ(ob.depth(49),0); // took all orders
    EXPECT_EQ(ob.depth(48),8); // take all, then place buy limit for remaining
    EXPECT_EQ(ob.depth(47),5); // no executions, 47 < limit price

    // has the buy order at 47 and remaining sell limit at 48
    EXPECT_EQ(rd.accepted().size(),2);

    //  limit at 52 still there
    EXPECT_EQ(rd.accepted().at(44).quantities.back(),5); //still there
    EXPECT_EQ(rd.accepted().at(44).states.back(),OrderState::ACCEPTED); //still there

    // market order is now a partially filled limit order
    EXPECT_EQ(rd.accepted().at(45).quantities.back(),8);
    EXPECT_EQ(rd.accepted().at(45).states.back(),OrderState::PARTIAL);

    // Filled limit orders are in executions
    EXPECT_EQ(rd.completed().size(),2);
    EXPECT_EQ(rd.completed().at(42).quantities.back(),0);
    EXPECT_EQ(rd.completed().at(42).states.back(),OrderState::FILLED);

    EXPECT_EQ(rd.completed().at(43).quantities.back(),0);
    EXPECT_EQ(rd.completed().at(43).states.back(),OrderState::FILLED);

    //Now fill the remainder
    process_order(BuyMarket(48,8,55));
    EXPECT_EQ(rd.completed().size(),4);
    EXPECT_EQ(rd.accepted().size(),1);
    EXPECT_EQ(rd.completed().at(45).quantities.back(),0);
    EXPECT_EQ(rd.completed().at(45).states.back(),OrderState::FILLED);
    EXPECT_EQ(rd.completed().at(45).type,"SellMarketLimit");

}

TEST_F(OrderBookTest, BuyLimitToBuyMarketLimit)
{

    // make sell orders to buy
    process_order(SellLimit(42,5,50));
    process_order(SellLimit(43,5,51));
    process_order(SellLimit(44,5,52));

    EXPECT_EQ(ob.depth(50),5);
    EXPECT_EQ(ob.depth(51),5);
    EXPECT_EQ(ob.depth(52),5);

    process_order(BuyLimit(45,18,51));
    EXPECT_EQ(ob.depth(50),0); // took all orders at 50
    EXPECT_EQ(ob.depth(51),8); // take all at 51, then place buy limit for remaining
    EXPECT_EQ(ob.depth(52),5); // no executions, 52 > limit price

    // has the sell order at 52 and remaining buy limit at 51
    EXPECT_EQ(rd.accepted().size(),2);
    // sell limit at 52 still there
    EXPECT_EQ(rd.accepted().at(44).quantities.back(),5); //still there
    EXPECT_EQ(rd.accepted().at(44).states.back(),OrderState::ACCEPTED); //still there

    // market order is now a partially filled limit order
    EXPECT_EQ(rd.accepted().at(45).quantities.back(),8);
    EXPECT_EQ(rd.accepted().at(45).states.back(),OrderState::PARTIAL);

    // Filled limit orders are in executions
    EXPECT_EQ(rd.completed().size(),2);
    EXPECT_EQ(rd.completed().at(42).quantities.back(),0);
    EXPECT_EQ(rd.completed().at(42).states.back(),OrderState::FILLED);

    EXPECT_EQ(rd.completed().at(43).quantities.back(),0);
    EXPECT_EQ(rd.completed().at(43).states.back(),OrderState::FILLED);

}

TEST_F(OrderBookTest, SellLimitToSellMarketLimit)
{

    // make sell orders to buy
    process_order(BuyLimit(42,5,49));
    process_order(BuyLimit(43,5,48));
    process_order(BuyLimit(44,5,47));

    EXPECT_EQ(ob.depth(49),5);
    EXPECT_EQ(ob.depth(48),5);
    EXPECT_EQ(ob.depth(47),5);

    process_order(SellLimit(45,18,48));
    EXPECT_EQ(ob.depth(49),0); // took all orders
    EXPECT_EQ(ob.depth(48),8); // take all, then place buy limit for remaining
    EXPECT_EQ(ob.depth(47),5); // no executions, 47 < limit price

    // has the buy order at 47 and remaining sell limit at 48
    EXPECT_EQ(rd.accepted().size(),2);

    //  limit at 52 still there
    EXPECT_EQ(rd.accepted().at(44).quantities.back(),5); //still there
    EXPECT_EQ(rd.accepted().at(44).states.back(),OrderState::ACCEPTED); //still there

    // market order is now a partially filled limit order
    EXPECT_EQ(rd.accepted().at(45).quantities.back(),8);
    EXPECT_EQ(rd.accepted().at(45).states.back(),OrderState::PARTIAL);

    // Filled limit orders are in executions
    EXPECT_EQ(rd.completed().size(),2);
    EXPECT_EQ(rd.completed().at(42).quantities.back(),0);
    EXPECT_EQ(rd.completed().at(42).states.back(),OrderState::FILLED);

    EXPECT_EQ(rd.completed().at(43).quantities.back(),0);
    EXPECT_EQ(rd.completed().at(43).states.back(),OrderState::FILLED);

}

TEST_F(OrderBookTest, Cancel)
{
    process_order(SellLimit(42,5,50));
    process_order(SellLimit(43,5,50));
    process_order(SellLimit(44,5,50));

    // All three orders were placed and accepted
    EXPECT_EQ(ob.depth(50),15);
    EXPECT_EQ(rd.accepted().size(),3);
    EXPECT_TRUE(rd.accepted().contains(43));

    process_order(Cancel(43,5,50));
    // the cancelled order
    // qty is subtracted from depth
    // state set to cancel
    // moved out of accepted and into executed
    EXPECT_EQ(ob.depth(50),10);
    EXPECT_EQ(rd.completed().size(),1);
    EXPECT_EQ(rd.completed().at(43).states.back(),OrderState::CANCELLED);
    EXPECT_EQ(rd.completed().at(43).quantities.back(),5);

    EXPECT_EQ(rd.accepted().size(),2);
    EXPECT_FALSE(rd.accepted().contains(43));

}

TEST_F(OrderBookTest,SelfConsistent_WithUnderflowCheck)
{
    size_t iterations{1000};

    //Set up
    OrderBook<Fifo> ob1(Instrument("x",100,49,50,2));
    gen::OrderGenerator og1;
    RecordDepot<Record> rd1;

    og1.initialize_orderbook(ob1,rd1,1000,25);

    for (int i{}; i < iterations; ++i)
    {
        /// we record each order as it gets processes
        ob1.submit_order(og1.record_order(og1.make_random_order(ob1, rd1,10)));
        ob1.accept_order();
        ob1.match_order();
        rd1.record_processed_orders(std::move(ob1.get_processed_orders()));
        rd1.update_order_records();

        /// for catching underflows
        for (auto& depth : ob1.depth(0,ob1.num_prices()-1))
            EXPECT_TRUE( depth < 10000);
    }


    std::unordered_map<ID,order::Record> accepted_1 = rd1.accepted();
    std::unordered_map<ID,order::Record> executed_1 = rd1.completed();

    // =========================================
    OrderBook<Fifo> ob2(Instrument("x",100,49,50,2));;
    RecordDepot<Record> rd2;
    gen::OrderGenerator og2;

    /// Now we play back the recorded orders and check that we end up with the same results
    while (!og1.recorded_orders.empty())
    {
        rd2.make_order_record(og1.recorded_orders.front());
        ob2.submit_order(og1.recorded_orders.front());
        og1.recorded_orders.pop();
        ob2.accept_order();
        ob2.match_order();
        rd2.record_processed_orders(std::move(ob2.get_processed_orders()));
        rd2.update_order_records();

    }


    std::unordered_map<ID,order::Record> accepted_2 = rd2.accepted();
    std::unordered_map<ID,order::Record> executed_2 = rd2.completed();


    for (auto& [id,r1] : accepted_1)
    {
        auto r2 = accepted_2.at(id);
        EXPECT_EQ(r1,r2);
    }

    for (auto& [id,r1] : executed_1)
    {
        auto r2 = executed_2.at(id);
        EXPECT_EQ(r1,r2);
    }

}