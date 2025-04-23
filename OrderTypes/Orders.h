//
// Created by prw on 4/22/25.
//

#ifndef ORDERS_H
#define ORDERS_H

#include "Qualifiers.h"

using ID = unsigned long;



struct Order
{
  ID id_;
  int qty_;
  double price_;

  Order(ID id, int qty, double price)
    : id_(id), qty_(qty), price_(price){}

};


// Queued Order Types wrap order

struct QueuedOrderTag{};

struct BuyLimit : private QueuedOrderTag
{
    Order _;
    explicit BuyLimit(Order order) : _(order){}
};

struct SellLimit : private QueuedOrderTag
{
    Order _;
    explicit SellLimit(Order order) : _(order){}
};

struct BuyMarket : private QueuedOrderTag
{
    Order _;
    explicit BuyMarket(Order order) : _(order){}
};

struct SellMarket : private QueuedOrderTag
{
    Order _;
    explicit SellMarket(Order order) : _(order){}
};

struct Cancel : private QueuedOrderTag
{
    Order _;
    ID cancel_id_;
    explicit Cancel(Order order, ID cancel_id)
    : _(order),cancel_id_(cancel_id){}
};

// update object, returned from order book and sent to client order list

struct OrderUpdate {

    Order _;

    OrderState state_;
    long update_ts{};

    OrderUpdate()
        : _({0,0, 0}),state_(OrderState::SUBMITTED) {}
    OrderUpdate(ID id,double price,int qty,OrderState state)
        : _({id, qty, price}),state_(state) {}

};


#endif //ORDERS_H
