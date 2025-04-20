//
// Created by prw on 4/19/25.
//
#include "../OrderTypes/ClientOrders/ClientOrders.h"

#include "gtest/gtest.h"


TEST(ClientOrderTypeTests, BuyLimitOrderAccepted)
{

    long pre_id = std::chrono::utc_clock::now().time_since_epoch().count();

    BuyLimitOrder order = BuyLimitOrder(10,5,100.5,Duration::DAY);

    long post_id = std::chrono::utc_clock::now().time_since_epoch().count();

    EXPECT_LT(pre_id, order.id());
    EXPECT_GT(post_id, order.id());

    EXPECT_EQ(order.qty(),10);
    EXPECT_EQ(order.display(),5);
    EXPECT_EQ(order.price(),100.5);

    EXPECT_EQ("Day",DurationToString(order.good_until()));
    EXPECT_EQ("ACCEPTED",OrderStateToString(order.state()));


}

TEST(ClientOrderTypeTests, BuyLimitOrderRejected)
{

    long pre_id = std::chrono::utc_clock::now().time_since_epoch().count();

    BuyLimitOrder order = BuyLimitOrder(10,5,100.5,Duration::GTD);

    long post_id = std::chrono::utc_clock::now().time_since_epoch().count();

    EXPECT_LT(pre_id, order.id());
    EXPECT_GT(post_id, order.id());

    EXPECT_EQ(order.qty(),10);
    EXPECT_EQ(order.display(),5);
    EXPECT_EQ(order.price(),100.5);

    EXPECT_EQ("GTD",DurationToString(order.good_until()));
    EXPECT_EQ("REJECTED",OrderStateToString(order.state()));


}