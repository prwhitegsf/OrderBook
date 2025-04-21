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
private:
    LimitOrder order_;

public:
    BuyLimitOrder(int qty, int display, double price, Duration good_until, long order_expiration = 0)
        : order_(
            std::chrono::utc_clock::now().time_since_epoch().count(),
            qty,
            display,
            price,
            good_until,
            order_expiration){}


    // getters
    ID id() const{return order_.id_;}
    int qty() const{return order_.qty_;}
    int& qty() {return order_.qty_;}
    int display() const{return order_.display_;}
    double price() const{return order_.price_;}
    double& price() {return order_.price_;}
    Duration good_until() const{return order_.good_until_;}
    OrderState state() const{return order_.order_state_;}
    OrderState& state() {return order_.order_state_;}
    long order_expiration() const{return order_.order_expiration_;}


   // void update_qty(int qty){order_.qty_ = qty;}
    //void update_state(OrderState order_state){order_.order_state_ = order_state;}

    QueuedLimitOrder make_limit_order()
    {
        return {order_.id_,order_.qty_,order_.display_};
    }

    void print()
    {
        std::cout<< "BuyLimitOrder: "<< order_.id_ << " State: " << OrderStateToString(order_.order_state_) <<std::endl;
        std::cout << "Price: "<< order_.price_ << " Qty: " << order_.qty_ <<
                    " Display: "<< order_.display_ << " Duration: " << DurationToString(order_.good_until_)<<
                    " Expiry: "<<order_.order_expiration_<<std::endl;
    }
};

class SellLimitOrder : private ClientOrderTag
{
private:
    LimitOrder order_;

public:
    SellLimitOrder(int qty, int display, double price, Duration good_until,long order_expiration = 0)
        : order_(
            std::chrono::utc_clock::now().time_since_epoch().count(),
            qty*-1,
            display*-1,
            price,
            good_until,
            order_expiration){}


    // getters
    ID id() const{return order_.id_;}
    int qty() const{return order_.qty_;}
    int& qty() {return order_.qty_;}
    int display() const{return order_.display_;}
    double price() const{return order_.price_;}
    double& price() {return order_.price_;}
    Duration good_until() const{return order_.good_until_;}
    OrderState state() const{return order_.order_state_;}
    OrderState& state() {return order_.order_state_;}
    long order_expiration() const{return order_.order_expiration_;}


    //void update_qty(int qty){order_.qty_ = qty;}
    //void update_state(OrderState order_state){order_.order_state_ = order_state;}

    QueuedLimitOrder make_limit_order()
    {
        return {order_.id_,order_.qty_,order_.display_};
    }

    void print()
    {
        std::cout<< "SellLimitOrder: "<< order_.id_ << " State: " << OrderStateToString(order_.order_state_) <<std::endl;
        std::cout << "Price: "<< order_.price_ << " Qty: " << order_.qty_ <<
                    " Display: "<< order_.display_ << " Duration: " << DurationToString(order_.good_until_)<<
                    " Expiry: "<<order_.order_expiration_<<std::endl;
    }
};



#endif //LIMITORDERS_H
