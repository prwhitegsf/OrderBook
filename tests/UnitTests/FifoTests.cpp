//
// Created by prw on 8/12/25.
//


#include "gtest/gtest.h"
#include "Fifo.h"

using namespace order;


/// Note that BuyMarket and SellMarket orders passed to fifo
/// expect the order.price variable to define the price they
/// begin executing at

class FifoTests : public ::testing::Test
{

public:
    Fifo fifo;

    FifoTests() : fifo(10){}

    template<class T>
    std::vector<T> make_limit_orders(std::vector<std::vector<int>>& quantities, const int start_price)
    {
        ID id{};
        std::vector<T> orders;

        for (int i{}; i < quantities.size(); ++i)
            for (auto& qty : quantities[i])
                orders.emplace_back(id++,qty,start_price + i);

        return orders;
    }
};

TEST_F(FifoTests,BuyLimit)
{

    auto o = fifo.match(BuyLimit(42,1,5));

    EXPECT_EQ(fifo.level(5).depth, 1);
    EXPECT_EQ(fifo.level(5).count(), 1);
    EXPECT_EQ(fifo.level(5).orders.front().id, 42);
    EXPECT_EQ(fifo.level(5).orders.front().qty, 1);
    EXPECT_EQ(o.id, 42);

}

TEST_F(FifoTests,SellLimit)
{
   // Fifo fifo(10);
    auto o = fifo.match(SellLimit(42,1,5));

    EXPECT_EQ(fifo.level(5).depth, 1);
    EXPECT_EQ(fifo.level(5).orders.front().id, 42);
    EXPECT_EQ(fifo.level(5).orders.front().qty, 1);
    EXPECT_EQ(o.id, 42);
}

TEST_F(FifoTests,BuyMarket)
{
  //  Fifo fifo(10);
    fifo.match(SellLimit(42,2,5));
    EXPECT_EQ(fifo.level(5).orders.front().id, 42);
    EXPECT_EQ(fifo.level(5).orders.front().qty, 2);

    auto fills = fifo.match(BuyMarket(43,2,4));

    EXPECT_EQ(fifo.level(5).depth, 0);
    EXPECT_EQ(fills.limit_fills.back(), 42);

    EXPECT_EQ(fills.market_fill.id, 43);
    EXPECT_EQ(fills.market_fill.qty, 2);
    EXPECT_EQ(fills.market_fill.fill_price, 5);

}

TEST_F(FifoTests,SellMarket)
{
  //  Fifo fifo(10);
    fifo.match(BuyLimit(42,2,5));
    EXPECT_EQ(fifo.level(5).orders.front().id, 42);
    EXPECT_EQ(fifo.level(5).orders.front().qty, 2);

    auto fills = fifo.match(SellMarket(43,2,6));

    EXPECT_EQ(fifo.level(5).depth, 0);

    EXPECT_EQ(fills.limit_fills.back(), 42);

    EXPECT_EQ(fills.market_fill.id, 43);
    EXPECT_EQ(fills.market_fill.qty, 2);
    EXPECT_EQ(fills.market_fill.fill_price, 5);

}


TEST_F(FifoTests,BuyMarketPartial)
{
   // Fifo fifo(10);
    fifo.match(SellLimit(42,2,5));
    EXPECT_EQ(fifo.level(5).orders.front().id, 42);
    EXPECT_EQ(fifo.level(5).orders.front().qty, 2);
    EXPECT_EQ(fifo.level(5).depth, 2);

    auto fills =fifo.match(BuyMarket(43,1,4));

    EXPECT_EQ(fifo.level(5).depth, 1);
    EXPECT_EQ(fifo.level(5).orders.front().qty, 1);
    EXPECT_EQ(fills.partial_fill.id, 42);
    EXPECT_EQ(fills.partial_fill.qty, 1);

    EXPECT_EQ(fills.market_fill.id, 43);
    EXPECT_EQ(fills.market_fill.qty, 1);
    EXPECT_EQ(fills.market_fill.fill_price, 5);

}


TEST_F(FifoTests,SellMarketPartial)
{
   // Fifo fifo(10);
    fifo.match(BuyLimit(42,2,5));
    EXPECT_EQ(fifo.level(5).orders.front().id, 42);
    EXPECT_EQ(fifo.level(5).orders.front().qty, 2);
    EXPECT_EQ(fifo.level(5).depth, 2);

     auto fills = fifo.match(SellMarket(43,1,6));

    EXPECT_EQ(fifo.level(5).depth, 1);
    EXPECT_EQ(fifo.level(5).orders.front().qty, 1);
    EXPECT_EQ(fills.partial_fill.id, 42);
    EXPECT_EQ(fills.partial_fill.qty, 1);

    EXPECT_EQ(fills.market_fill.id, 43);
    EXPECT_EQ(fills.market_fill.qty, 1);
    EXPECT_EQ(fills.market_fill.fill_price, 5);

}

TEST_F(FifoTests,BuyMarketSweep)
{
    fifo.match(SellLimit(1,2,5));
    fifo.match(SellLimit(2,2,5));
    fifo.match(SellLimit(3,2,6));
    fifo.match(SellLimit(4,2,6));
    fifo.match(SellLimit(5,2,7));
    fifo.match(SellLimit(6,2,7));

    // 2 orders for 2 contracts at each price
    for (int i{5}; i < 8; ++i)
    {
        EXPECT_EQ(fifo.level(i).depth, 4);
        EXPECT_EQ(fifo.level(i).orders.size(), 2);
        EXPECT_EQ(fifo.level(i).orders.front().qty, 2);
        EXPECT_EQ(fifo.level(i).orders.back().qty, 2);
    }

    auto fills = fifo.match(BuyMarket(42,8,5));

    // Buy Sweep takes all the orders from 5 and 6
    EXPECT_EQ(fifo.level(5).depth, 0);
    EXPECT_EQ(fifo.level(6).depth, 0);
    EXPECT_EQ(fifo.level(7).depth, 4);

    EXPECT_EQ(fills.market_fill.id, 42);
    EXPECT_EQ(fills.market_fill.qty, 8);
    EXPECT_EQ(fills.market_fill.fill_price, 5.5);

    EXPECT_EQ(fills.limit_fills.size(),4);
    // filled limit order ids are in the limit_fills container
    for (int i{1}; i < fills.limit_fills.size(); ++i)
        EXPECT_EQ(i, fills.limit_fills[i-1]);
}

TEST_F(FifoTests,SellMarketSweep)
{

    //Fifo fifo(10);
    fifo.match(BuyLimit(1,2,5));
    fifo.match(BuyLimit(2,2,5));
    fifo.match(BuyLimit(3,2,6));
    fifo.match(BuyLimit(4,2,6));
    fifo.match(BuyLimit(5,2,7));
    fifo.match(BuyLimit(6,2,7));

    for (int i{5}; i < 8; ++i)
    {
        EXPECT_EQ(fifo.level(i).depth, 4);
        EXPECT_EQ(fifo.level(i).orders.size(), 2);
        EXPECT_EQ(fifo.level(i).orders.front().qty, 2);
        EXPECT_EQ(fifo.level(i).orders.back().qty, 2);
    }

    auto fills = fifo.match(SellMarket(42,8,7));

    EXPECT_EQ(fifo.level(7).depth, 0);
    EXPECT_EQ(fifo.level(6).depth, 0);
    EXPECT_EQ(fifo.level(5).depth, 4);

    EXPECT_EQ(fills.market_fill.id, 42);
    EXPECT_EQ(fills.market_fill.qty, 8);
    EXPECT_EQ(fills.market_fill.fill_price, 6.5);

    std::vector<ID> expects {5,6,3,4};
    for (int i{1}; i < fills.limit_fills.size(); ++i)
        EXPECT_EQ(expects[i],fills.limit_fills[i]);
}


TEST_F(FifoTests,BuyMarketLimit)
{
    fifo.match(SellLimit(1,2,5));
    fifo.match(SellLimit(2,2,5));
    fifo.match(SellLimit(3,2,6));

    fifo.match(SellLimit(5,2,7));
    fifo.match(SellLimit(6,2,7));

    EXPECT_EQ(fifo.level(5).depth, 4);
    EXPECT_EQ(fifo.level(5).orders.size(), 2);
    EXPECT_EQ(fifo.level(5).orders.front().qty, 2);
    EXPECT_EQ(fifo.level(5).orders.back().qty, 2);

    EXPECT_EQ(fifo.level(6).depth, 2);
    EXPECT_EQ(fifo.level(6).orders.size(), 1);
    EXPECT_EQ(fifo.level(6).orders.front().qty, 2);

    auto fills = fifo.match(BuyMarketLimit(42,6,8,5,5));

    // BuyMarketLimit takes all the orders from 5 and 6
    EXPECT_EQ(fifo.level(5).depth, 0);
    EXPECT_EQ(fifo.level(6).depth, 0); // the next order will be a buy limit at this price
    EXPECT_EQ(fifo.level(7).depth, 4);

    EXPECT_EQ(fills.market_fill.id, 42);
    EXPECT_EQ(fills.market_fill.qty, 6);
    EXPECT_EQ(fills.market_fill.fill_price, 5.5);

    EXPECT_EQ(fills.limit_fills.size(),3);
    // filled limit order ids are in the limit_fills container
    for (int i{1}; i < fills.limit_fills.size(); ++i)
        EXPECT_EQ(i, fills.limit_fills[i-1]);
}

TEST_F(FifoTests,SellMarketLimit)
{
    fifo.match(BuyLimit(1,2,5));
    fifo.match(BuyLimit(2,2,5));
    fifo.match(BuyLimit(3,2,6));
    fifo.match(BuyLimit(5,2,7));
    fifo.match(BuyLimit(6,2,7));

    EXPECT_EQ(fifo.level(7).depth, 4);
    EXPECT_EQ(fifo.level(7).orders.size(), 2);
    EXPECT_EQ(fifo.level(7).orders.front().qty, 2);
    EXPECT_EQ(fifo.level(7).orders.back().qty, 2);

    EXPECT_EQ(fifo.level(6).depth, 2);
    EXPECT_EQ(fifo.level(6).orders.size(), 1);
    EXPECT_EQ(fifo.level(6).orders.front().qty, 2);

    auto fills = fifo.match(SellMarketLimit(42,6,8,7,7));

    // SellMarketLimit takes all the orders from 5 and 6
    EXPECT_EQ(fifo.level(7).depth, 0);
    EXPECT_EQ(fifo.level(6).depth, 0); // the next order will be a sell limit at this price
    EXPECT_EQ(fifo.level(5).depth, 4);

    EXPECT_EQ(fills.market_fill.id, 42);
    EXPECT_EQ(fills.market_fill.qty, 6);
    EXPECT_EQ(fills.market_fill.fill_price, 6.5);

    EXPECT_EQ(fills.limit_fills.size(),3);
    // filled limit order ids are in the limit_fills container

    std::vector<ID> expects {5,6,3};
    for (int i{}; i < fills.limit_fills.size(); ++i)
        EXPECT_EQ(expects[i], fills.limit_fills[i]);
}

TEST_F(FifoTests,Cancel)
{
   // Fifo fifo(10);

    std::vector<std::vector<int>> qtys {{2,3,4}};
    int start{5};
    std::vector<SellLimit> orders = make_limit_orders<SellLimit>(qtys,start);

    for (auto ord : orders)
        fifo.match(ord);

    EXPECT_EQ(fifo.level(5).depth, 9);

    auto o = fifo.match(Cancel(1,3,5));
    EXPECT_EQ(fifo.level(5).depth, 6);
    EXPECT_EQ(o.id,1);
}


