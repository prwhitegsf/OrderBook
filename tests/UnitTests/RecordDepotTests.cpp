//
// Created by prw on 8/12/25.
//

#include "gtest/gtest.h"
#include "RecordDepot.h"
#include "OrderTypes.h"

#include "Record.h"


using namespace order;
class OrderRecordsTest : public ::testing::Test
{
public:
    RecordDepot<order::Record> ors;

};



TEST_F(OrderRecordsTest, MakeRecord)
{
    ors.make_order_record(BuyLimit(1,5,10));
    auto  o = ors.find_order_record(1);

    EXPECT_EQ(o.id,1);
    EXPECT_EQ(o.quantities.back(),5);
    EXPECT_EQ(o.limit_price,10);
    EXPECT_EQ(o.type,"BuyLimit");
}


TEST_F(OrderRecordsTest, StateUpdatesNoMove)
{
    ors.make_order_record(BuyLimit(1,5,9));
    ors.make_order_record(BuyLimit(2,10,10));
    ors.make_order_record(SellLimit(3,6,11));
    ors.make_order_record(SellLimit(4,11,12));

    for (int id{1}; id <= 4; ++id)
        EXPECT_EQ(OrderState::SUBMITTED, ors.accepted().at(id).states.back());

    std::vector<StateUpdate> state_updates({StateUpdate{1,OrderState::ACCEPTED}});


    ors.record_processed_orders({{},state_updates});
    ors.update_order_records();
    EXPECT_EQ(OrderState::ACCEPTED, ors.accepted().at(1).states.back());

    state_updates.back() = StateUpdate{2,OrderState::PENDING};
    ors.record_processed_orders({{},state_updates});
    ors.update_order_records();
    EXPECT_EQ(OrderState::PENDING, ors.accepted().at(2).states.back());


}


TEST_F(OrderRecordsTest,StateUpdateWithMove)
{
    // Moves the updated orders into the executed table
    // get_record(id) first looks in the pending table
    // then looks in the executed table
    // when they're no longer in the market
    // Rejected, Cancelled orders happen here
    // Fills are another function


    ors.make_order_record(BuyLimit(1,5,9));
    ors.make_order_record(BuyLimit(2,10,10));
    ors.make_order_record(SellLimit(3,6,11));
    ors.make_order_record(SellLimit(4,11,12));

    // confirming orderers in accepted with submitted state
    for (int id{1}; id <= 4; ++id)
        EXPECT_EQ(OrderState::SUBMITTED, ors.accepted().at(id).states.back());


    EXPECT_EQ(ors.accepted().size(),4); // everything is in the accepted table
    EXPECT_EQ(ors.completed().size(),0); // nothing in the executed table

    std::vector<StateUpdate> state_updates({
        StateUpdate{1,OrderState::CANCELLED},
        StateUpdate{2,OrderState::REJECTED},
        StateUpdate{3,OrderState::CANCELLED},
        StateUpdate{4,OrderState::REJECTED}});

    ors.record_processed_orders({{},state_updates});
    ors.update_order_records();

    EXPECT_EQ(ors.completed().size(),4); // now the orders are in the executed table
    EXPECT_EQ(ors.accepted().size(),0); // and removed from accepted

    EXPECT_EQ(OrderState::CANCELLED, ors.completed().at(1).states.back());
    EXPECT_EQ(OrderState::REJECTED, ors.completed().at(2).states.back());
    EXPECT_EQ(OrderState::CANCELLED, ors.completed().at(3).states.back());
    EXPECT_EQ(OrderState::REJECTED, ors.completed().at(4).states.back());


}


TEST_F(OrderRecordsTest, ProcessBuyMarket)
{
    constexpr ID id{1};
    ors.make_order_record(BuyMarket(id,10,20));
    EXPECT_EQ(ors.accepted().size(),1);

    std::vector<order::OrderFills> fills(1);

    fills.back().market_fill.id = id;
    fills.back().market_fill.qty = 10;
    fills.back().market_fill.fill_price = 20;

    ors.record_processed_orders({fills,{}});
    ors.update_order_records();


    EXPECT_EQ(ors.accepted().size(),0);
    EXPECT_EQ(ors.completed().size(),1);
    EXPECT_EQ(ors.completed().at(id).quantities.back(),0);
    EXPECT_EQ(ors.completed().at(id).quantities.front(),10);
    EXPECT_EQ(ors.completed().at(id).filled_price,20);
}

TEST_F(OrderRecordsTest, FindRecord)
{
    constexpr ID id{1};
    ors.make_order_record(BuyLimit(id,5,10));
    // find in accepted
    EXPECT_EQ(ors.accepted().size(),1);
    auto  o = ors.find_order_record(1); // finding in accepted

    EXPECT_EQ(o.id,id);

    std::vector<order::OrderFills> fills(1);
    fills.back().limit_fills.push_back(id);

    // unfound order
    auto ufo = ors.find_order_record(2);
    EXPECT_EQ(ufo.id,0);


}




TEST_F(OrderRecordsTest,SplitOrders)
{
    // market order gets split into market and limit portion
    // We can tell we did this on the back end because
    // when we process the market fills, we'll see that
    // the fill_qty is less than the qty in pending

    // Only testing from market: the process from
    // order records perspective is the same, it
    // executes a market portion, then the limit portion
    // so what ends up on the books is identical

    // simulate placement of original order
    constexpr ID id{1};
    ors.make_order_record(BuyMarket(id,10,20));
    EXPECT_EQ(ors.accepted().size(),1);

    EXPECT_EQ(ors.accepted().at(id).quantities.size(),1);
    EXPECT_EQ(ors.accepted().at(id).states.size(),1);

    // it's split: 8 filled at market, 5 @ 19, 3 @ 20
    // and two remain on the book as buy limits at 20
    std::vector<order::OrderFills> fills(1);
    fills.back().market_fill.id = id;
    fills.back().market_fill.qty = 8;
    fills.back().market_fill.fill_price = 19.5;

    ors.record_processed_orders({fills,{}});
    ors.update_order_records();
    fills.clear(); // we do this manually, ob provides its own cleanup

    EXPECT_EQ(ors.accepted().at(id).states.size(),2);
    EXPECT_EQ(ors.accepted().at(id).quantities.size(),2);

    EXPECT_EQ(ors.accepted().at(id).quantities.at(0),10);
    EXPECT_EQ(ors.accepted().at(id).states.at(0),OrderState::SUBMITTED);

    EXPECT_EQ(ors.accepted().at(id).quantities.at(1),2);
    EXPECT_EQ(ors.accepted().at(id).states.at(1),OrderState::PARTIAL);

    EXPECT_EQ(ors.accepted().at(id).filled_price,19.5);
    EXPECT_EQ(ors.completed().size(),0);


    // Now simulate the remaining order being filled
    fills.emplace_back();
    fills.back().market_fill.id = 50;
    fills.back().market_fill.qty = 2;
    fills.back().market_fill.fill_price = 20;
    fills.back().limit_fills.push_back(id);

    ors.record_processed_orders({fills,{}});
    ors.update_order_records();
    EXPECT_EQ(ors.last_processed().size(),2);


    // We expect the order to move to executed
    EXPECT_EQ(ors.accepted().size(),0);
    EXPECT_EQ(ors.completed().size(),2);

    // We should have 3 quantities and states
    EXPECT_EQ(ors.completed().at(id).states.size(),3);
    EXPECT_EQ(ors.completed().at(id).quantities.size(),3);

    EXPECT_EQ(ors.completed().at(id).quantities.at(0),10);
    EXPECT_EQ(ors.completed().at(id).states.at(0),OrderState::SUBMITTED);

    EXPECT_EQ(ors.completed().at(id).quantities.at(1),2);
    EXPECT_EQ(ors.completed().at(id).states.at(1),OrderState::PARTIAL);

    EXPECT_EQ(ors.completed().at(id).quantities.at(2),0);
    EXPECT_EQ(ors.completed().at(id).states.at(2),OrderState::FILLED);



    EXPECT_EQ(ors.completed().at(id).filled_price,19.5);
}



TEST_F(OrderRecordsTest,PartialFills)
{
    ors.make_order_record(BuyLimit(1,5,9));
    ors.make_order_record(BuyLimit(2,5,10));
    ors.make_order_record(SellLimit(3,5,11));
    ors.make_order_record(SellLimit(4,5,12));
    // order ids 1-4, all with qty of 5

    for (ID id{1}; id <= 4; ++id)
        EXPECT_EQ(5, ors.accepted().at(id).quantities.back());

    std::vector<order::OrderFills> fills(1);
    fills.back().market_fill.id = 50;
    fills.back().partial_fill.id = 1;
    fills.back().partial_fill.qty = static_cast<Qty>(1);

    ors.record_processed_orders({fills,{}});
    ors.update_order_records();

    EXPECT_EQ(4, ors.accepted().at(1).quantities.back());
    EXPECT_EQ(OrderState::PARTIAL, ors.accepted().at(1).states.back());

    fills.back().market_fill.id = 51;
    fills.back().partial_fill.id = 2;
    fills.back().partial_fill.qty = static_cast<Qty>(2);

    ors.record_processed_orders({fills,{}});
    ors.update_order_records();

    EXPECT_EQ(3, ors.accepted().at(2).quantities.back());
    EXPECT_EQ(OrderState::PARTIAL, ors.accepted().at(2).states.back());

    fills.back().market_fill.id = 52;
    fills.back().partial_fill.id = 3;
    fills.back().partial_fill.qty = static_cast<Qty>(3);

    ors.record_processed_orders({fills,{}});
    ors.update_order_records();

    EXPECT_EQ(2, ors.accepted().at(3).quantities.back());
    EXPECT_EQ(OrderState::PARTIAL, ors.accepted().at(3).states.back());


}
TEST_F(OrderRecordsTest,FilledLimitOrders)
{
    ors.make_order_record(BuyLimit(1,5,1));
    ors.make_order_record(BuyLimit(2,5,2));
    ors.make_order_record(SellLimit(3,5,3));
    ors.make_order_record(SellLimit(4,5,4));

    EXPECT_EQ(ors.accepted().size(),4);


    for (ID id{1}; id <= 4; ++id)
        EXPECT_EQ(5, ors.accepted().at(id).quantities.back());

    std::vector<order::OrderFills> fills(1);
    fills.back().market_fill.id = 50;

    for (ID id{1}; id  <= 4; ++id)
        fills.back().limit_fills.push_back(id);

    ors.record_processed_orders({fills,{}});
    ors.update_order_records();

    EXPECT_EQ(0,ors.accepted().size());
    EXPECT_EQ(5,ors.completed().size());

    for (ID id{1}; id  <= 4; ++id)
        EXPECT_EQ(id, ors.find_order_record(id).id);



    for (ID id{1}; id  <= 4; ++id)
    {
        EXPECT_EQ(0, ors.completed().at(id).quantities.back());
        EXPECT_EQ(id, ors.completed().at(id).filled_price);
        EXPECT_EQ(OrderState::FILLED, ors.completed().at(id).states.back());
    }
}