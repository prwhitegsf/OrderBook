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
    //inline ID from_ts(){ return std::chrono::utc_clock::now().time_since_epoch().count();}

    //inline ID from_start(auto& start){ return std::chrono::utc_clock::now().time_since_epoch().count() - start;}


    inline ID from_counter()
    {
        static unsigned int counter;
        counter++;
        return counter;
    }


}

template<Is_Core_Order O>
class SubmittedBuyLimit {
public:
    O order;

    Duration duration_;
    long expiry_;
    OrderState state_;
    float fill_{};
    Qty display_;


    SubmittedBuyLimit(Qty qty, PriceIdx price, Duration duration, long expiry = 0)
        : order(gen_id::from_counter(),qty,price),
        duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED),display_(qty){}


    SubmittedBuyLimit(Qty qty,Qty display, PriceIdx price, Duration duration, long expiry = 0)
    : order(gen_id::from_counter(),qty,price),
    duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED),display_(display){}



    BuyLimit<O> make_queued_order() const
    {
        return BuyLimit(order);
    }

    void print() const
    {
        std::cout<< "BuyLimitOrder: "<< order.id_ <<std::endl;
        std::cout << "State: " << OrderStateToString(state_) <<" | Price: "<< order.price_<<std::endl;
        std::cout << "Show Qty: " << order.qty_ ;
        std::cout<< "Duration: " << DurationToString(duration_)<<" | Expiry: "<<expiry_<<std::endl;
    }
};


template<Is_Core_Order O>
class SubmittedSellLimit {
public:
    O order;

    Duration duration_;
    long expiry_;
    OrderState state_;
    float fill_{};
    Qty display_;


    SubmittedSellLimit(Qty qty, PriceIdx price, Duration duration, long expiry = 0)
        : order(gen_id::from_counter(),qty,price),
        duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED),display_(qty){}


    SubmittedSellLimit(Qty qty,Qty display, PriceIdx price, Duration duration, long expiry = 0)
    : order(gen_id::from_counter(),qty,price),
    duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED),display_(display){}



    SellLimit<O> make_queued_order() const
    {
        return SellLimit(order);
    }

    void print() const
    {
        std::cout<< "SellLimitOrder: "<< order.id_ <<std::endl;
        std::cout << "State: " << OrderStateToString(state_) <<" | Price: "<< order.price_<<std::endl;
        std::cout << "Show Qty: " << order.qty_ ;
        std::cout<< "Duration: " << DurationToString(duration_)<<" | Expiry: "<<expiry_<<std::endl;
    }
};


template<Is_Core_Order O>
class SubmittedBuyMarket {
public:

    O order;
    OrderState state_;
    float fill_{};

    explicit SubmittedBuyMarket(Qty qty)
        : order(gen_id::from_counter(),qty,0),state_(OrderState::SUBMITTED){}

    BuyMarket<O> make_queued_order() const { return BuyMarket(order); }

    void print() const
    {
        std::cout<< "BuyMarketOrder: "<< order.id_ <<std::endl;
        std::cout << "State: " << OrderStateToString(state_) <<" | FillPrice: "<< fill_<<std::endl;
        std::cout << "Qty: " << order.qty_ <<std::endl;

    }
};

template<Is_Core_Order O>
class SubmittedSellMarket {
public:

    O order;
    OrderState state_;
    float fill_{};

    explicit SubmittedSellMarket(Qty qty)
        : order(gen_id::from_counter(),qty,0),state_(OrderState::SUBMITTED){}

    SellMarket<O> make_queued_order() const { return SellMarket(order); }

    void print() const
    {
        std::cout<< "SellMarketOrder: "<< order.id_ <<std::endl;
        std::cout << "State: " << OrderStateToString(state_) <<" | FillPrice: "<< fill_<<std::endl;
        std::cout << "Qty: " << order.qty_ <<std::endl;

    }
};

template<Is_Core_Order O>
class SubmittedCancel {
public:

    O order;
    ID cancel_id_;
    OrderState state_;

    explicit SubmittedCancel(ID cancel_id, PriceIdx price)
        : order(gen_id::from_counter(),0,price),cancel_id_(cancel_id),state_(OrderState::SUBMITTED){}

    Cancel<O> make_queued_order() const { return Cancel(order, cancel_id_); }

    void print() const
    {
        std::cout<< "CancelOrder: "<< order.id_ <<std::endl;
        std::cout << "State: " << OrderStateToString(state_) <<" | Price: "<< order.price_<<std::endl;
        std::cout<< "Order to be cancelled ID : "<< cancel_id_ << std::endl;

    }
};

#endif //SUBMITTEDORDERTYPES_H
