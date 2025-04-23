//
// Created by prw on 4/22/25.
//

#ifndef SUBMITTEDORDERTYPES_H
#define SUBMITTEDORDERTYPES_H

#include "Orders.h"
#include <chrono>
#include <iostream>


namespace gen_id
{
    inline ID from_ts(){ return std::chrono::utc_clock::now().time_since_epoch().count();}

}


class SubmittedBuyLimit {
public:
    Order order;

    Duration duration_;
    long expiry_;



    SubmittedBuyLimit(Qty qty, Qty total, PriceIdx price, Duration duration, long expiry = 0)
        : order(gen_id::from_ts(),qty,total,price,OrderState::SUBMITTED),
        duration_(duration),expiry_(expiry){}


    SubmittedBuyLimit(int qty, PriceIdx price, Duration duration, long expiry = 0)
    : order(gen_id::from_ts(),qty,qty,price,OrderState::SUBMITTED),
    duration_(duration),expiry_(expiry){}



    BuyLimit make_queued_order() const
    {
        return BuyLimit(order);
    }

    void print() const
    {
        std::cout<< "BuyLimitOrder: "<< order.id_ <<std::endl;
        std::cout << "State: " << OrderStateToString(order.state_) <<" | Price: "<< order.price_<<std::endl;
        std::cout << "Show Qty: " << order.qty_ <<" | Total Qty: "<< order.total_ <<std::endl;
        std::cout<< "Duration: " << DurationToString(duration_)<<" | Expiry: "<<expiry_<<std::endl;
    }
};

class SubmittedSellLimit {
public:
    Order order;


    Duration duration_;
    long expiry_;




    SubmittedSellLimit(Qty qty, Qty total, PriceIdx price, Duration duration, long expiry = 0)
        : order(gen_id::from_ts(),qty,total,price,OrderState::SUBMITTED),
        duration_(duration),expiry_(expiry){}


    SubmittedSellLimit(int qty, PriceIdx price, Duration duration, long expiry = 0)
    : order(gen_id::from_ts(),qty,qty,price,OrderState::SUBMITTED),
    duration_(duration),expiry_(expiry){}


    SellLimit make_queued_order() const
    {
        return SellLimit(order);
    }

    void print() const
    {
        std::cout<< "SellLimitOrder: "<< order.id_ <<std::endl;
        std::cout << "State: " << OrderStateToString(order.state_) <<" | Price: "<< order.price_<<std::endl;
        std::cout << "Show Qty: " << order.qty_ <<" | Total Qty: "<< order.total_ <<std::endl;
        std::cout<< "Duration: " << DurationToString(duration_)<<" | Expiry: "<<expiry_<<std::endl;
    }
};

class SubmittedBuyMarket {
public:

    Order order;


    explicit SubmittedBuyMarket(Qty qty)
        : order(gen_id::from_ts(),qty,qty,0,OrderState::SUBMITTED){}

    BuyMarket make_queued_order() const { return BuyMarket(order); }

    void print() const
    {
        std::cout<< "BuyMarketOrder: "<< order.id_ <<std::endl;
        std::cout << "State: " << OrderStateToString(order.state_) <<" | FillPrice: "<< order.price_<<std::endl;
        std::cout << "Qty: " << order.qty_ <<std::endl;

    }
};

class SubmittedSellMarket {
public:

    Order order;


    explicit SubmittedSellMarket(Qty qty)
     : order(gen_id::from_ts(),qty,qty,0,OrderState::SUBMITTED){}

    SellMarket make_queued_order() const { return SellMarket(order); }

    void print() const
    {
        std::cout<< "SellMarketOrder: "<< order.id_ <<std::endl;
        std::cout << "State: " << OrderStateToString(order.state_) <<" | FillPrice: "<< order.price_<<std::endl;
        std::cout << "Qty: " << order.qty_ <<std::endl;

    }
};


class SubmittedCancel {
public:

    Order order;
    ID cancel_id_;

    explicit SubmittedCancel(ID cancel_id, PriceIdx price)
        : order(gen_id::from_ts(),0,0,price,OrderState::SUBMITTED),cancel_id_(cancel_id){}

    Cancel make_queued_order() const { return Cancel(order, cancel_id_); }

    void print() const
    {
        std::cout<< "CancelOrder: "<< order.id_ <<std::endl;
        std::cout << "State: " << OrderStateToString(order.state_) <<" | Price: "<< order.price_<<std::endl;
        std::cout<< "Order to be cancelled ID : "<< cancel_id_ << std::endl;

    }
};

#endif //SUBMITTEDORDERTYPES_H
