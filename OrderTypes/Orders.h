//
// Created by prw on 4/22/25.
//

#ifndef ORDERS_H
#define ORDERS_H

#include "Qualifiers.h"

using ID = unsigned long;
using Qty = unsigned short int;
using PriceIdx = unsigned short int;

struct Order {

    ID id_;
    Qty qty_;
    Qty total_;
    PriceIdx price_;
    float fill_price_{};
    OrderState state_;


    Order(ID id, Qty qty, Qty total, PriceIdx price, OrderState state)
    : id_(id), qty_(qty),total_(total) ,price_(price),state_(state){}

};


// Queued Order Types wrap order

struct QueuedOrderTag{};

struct BuyLimit : private QueuedOrderTag
{
    Order order;
    explicit BuyLimit(Order order) : order(order){}
};

struct SellLimit : private QueuedOrderTag
{
    Order order;
    explicit SellLimit(Order order) : order(order){}
};

struct BuyMarket : private QueuedOrderTag
{
    Order order;
    explicit BuyMarket(Order order) : order(order){}
};

struct SellMarket : private QueuedOrderTag
{
    Order order;
    explicit SellMarket(Order order) : order(order){}
};

struct Cancel : private QueuedOrderTag
{
    Order order;
    ID cancel_id_;
    explicit Cancel(Order order, ID cancel_id)
    : order(order),cancel_id_(cancel_id){}
};

// update object, returned from order book and sent to client order list

struct OrderUpdate {

    Order order;
    long update_ts{};

    OrderUpdate()
        : order({0,0,0,0,OrderState::SUBMITTED}) {}

    OrderUpdate(ID id,Qty qty, Qty total, PriceIdx price,OrderState state)
        : order({id, qty, total, price, state}){}

};


#endif //ORDERS_H
