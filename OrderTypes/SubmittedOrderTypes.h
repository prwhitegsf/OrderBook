//
// Created by prw on 4/22/25.
//

#ifndef SUBMITTEDORDERTYPES_H
#define SUBMITTEDORDERTYPES_H

#include "Orders.h"
#include <chrono>
#include <iostream>


class SubmittedBuyLimit {
public:
    Order _;
    int total_qty_;

    Duration duration_;
    long expiry_;

    OrderState state_;


    SubmittedBuyLimit(int total_qty, int display_qty, double price, Duration duration, long expiry = 0)
        : _(std::chrono::utc_clock::now().time_since_epoch().count(),display_qty,price),
        total_qty_(total_qty),duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED){}


    SubmittedBuyLimit(int total_qty, double price, Duration duration, long expiry = 0)
        : _(std::chrono::utc_clock::now().time_since_epoch().count(),total_qty,price),
        total_qty_(total_qty),duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED){}


    BuyLimit make_queued_order() const
    {
        return BuyLimit(_);
    }

    void print() const
    {
        std::cout<< "BuyLimitOrder: "<< _.id_ << " State: " << OrderStateToString(state_) <<std::endl;
        std::cout << "Price: "<< _.price_ << "Show Qty: " << _.qty_ <<
                    " Total Qty: "<< total_qty_ << " Duration: " << DurationToString(duration_)<<
                    " Expiry: "<<expiry_<<std::endl;
    }
};

class SubmittedSellLimit {
public:
    Order _;
    int total_qty_;

    Duration duration_;
    long expiry_;

    OrderState state_;


    SubmittedSellLimit(int total_qty, int display_qty, double price, Duration duration, long expiry = 0)
        : _(std::chrono::utc_clock::now().time_since_epoch().count(),display_qty,price),
        total_qty_(total_qty),duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED){}


    SubmittedSellLimit(int total_qty, double price, Duration duration, long expiry = 0)
        : _(std::chrono::utc_clock::now().time_since_epoch().count(),total_qty,price),
        total_qty_(total_qty),duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED){}


    SellLimit make_queued_order() const
    {
        return SellLimit(_);
    }

    void print() const
    {
        std::cout<< "SellLimitOrder: "<< _.id_ << " State: " << OrderStateToString(state_) <<std::endl;
        std::cout << "Price: "<< _.price_ << "Show Qty: " << _.qty_ <<
                    " Total Qty: "<< total_qty_ << " Duration: " << DurationToString(duration_)<<
                    " Expiry: "<<expiry_<<std::endl;
    }
};

class SubmittedBuyMarket {
public:

    Order _;
    OrderState state_;

    explicit SubmittedBuyMarket(const int qty)
        : _(std::chrono::utc_clock::now().time_since_epoch().count(),qty,0),
        state_(OrderState::SUBMITTED){}

    BuyMarket make_queued_order() const { return BuyMarket(_); }

    void print() const
    {
        std::cout<< "BuyMarketOrder: "<< _.id_ << " State: " << OrderStateToString(state_) <<std::endl;
        std::cout << "Avg Fill Price: "<< _.price_ << "Show Qty: " << _.qty_ << std::endl;
    }
};

class SubmittedSellMarket {
public:

    Order _;
    OrderState state_;

    explicit SubmittedSellMarket(const int qty)
        : _(std::chrono::utc_clock::now().time_since_epoch().count(),qty,0),
        state_(OrderState::SUBMITTED){}

    SellMarket make_queued_order() const { return SellMarket(_); }

    void print() const
    {
        std::cout<< "SellMarketOrder: "<< _.id_ << " State: " << OrderStateToString(state_) <<std::endl;
        std::cout << "Avg Fill Price: "<< _.price_ << "Show Qty: " << _.qty_ << std::endl;
    }
};


class SubmittedCancel {
public:

    Order _;
    OrderState state_;
    ID cancel_id_;

    explicit SubmittedCancel(ID cancel_id, double price)
        : _(std::chrono::utc_clock::now().time_since_epoch().count(),0,price),
        cancel_id_(cancel_id),
        state_(OrderState::SUBMITTED){}

    Cancel make_queued_order() const { return Cancel(_, cancel_id_); }

    void print() const
    {
        std::cout<< "Cancel Order ID : "<< _.id_ << " State: " << OrderStateToString(state_) <<std::endl;
        std::cout<< "Order to be cancelled ID : "<< cancel_id_ << std::endl;
    }
};

#endif //SUBMITTEDORDERTYPES_H
