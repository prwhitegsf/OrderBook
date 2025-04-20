//
// Created by prw on 4/19/25.
//

#ifndef STOPORDERS_H
#define STOPORDERS_H

#include <chrono>
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
private:
    StopOrder order_;

public:
    BuyStopOrder(int qty, double price, Duration good_until, long order_expiration = 0)
        : order_(
            std::chrono::utc_clock::now().time_since_epoch().count(),
            qty,
            price,
            good_until,
            order_expiration){}


    // getters
    ID id() const{return order_.id_;}
    int qty() const{return order_.qty_;}
    double price() const{return order_.price_;}
    double fill_price() const{return order_.avg_fill_price_;}
    Duration good_until() const{return order_.good_until_;}
    OrderState state() const{return order_.order_state_;}
    long order_expiration() const{return order_.order_expiration_;}


    void update_qty(int qty){order_.qty_ = qty;}
    void update_state(OrderState order_state){order_.order_state_ = order_state;}
    void update_fill_price(double price){order_.avg_fill_price_ = price;}

    QueuedMarketOrder make_stop_order()
    {
        return {order_.id_,order_.qty_};
    }

};

class SellStopOrder : private ClientOrderTag
{
private:
    StopOrder order_;

public:
    SellStopOrder(int qty, double price, Duration good_until, long order_expiration = 0)
        : order_(
            std::chrono::utc_clock::now().time_since_epoch().count(),
            qty,
            price,
            good_until,
            order_expiration){}


    // getters
    ID id() const{return order_.id_;}
    int qty() const{return order_.qty_;}
    double price() const{return order_.price_;}
    double fill_price() const{return order_.avg_fill_price_;}
    Duration good_until() const{return order_.good_until_;}
    OrderState state() const{return order_.order_state_;}
    long order_expiration() const{return order_.order_expiration_;}


    void update_qty(int qty){order_.qty_ = qty;}
    void update_state(OrderState order_state){order_.order_state_ = order_state;}
    void update_fill_price(double price){order_.avg_fill_price_ = price;}

    QueuedMarketOrder make_stop_order()
    {
        return {order_.id_,order_.qty_};
    }

};

#endif //STOPORDERS_H
