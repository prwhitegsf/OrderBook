//
// Created by prw on 8/12/25.
//
//
// Created by prw on 7/28/25.
//

#include "Fifo.h"
#include <vector>
#include <deque>
#include <iostream>

using namespace order;

OrderFills Fifo::match(BuyMarket o) { return market(o,std::plus<>()); };
OrderFills Fifo::match(SellMarket o) { return market(o,std::minus<>()); }
OrderFills Fifo::match(BuyMarketLimit o) { return market_limit(o,std::plus<>()); };
OrderFills Fifo::match(SellMarketLimit o) { return market_limit(o,std::minus<>()); }
StateUpdate Fifo::match(BuyLimit o) { return limit(Limit(o));}
StateUpdate Fifo::match(SellLimit o) { return limit(Limit(o));}

const Level& Fifo::level(const size_t idx) const { return level_[idx]; }

float Fifo::fill_price(Price price, Qty filled_qty, Qty full_qty) {
    return price * (filled_qty / static_cast<float>(full_qty));
}


StateUpdate Fifo::match(Cancel o)
{
    // if we find the order, update the depth and order state, otherwise throw
    if (!std::erase_if(level_[o.price].orders,[&](const Limit& ord){ return ord.id == o.id; }))
    {
        throw(std::invalid_argument("Cancelled order not found at price"));
    }

    level_[o.price].depth -= o.qty;
    return StateUpdate{o.id,OrderState::CANCELLED};
}

StateUpdate Fifo::match(Rejected o)
{
    return StateUpdate{o.id,OrderState::REJECTED};
}

StateUpdate Fifo::limit(auto o)
{
    // Limit orders are just placed into the level
    level_[o.price].orders.emplace_back(o);
    level_[o.price].depth += o.qty;
    return StateUpdate{o.id,OrderState::ACCEPTED};
}



OrderFills Fifo::market_limit(auto o, auto&& dir)
{
    OrderFills fills;
    fills.market_fill.id = o.id;
    fills.market_fill.qty = o.qty; // our original qty for market portion
    const Qty full_qty = o.qty + o.limit_qty;
    fills.market_fill.limit = o.limit_price;

    // Market order takes all the available liquidity at current the level at once
    for (;o.qty > level_[o.price].depth; o.price = dir(o.price,1))
    {
        fills.market_fill.fill_price += fill_price(o.price,level_[o.price].depth,full_qty);
        o.qty -= level_[o.price].depth;
        fill_level(o, fills);
    }

    // Remaining orders are filled at the current price
    fills.market_fill.fill_price += fills.market_fill.fill_price == 0 ?
        o.price : fill_price(o.price,o.qty + o.limit_qty,full_qty);

    // take orders in full
    while (!level_[o.price].orders.empty() && o.qty > level_[o.price].orders.front().qty)
    {
        o.qty -=  level_[o.price].orders.front().qty;
        fill_orders(o, fills);
    }

    // take partial order
    if (o.qty)
    {
        fill_remaining(o, fills);
    }

    return std::move(fills);
}

OrderFills Fifo::market(auto o, auto&& dir)
{
    OrderFills fills;
    fills.market_fill.id = o.id;
    fills.market_fill.qty = o.qty;

    // Market order takes all the available liquidity at current the level at once
    for (;o.qty > level_[o.price].depth; o.price = dir(o.price,1))
    {
        fills.market_fill.fill_price +=
            fill_price(o.price,level_[o.price].depth,fills.market_fill.qty);

        o.qty -= level_[o.price].depth;
        fill_level(o, fills);
    }

    // Remaining orders are filled at the current price
    fills.market_fill.fill_price += fills.market_fill.fill_price == 0 ?
        o.price : fill_price(o.price,level_[o.price].depth,fills.market_fill.qty);

    fills.market_fill.limit = o.price;

    // take orders in full
    while (!level_[o.price].orders.empty() && o.qty > level_[o.price].orders.front().qty)
    {
        o.qty -=  level_[o.price].orders.front().qty;
        fill_orders(o, fills);
    }

    // take partial order
    if (o.qty)
    {
        fill_remaining(o, fills);
    }

    return std::move(fills);
}


int Fifo::fill_level(auto& o, OrderFills& fills)
{
    level_[o.price].depth = 0;

    for (;!level_[o.price].orders.empty(); level_[o.price].orders.pop_front())
    {
        fills.limit_fills.push_back(level_[o.price].orders.front().id);
    }

    return 1;
}

int Fifo::fill_orders(auto& o, OrderFills& fills)
{
    level_[o.price].depth -= level_[o.price].orders.front().qty;

    fills.limit_fills.push_back(level_[o.price].orders.front().id);
    level_[o.price].orders.pop_front();

    return 1;
}

int Fifo::fill_remaining(auto& o, OrderFills& fills)
{
    level_[o.price].depth -= o.qty;
    level_[o.price].orders.front().qty -= o.qty;

    if (level_[o.price].orders.front().qty) // last limit order partially filled
    {
        fills.partial_fill.id = level_[o.price].orders.front().id;
        fills.partial_fill.qty = o.qty;

    }
    else // last limit order fully filled
    {
        fills.limit_fills.push_back(level_[o.price].orders.front().id);
        level_[o.price].orders.pop_front();

    }

    o.qty = 0;
    return 1;
}



