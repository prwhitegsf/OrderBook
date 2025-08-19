//
// Created by prw on 8/12/25.
//


//
// Created by prw on 8/7/25.
//


#include "Record.h"


using namespace order;

void Record::update(const PartialFill o, const Time& ts)
{
    quantities.push_back(quantities.back()-o.qty);
    states.push_back(OrderState::PARTIAL);
    timestamps.push_back(ts);
    if (!filled_price)
        filled_price = limit_price;
}

void Record::update(const Time& ts)
{
    quantities.push_back(0);
    states.push_back(OrderState::FILLED);
    timestamps.push_back(ts);
    if (!filled_price)
        filled_price = limit_price;
}

bool Record::is_market_to_limit(const Qty fill_qty) const
{
    return quantities.front() > fill_qty;
}

void Record::update_market_limit(const MarketFill& o, const Time& ts)
{
    quantities.push_back(quantities.back() - o.qty);
    states.push_back(OrderState::PARTIAL);
    timestamps.push_back(ts);
    filled_price = o.fill_price;
    limit_price = o.limit;
    type = type.starts_with("Buy") ? "BuyMarketLimit" : "SellMarketLimit";
}

void Record::update(const MarketFill& o, const Time& ts)
{
    quantities.push_back(0);
    states.push_back(OrderState::FILLED);
    timestamps.push_back(ts);
    filled_price = o.fill_price;
    if (!limit_price)
        limit_price = o.limit;

    if (type == "BuyLimit")
        type= "BuyMarketLimit";
    else if (type == "SellLimit")
        type = "SellMarketLimit";
}


void Record::update(const OrderState state, const Time& ts)
{
    quantities.push_back(quantities.back());
    states.push_back(state);
    timestamps.push_back(ts);
}


Time Record::timestamp() { return {Clock::now()};}

Record::Record():  type("empty"), id(0), limit_price(0), filled_price(0),
                   quantities(1,0), states({1,OrderState::EMPTY}),
                   timestamps({timestamp()}) {
}

