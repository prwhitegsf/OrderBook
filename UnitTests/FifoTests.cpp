//
// Created by prw on 4/24/25.
//
#include "gtest/gtest.h"
#include <random>
#include <vector>
#include "../OrderTypes/SubmittedOrderTypes.h"
#include "../PriceLevels/DequeLevel/DequeLevel.h"
#include "../Matchers/FIFO/FifoMatchingStrategy.h"
#include "../DOMS/MidLadder/MidLadder.h"
#include "Printer.h"



using Fifo = FifoMatchingStrategy<MidLadder<Order,DequeLevel>>;
class FifoTest : public ::testing::Test
{


public:

    Print print;
    Fifo f;
    MidLadder<Order,DequeLevel> dom;


    static BuyLimit<Order> make_buy_limit(ID id, Qty qty, PriceIdx price)
    {
        Order o{id, qty, price};
        return BuyLimit(o);
    }

    static SellLimit<Order> make_sell_limit(ID id, Qty qty, PriceIdx price)
    {
        Order o{id, qty, price};
        return SellLimit(o);
    }

    static BuyMarket<Order> make_buy_market(ID id, Qty qty)
    {
        Order o{id, qty,0};
        return BuyMarket(o);
    }

    static SellMarket<Order> make_sell_market(ID id, Qty qty)
    {
        Order o{id, qty,0};
        return SellMarket(o);
    }

    static Cancel<Order> make_cancel(ID cancel_id, PriceIdx price)
    {
        Order o{1000, 0,price};
        return Cancel<Order>(o, cancel_id);
    }

    void generate_dom_orders(size_t bid, size_t ask, std::vector<int> level_depths)
    {
        if (dom.num_prices() < level_depths.size())
            return;

        dom.set_bid(bid);
        dom.set_ask(ask);

        ID id{1000};
        size_t i = bid;
        for(const int depth : level_depths)
        {
            int j{depth};
            while (j > 0)
            {
                SubmittedBuyLimit<Order> buy(1,i,Duration::DAY);
                f.match(make_buy_limit(id,1,i),dom);
                ++id;
                --j;
            }
            --i;
        }

        i = ask;
        for (const int depth : level_depths)
        {
            int j{depth};
            while (j > 0) {

                f.match(make_sell_limit(id,1,i),dom);
                ++id;
                --j;
            }

            ++i;
        }

    }

    void generate_default_dom()
    {
        std::vector<int> depths{5,4,3,2,1};
        size_t bid = 50;
        size_t ask = 51;
        generate_dom_orders(bid,ask,depths);
    }
};


/*
TEST_F(FifoTest,Construct)
{
    Fifo fifo(100);
    EXPECT_EQ(fifo.num_prices(),100);

    auto it = std::begin(fifo);
    while (it != std::end(fifo))
    {
        EXPECT_EQ(it->depth_, 0);
        ++it;
    }
}
*/

TEST_F(FifoTest,PlaceNewBuyLimit)
{


    f.match(make_buy_limit(12,10,50),dom);

    EXPECT_EQ((std::begin(dom.dom())+50)->depth_,10);

}


TEST_F(FifoTest,PlaceNewSellLimit)
{


    f.match(make_sell_limit(12,10,50),dom);

    EXPECT_EQ((std::begin(dom.dom())+50)->depth_,10);

}


TEST_F(FifoTest,DomGeneration)
{
    std::vector<int> depths{5,4,3,2,1};
    size_t bid = 50;
    size_t ask = 51;
    generate_dom_orders(bid,ask,depths);

    // Bid/Ask
    EXPECT_EQ(dom.ask(),51);
    EXPECT_EQ(dom.bid(),50);

    //const auto& dom = dom.dom();
    size_t idx = dom.num_prices();

    for (size_t i{}; i < depths.size(); ++i)
    {
        EXPECT_EQ(dom.dom().at(dom.ask() + i).depth_,depths.at(i));
        EXPECT_EQ(dom.dom().at(dom.bid() - i).depth_,depths.at(i));
    }

}

TEST_F(FifoTest,SimpleBuyMarket)
{
    generate_default_dom();


    EXPECT_EQ(dom.ask(),51);
    EXPECT_EQ(dom.bid(),50);

    EXPECT_EQ(dom.dom().at(dom.ask()).depth_,5);

    f.match(make_buy_market(1, 1),dom);
    EXPECT_EQ(dom.dom().at(dom.ask()).depth_,4);

    f.match(make_buy_market(2, 3), dom);
    EXPECT_EQ(dom.ask(),51);
    EXPECT_EQ(dom.dom().at(dom.ask()).depth_,1);

}

TEST_F(FifoTest,SimpleSellMarket)
{
    generate_default_dom();

    EXPECT_EQ(dom.ask(),51);
    EXPECT_EQ(dom.bid(),50);

    EXPECT_EQ(dom.dom().at(dom.bid()).depth_,5);

    f.match(make_sell_market(1, 1),dom);
    EXPECT_EQ(dom.dom().at(dom.bid()).depth_,4);

    f.match(make_sell_market(2, 3),dom);
    EXPECT_EQ(dom.bid(),50);
    EXPECT_EQ(dom.dom().at(dom.bid()).depth_,1);
}


TEST_F(FifoTest,OrderUpdate)
{
    //Fifo fifo(100);

    f.match(make_sell_limit(12,10,50),dom);

    EXPECT_EQ(f.order_updates().back().order.id_,12);
    EXPECT_EQ(f.order_updates().back().order.qty_,10);
    EXPECT_EQ(f.order_updates().back().order.price_,50);

    f.match(make_sell_limit(42,17,51),dom);

    EXPECT_EQ(f.order_updates().back().order.id_,42);
    EXPECT_EQ(f.order_updates().back().order.qty_,17);
    EXPECT_EQ(f.order_updates().back().order.price_,51);

    EXPECT_EQ(f.order_updates().front().order.id_,12);
    EXPECT_EQ(f.order_updates().front().order.qty_,10);
    EXPECT_EQ(f.order_updates().front().order.price_,50);


}

TEST_F(FifoTest,CancelOrder)
{
    std::vector<int> depths{5,4,3,2,1};
    size_t bid = 50;
    size_t ask = 51;
    generate_dom_orders(bid,ask,depths);

    EXPECT_EQ(dom.dom().at(dom.ask()).depth_,5);

    OrderUpdate sell_order = f.match(make_sell_limit(4200, 10,51),dom);

    EXPECT_EQ(dom.dom().at(dom.ask()).depth_,15);

    f.match(make_cancel(4200,51),dom);

    EXPECT_EQ(dom.dom().at(dom.ask()).depth_,5);

}

TEST_F(FifoTest,BuySweepMarket)
{
    std::vector<int> depths{3,4,3,2,1};
    size_t bid = 50;
    size_t ask = 51;
    generate_dom_orders(bid,ask,depths);

    EXPECT_EQ(dom.ask(),51);
    EXPECT_EQ(dom.bid(),50);

    EXPECT_EQ(dom.dom().at(dom.ask()).depth_,3);

    f.match(make_buy_market(42, 6),dom);
    EXPECT_EQ(dom.dom().at(51).depth_,0);
    EXPECT_EQ(dom.dom().at(52).depth_,1);
    EXPECT_EQ(dom.ask(),52);

    float avg_fill = f.order_updates().back().fill_;
    EXPECT_EQ(avg_fill,51.5);
}

TEST_F(FifoTest,SellSweepMarket)
{
    std::vector<int> depths{3,4,3,2,1};
    size_t bid = 50;
    size_t ask = 51;
    generate_dom_orders(bid,ask,depths);

    EXPECT_EQ(dom.ask(),51);
    EXPECT_EQ(dom.bid(),50);

    EXPECT_EQ(dom.dom().at(dom.bid()).depth_,3);

    f.match(make_sell_market(1, 6),dom);
    EXPECT_EQ(dom.dom().at(50).depth_,0);
    EXPECT_EQ(dom.dom().at(49).depth_,1);
    EXPECT_EQ(dom.bid(),49);

    float avg_fill = f.order_updates().back().fill_;
    EXPECT_EQ(avg_fill,49.5);
}

TEST_F(FifoTest,BuyLimitAboveBid)
{
    // Raising the bid, but not executing at market
    std::vector<int> depths{5,4,3,2,1};
    size_t bid = 50;
    size_t ask = 60;
    generate_dom_orders(bid,ask,depths);

    EXPECT_EQ(dom.bid(),50);
    EXPECT_EQ(dom.dom().at(55).depth_,0);

    f.match(make_buy_limit(1, 10,55),dom);

    EXPECT_EQ(dom.bid(),55);
    EXPECT_EQ(dom.dom().at(55).depth_,10);

}

TEST_F(FifoTest,SellLimitBelowAsk)
{
    std::vector<int> depths{5,4,3,2,1};
    size_t bid = 50;
    size_t ask = 60;
    generate_dom_orders(bid,ask,depths);

    EXPECT_EQ(dom.ask(),60);
    EXPECT_EQ(dom.dom().at(55).depth_,0);

    f.match(make_sell_limit(1, 10,55),dom);

    EXPECT_EQ(dom.ask(),55);
    EXPECT_EQ(dom.dom().at(55).depth_,10);
}

TEST_F(FifoTest,BuyLimitAtAsk)
{

    std::vector<int> depths{3,4,3,2,1};
    size_t bid = 50;
    size_t ask = 51;
    generate_dom_orders(bid,ask,depths);


    EXPECT_EQ(dom.bid(),50);
    f.match(make_buy_limit(42, 2,51),dom);

    // Does not lift bid
    EXPECT_EQ(dom.bid(),50);
    EXPECT_EQ(dom.dom().at(51).depth_,1);

    float avg_fill = f.order_updates().back().fill_;
    EXPECT_EQ(avg_fill,51);

}

TEST_F(FifoTest,SellLimitAtBid)
{

    std::vector<int> depths{3,4,3,2,1};
    size_t bid = 50;
    size_t ask = 51;
    generate_dom_orders(bid,ask,depths);

    EXPECT_EQ(dom.ask(),51);

    f.match(make_sell_limit(42, 2,50),dom);

    EXPECT_EQ(dom.ask(),51);
    EXPECT_EQ(dom.dom().at(50).depth_,1);

    float avg_fill = f.order_updates().back().fill_;
    EXPECT_EQ(avg_fill,50);

}

TEST_F(FifoTest, BuyLimitReplaceAsk)
{
    std::vector<int> depths{3,4,3,2,1};
    size_t bid = 50;
    size_t ask = 51;
    generate_dom_orders(bid,ask,depths);


    EXPECT_EQ(dom.bid(),50);
    f.match(make_buy_limit(42, 6,51),dom);

    // Take all liquidity at ask, then place limit order for the rest
    // lifts bid and ask
    EXPECT_EQ(dom.bid(),51);
    EXPECT_EQ(dom.ask(),52);
    EXPECT_EQ(dom.dom().at(51).depth_,3);

    float avg_fill = f.order_updates().back().fill_;
    EXPECT_EQ(avg_fill,51);

}

TEST_F(FifoTest,SellLimitReplaceBid)
{
    std::vector<int> depths{3,4,3,2,1};
    size_t bid = 50;
    size_t ask = 51;
    generate_dom_orders(bid,ask,depths);


    EXPECT_EQ(dom.ask(),51);
    f.match(make_sell_limit(42, 6,50),dom);

    // Take all liquidity at ,bid then place limit order for the rest
    // drops bid and ask
    EXPECT_EQ(dom.bid(),49);
    EXPECT_EQ(dom.ask(),50);
    EXPECT_EQ(dom.dom().at(50).depth_,3);

    float avg_fill = f.order_updates().back().fill_;
    EXPECT_EQ(avg_fill,50);

}

TEST_F(FifoTest,BuyLimitAboveAsk)
{
    std::vector<int> depths{2,2,2,2,15,3};
    size_t bid = 50;
    size_t ask = 51;
    generate_dom_orders(bid,ask,depths);


    EXPECT_EQ(dom.bid(),50);
    f.match(make_buy_limit(42, 8,53),dom);

    // Take all liquidity at ask, then place limit order for the rest
    // lifts bid and ask
    EXPECT_EQ(dom.bid(),53);
    EXPECT_EQ(dom.ask(),54);
    EXPECT_EQ(dom.dom().at(51).depth_,0);
    EXPECT_EQ(dom.dom().at(52).depth_,0);
    EXPECT_EQ(dom.dom().at(53).depth_,2);

    float avg_fill = f.order_updates().back().fill_;
    EXPECT_EQ(avg_fill,52.25);
}

TEST_F(FifoTest, SellLimitBelowBid)
{
    std::vector<int> depths{2,2,2,2,15,3};
    size_t bid = 50;
    size_t ask = 51;
    generate_dom_orders(bid,ask,depths);


    EXPECT_EQ(dom.ask(),51);
    f.match(make_sell_limit(42, 8,48),dom);

    // Take all liquidity at ask, then place limit order for the rest
    // lifts bid and ask
    EXPECT_EQ(dom.bid(),47);
    EXPECT_EQ(dom.ask(),48);
    EXPECT_EQ(dom.dom().at(50).depth_,0);
    EXPECT_EQ(dom.dom().at(49).depth_,0);
    EXPECT_EQ(dom.dom().at(48).depth_,2);

    float avg_fill = f.order_updates().back().fill_;
    EXPECT_EQ(avg_fill,48.75);
}