//
// Created by prw on 8/12/25.


#include <vector>
#include <deque>

#include "Fifo.h"


using namespace order;

const order::Matched& Fifo::match(const BuyMarket o) { return market(o,std::plus<>()); };
const order::Matched& Fifo::match(const SellMarket o) { return market(o,std::minus<>()); }
const order::Matched& Fifo::match(const BuyMarketLimit o) { return market_limit(o,std::plus<>()); };
const order::Matched& Fifo::match(const SellMarketLimit o) { return market_limit(o,std::minus<>()); }
const order::Matched& Fifo::match(const BuyLimit o) { return limit(Limit(o));}
const order::Matched& Fifo::match(const SellLimit o) { return limit(Limit(o));}

const Level& Fifo::level(const size_t idx) const { return level_[idx]; }

double Fifo::fill_price(const Price price, const Qty filled_qty, const Qty full_qty) {
    return price * (filled_qty / static_cast<double>(full_qty));
}


const order::Matched& Fifo::match(const Cancel o)
{
    // if we find the order, update the depth and order state, otherwise throw
    if (!std::erase_if(level_[o.price].orders,[&](const Limit& ord){ return ord.id == o.id; }))
    {
        throw(std::invalid_argument("Cancelled order not found at price"));
    }

    matched_.clear();
    level_[o.price].depth -= o.qty;
    matched_.state_update = {o.id,OrderState::CANCELLED};

    return matched_;
}

const order::Matched& Fifo::match(const Rejected o)
{
    matched_.clear();
    matched_.state_update = {o.id,OrderState::REJECTED};
    return matched_;
}

const order::Matched& Fifo::limit(auto o)
{
    // Limit orders are just placed into the level
    matched_.clear();
    level_[o.price].orders.emplace_back(o);
    level_[o.price].depth += o.qty;
    matched_.state_update = {o.id,OrderState::ACCEPTED};
    return matched_;
}



const order::Matched& Fifo::market_limit(auto o, auto&& dir)
{
    matched_.clear();
    matched_.market_fill.id = o.id;
    matched_.market_fill.qty = o.qty; // our original qty for market portion
    matched_.market_fill.fill_price = 0;
    const Qty full_qty = o.qty + o.limit_qty;
    matched_.market_fill.limit = o.limit_price;

    // Market order takes all the available liquidity at current the level at once
    for (;o.price != o.limit_price; o.price = dir(o.price,1))
    {
        matched_.market_fill.fill_price += fill_price(o.price,level_[o.price].depth,full_qty);
        o.qty -= level_[o.price].depth;
        fill_level(o);
    }
    /*for (;o.qty > level_[o.price].depth; o.price = dir(o.price,1))
    {
        matched_.market_fill.fill_price += fill_price(o.price,level_[o.price].depth,full_qty);
        o.qty -= level_[o.price].depth;
        fill_level(o);
    }*/


    fill_level(o);
    matched_.market_fill.fill_price += fill_price(o.limit_price,o.qty+o.limit_qty,full_qty);

    // Remaining orders are filled at the current price
    /*matched_.market_fill.fill_price += matched_.market_fill.fill_price == 0 ?
        o.price : fill_price(o.price,o.qty + o.limit_qty,full_qty);*/
    level_[o.limit_price].orders.emplace_back(o.id,o.limit_qty,o.limit_price);

    level_[o.limit_price].depth = o.limit_qty;
    /*// take orders in full
    while (!level_[o.price].orders.empty() && o.qty > level_[o.price].orders.front().qty)
    {
        o.qty -=  level_[o.price].orders.front().qty;
        fill_orders(o);
    }

    // take partial order
    if (o.qty)
    {
        fill_remaining(o);
    }*/


    return matched_;
}

const order::Matched& Fifo::market(auto o, auto&& dir)
{
    matched_.clear();
    matched_.market_fill.id = o.id;
    matched_.market_fill.qty = o.qty;
    matched_.market_fill.fill_price = 0;
    // Market order takes all the available liquidity at current the level at once
    for (;o.qty > level_[o.price].depth; o.price = dir(o.price,1))
    {
        matched_.market_fill.fill_price +=
            fill_price(o.price,level_[o.price].depth,matched_.market_fill.qty);

        o.qty -= level_[o.price].depth;
        fill_level(o);
    }

    // Remaining orders are filled at the current price
    matched_.market_fill.fill_price += matched_.market_fill.fill_price == 0 ?
        o.price : fill_price(o.price,o.qty,matched_.market_fill.qty);

    matched_.market_fill.limit = o.price;

    // take orders in full
    while (!level_[o.price].orders.empty() && o.qty > level_[o.price].orders.front().qty)
    {
        o.qty -=  level_[o.price].orders.front().qty;
        fill_orders(o);
    }

    // take partial order
    if (o.qty)
    {
        fill_remaining(o);
    }

    return matched_;
}


int Fifo::fill_level(auto& o)
{
    level_[o.price].depth = 0;

    for (;!level_[o.price].orders.empty(); level_[o.price].orders.pop_front())
    {
        matched_.limit_fills.push_back(level_[o.price].orders.front().id);
    }

    return 1;
}

int Fifo::fill_orders(auto& o)
{
    level_[o.price].depth -= level_[o.price].orders.front().qty;

    matched_.limit_fills.push_back(level_[o.price].orders.front().id);
    level_[o.price].orders.pop_front();

    return 1;
}

int Fifo::fill_remaining(auto& o)
{
    level_[o.price].depth -= o.qty;
    level_[o.price].orders.front().qty -= o.qty;

    if (level_[o.price].orders.front().qty) // last limit order partially filled
    {
        matched_.partial_fill.id = level_[o.price].orders.front().id;
        matched_.partial_fill.qty = o.qty;

    }
    else // last limit order fully filled
    {
        matched_.limit_fills.push_back(level_[o.price].orders.front().id);
        level_[o.price].orders.pop_front();

    }

    o.qty = 0;
    return 1;
}



