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
    Order order;
    int total_qty_;

    Duration duration_;
    long expiry_;

    OrderState state_;


    SubmittedBuyLimit(int total_qty, int display_qty, double price, Duration duration, long expiry = 0)
        : order(std::chrono::utc_clock::now().time_since_epoch().count(),display_qty,price),
        total_qty_(total_qty),duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED){}


    SubmittedBuyLimit(int total_qty, double price, Duration duration, long expiry = 0)
        : order(std::chrono::utc_clock::now().time_since_epoch().count(),total_qty,price),
        total_qty_(total_qty),duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED){}


    BuyLimit make_queued_order() const
    {
        return BuyLimit(order);
    }

    void print() const
    {
        std::cout<< "BuyLimitOrder: "<< order.id_ << " State: " << OrderStateToString(state_) <<std::endl;
        std::cout << "Price: "<< order.price_ << "Show Qty: " << order.qty_ <<
                    " Total Qty: "<< total_qty_ << " Duration: " << DurationToString(duration_)<<
                    " Expiry: "<<expiry_<<std::endl;
    }
};

class SubmittedSellLimit {
public:
    Order order;
    int total_qty_;

    Duration duration_;
    long expiry_;

    OrderState state_;


    SubmittedSellLimit(int total_qty, int display_qty, double price, Duration duration, long expiry = 0)
        : order(std::chrono::utc_clock::now().time_since_epoch().count(),display_qty,price),
        total_qty_(total_qty),duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED){}


    SubmittedSellLimit(int total_qty, double price, Duration duration, long expiry = 0)
        : order(std::chrono::utc_clock::now().time_since_epoch().count(),total_qty,price),
        total_qty_(total_qty),duration_(duration),expiry_(expiry),state_(OrderState::SUBMITTED){}


    SellLimit make_queued_order() const
    {
        return SellLimit(order);
    }

    void print() const
    {
        std::cout<< "SellLimitOrder: "<< order.id_ << " State: " << OrderStateToString(state_) <<std::endl;
        std::cout << "Price: "<< order.price_ << "Show Qty: " << order.qty_ <<
                    " Total Qty: "<< total_qty_ << " Duration: " << DurationToString(duration_)<<
                    " Expiry: "<<expiry_<<std::endl;
    }
};

class SubmittedBuyMarket {
public:

    Order order;
    OrderState state_;

    explicit SubmittedBuyMarket(const int qty)
        : order(std::chrono::utc_clock::now().time_since_epoch().count(),qty,0),
        state_(OrderState::SUBMITTED){}

    BuyMarket make_queued_order() const { return BuyMarket(order); }

    void print() const
    {
        std::cout<< "BuyMarketOrder: "<< order.id_ << " State: " << OrderStateToString(state_) <<std::endl;
        std::cout << "Avg Fill Price: "<< order.price_ << "Show Qty: " << order.qty_ << std::endl;
    }
};

class SubmittedSellMarket {
public:

    Order order;
    OrderState state_;

    explicit SubmittedSellMarket(const int qty)
        : order(std::chrono::utc_clock::now().time_since_epoch().count(),qty,0),
        state_(OrderState::SUBMITTED){}

    SellMarket make_queued_order() const { return SellMarket(order); }

    void print() const
    {
        std::cout<< "SellMarketOrder: "<< order.id_ << " State: " << OrderStateToString(state_) <<std::endl;
        std::cout << "Avg Fill Price: "<< order.price_ << "Show Qty: " << order.qty_ << std::endl;
    }
};


class SubmittedCancel {
public:

    Order order;
    OrderState state_;
    ID cancel_id_;

    explicit SubmittedCancel(ID cancel_id, double price)
        : order(std::chrono::utc_clock::now().time_since_epoch().count(),0,price),
        cancel_id_(cancel_id),
        state_(OrderState::SUBMITTED){}

    Cancel make_queued_order() const { return Cancel(order, cancel_id_); }

    void print() const
    {
        std::cout<< "Cancel Order ID : "<< order.id_ << " State: " << OrderStateToString(state_) <<std::endl;
        std::cout<< "Order to be cancelled ID : "<< cancel_id_ << std::endl;
    }
};

#endif //SUBMITTEDORDERTYPES_H
