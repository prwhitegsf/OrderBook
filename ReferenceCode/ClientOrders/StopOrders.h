//
// Created by prw on 4/19/25.
//

#ifndef STOPORDERS_H
#define STOPORDERS_H

#include <chrono>
#include <iostream>
#include "../QueuedOrders/QueuedOrders.h"
#include "OrderQualifiers.h"

using Expiry = std::chrono::year_month_day;

struct StopOrder
{
    ID id_;
    int qty_;
    double price_;
    double avg_fill_price_{};


    Duration good_until_;
    long order_expiration_;
    OrderState order_state_;

    StopOrder(ID id, int qty, double price, Duration good_until, long order_expiration = 0)
        :   id_(id),
            qty_(qty),
            price_(price),
            good_until_(good_until),
            order_expiration_(order_expiration),
            order_state_(order_expiration == 0 && good_until == Duration::GTD ? OrderState::REJECTED : OrderState::ACCEPTED){}
};

class BuyStopOrder : private ClientOrderTag
{
public:
    StopOrder _;


    BuyStopOrder(int qty, double price, Duration good_until, long order_expiration = 0)
        : _(
            std::chrono::utc_clock::now().time_since_epoch().count(),
            qty,
            price,
            good_until,
            order_expiration){}


    // getters
    ID id() const{return _.id_;}
    int qty() const{return _.qty_;}
    int& qty() {return _.qty_;}
    double price() const{return _.price_;}
    double& price() {return _.price_;}
    double fill_price() const{return _.avg_fill_price_;}
    double& fill_price() {return _.avg_fill_price_;}
    Duration good_until() const{return _.good_until_;}
    OrderState state() const{return _.order_state_;}
    OrderState& state() {return _.order_state_;}
    long order_expiration() const{return _.order_expiration_;}


    //void update_qty(int qty){order_.qty_ = qty;}
    void update_state(OrderState order_state){_.order_state_ = order_state;}
    //void update_fill_price(double price){order_.avg_fill_price_ = price;}

    QueuedMarketOrder make_stop_order()
    {
        return {_.id_,_.qty_};
    }

    void print()
    {
        std::cout<< "BuyStopOrder: "<< _.id_ << " State: " << OrderStateToString(_.order_state_) <<std::endl;
        std::cout << "Price: "<< _.price_ << " Qty: " << _.qty_ <<
                    " Duration: " << DurationToString(_.good_until_)<<
                    " Expiry: "<<_.order_expiration_<<std::endl;
    }
};

class SellStopOrder : private ClientOrderTag
{
public:
    StopOrder _;


    SellStopOrder(int qty, double price, Duration good_until, long order_expiration = 0)
        : _(
            std::chrono::utc_clock::now().time_since_epoch().count(),
            qty*-1,
            price,
            good_until,
            order_expiration){}


    // just ditch this and make order public
    ID id() const{return _.id_;}
    int qty() const{return _.qty_;}
    int& qty() {return _.qty_;}
    double price() const{return _.price_;}
    double& price() {return _.price_;}
    double fill_price() const{return _.avg_fill_price_;}
    double& fill_price() {return _.avg_fill_price_;}
    Duration good_until() const{return _.good_until_;}
    OrderState state() const{return _.order_state_;}
    OrderState& state() {return _.order_state_;}
    long order_expiration() const{return _.order_expiration_;}




    QueuedMarketOrder make_stop_order()
    {
        return {_.id_,_.qty_};
    }

    void print()
    {
        std::cout<< "SellStopOrder: "<< _.id_ << " State: " << OrderStateToString(_.order_state_) <<std::endl;
        std::cout << "Price: "<< _.price_ << " Qty: " << _.qty_ <<
                    " Duration: " << DurationToString(_.good_until_)<<
                    " Expiry: "<<_.order_expiration_<<std::endl;
    }
};

#endif //STOPORDERS_H
