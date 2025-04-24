//
// Created by prw on 4/22/25.
//

#ifndef ORDERS_H
#define ORDERS_H

#include "Qualifiers.h"

template<typename O>
concept Is_Core_Order = requires(O o)
{
    o.id_;
    o.qty_;
    o.total_;
    o.price_;
    o.state_;

};


using ID = unsigned int;
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

template<Is_Core_Order O>
struct BuyLimit : private QueuedOrderTag
{
    O order;
    explicit BuyLimit(O order) : order(order){}
};

template<Is_Core_Order O>
struct SellLimit : private QueuedOrderTag
{
    O order;
    explicit SellLimit(O order) : order(order){}
};

template<Is_Core_Order O>
struct BuyMarket : private QueuedOrderTag
{
    O order;
    explicit BuyMarket(O order) : order(order){}
};

template<Is_Core_Order O>
struct SellMarket : private QueuedOrderTag
{
    O order;
    explicit SellMarket(O order) : order(order){}
};

template<Is_Core_Order O>
struct Cancel : private QueuedOrderTag
{
    O order;
    ID cancel_id_;
    explicit Cancel(O order, ID cancel_id)
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
