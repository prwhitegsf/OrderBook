//
// Created by prw on 4/22/25.
//

#ifndef ORDERS_H
#define ORDERS_H

#include "Qualifiers.h"
#include "OrderRequirements.h"




using ID = unsigned int;
using Qty = unsigned short int;
using PriceIdx = unsigned short int;

struct Order {

    ID id_;
    Qty qty_;
    PriceIdx price_;

    Order(ID id, Qty qty,PriceIdx price)
    : id_(id), qty_(qty),price_(price){}


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
    float fill_;
    OrderState state_;

    OrderUpdate()
        : order({0,0,0}),fill_(0),state_(OrderState::SUBMITTED) {}

    OrderUpdate(ID id,Qty qty,PriceIdx price,OrderState state, float fill)
        : order({id, qty,price}), state_(state),fill_(fill){}

};


#endif //ORDERS_H
