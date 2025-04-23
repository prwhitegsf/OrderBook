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

    OrderState state_;
    long update_ts{};

    OrderUpdate()
        : order({0,0, 0}),state_(OrderState::SUBMITTED) {}
    OrderUpdate(ID id,double price,int qty,OrderState state)
        : order({id, qty, price}),state_(state) {}

};


#endif //ORDERS_H
