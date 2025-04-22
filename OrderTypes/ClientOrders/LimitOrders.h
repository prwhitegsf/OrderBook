//
// Created by prw on 4/19/25.
//

#ifndef LIMITORDERS_H
#define LIMITORDERS_H

#include <chrono>
#include <iostream>

#include "../QueuedOrders/QueuedOrders.h"
#include "OrderQualifiers.h"


//using Expiry = std::chrono::year_month_day;
using ID = unsigned long;


struct LimitOrder
{
    ID id_;
    int qty_;
    int display_;
    double price_;

    int filled_order_count_{};


    Duration good_until_;
    unsigned long order_expiration_;
    OrderState order_state_;

    LimitOrder(ID id, int qty, int display, double price, Duration good_until, long order_expiration = 0)
        :   id_(id),qty_(qty),display_(display),price_(price),
            good_until_(good_until),
            order_expiration_(order_expiration),
            order_state_(order_expiration == 0 && good_until == Duration::GTD ? OrderState::REJECTED : OrderState::ACCEPTED){}
};


class BuyLimitOrder : private ClientOrderTag
{
public:
    LimitOrder _;


    BuyLimitOrder(int qty, int display, double price, Duration good_until, long order_expiration = 0)
        : _(
            std::chrono::utc_clock::now().time_since_epoch().count(),
            qty,
            display,
            price,
            good_until,
            order_expiration){}


    // getters
    ID id() const{return _.id_;}
    int qty() const{return _.qty_;}
    int& qty() {return _.qty_;}
    int display() const{return _.display_;}
    double price() const{return _.price_;}
    double& price() {return _.price_;}
    Duration good_until() const{return _.good_until_;}
    OrderState state() const{return _.order_state_;}
    OrderState& state() {return _.order_state_;}
    long order_expiration() const{return _.order_expiration_;}


   // void update_qty(int qty){order_.qty_ = qty;}
    //void update_state(OrderState order_state){order_.order_state_ = order_state;}

    QueuedLimitOrder make_limit_order()
    {
        return {_.id_,_.qty_,_.display_};
    }

    void print()
    {
        std::cout<< "BuyLimitOrder: "<< _.id_ << " State: " << OrderStateToString(_.order_state_) <<std::endl;
        std::cout << "Price: "<< _.price_ << " Qty: " << _.qty_ <<
                    " Display: "<< _.display_ << " Duration: " << DurationToString(_.good_until_)<<
                    " Expiry: "<<_.order_expiration_<<std::endl;
    }
};

class SellLimitOrder : private ClientOrderTag
{
public:
    LimitOrder _;


    SellLimitOrder(int qty, int display, double price, Duration good_until,long order_expiration = 0)
        : _(
            std::chrono::utc_clock::now().time_since_epoch().count(),
            qty*-1,
            display*-1,
            price,
            good_until,
            order_expiration){}


    // getters
    ID id() const{return _.id_;}
    int qty() const{return _.qty_;}
    int& qty() {return _.qty_;}
    int display() const{return _.display_;}
    double price() const{return _.price_;}
    double& price() {return _.price_;}
    Duration good_until() const{return _.good_until_;}
    OrderState state() const{return _.order_state_;}
    OrderState& state() {return _.order_state_;}
    long order_expiration() const{return _.order_expiration_;}


    //void update_qty(int qty){order_.qty_ = qty;}
    //void update_state(OrderState order_state){order_.order_state_ = order_state;}

    QueuedLimitOrder make_limit_order()
    {
        return {_.id_,_.qty_,_.display_};
    }

    void print()
    {
        std::cout<< "SellLimitOrder: "<< _.id_ << " State: " << OrderStateToString(_.order_state_) <<std::endl;
        std::cout << "Price: "<< _.price_ << " Qty: " << _.qty_ <<
                    " Display: "<< _.display_ << " Duration: " << DurationToString(_.good_until_)<<
                    " Expiry: "<<_.order_expiration_<<std::endl;
    }
};



#endif //LIMITORDERS_H
