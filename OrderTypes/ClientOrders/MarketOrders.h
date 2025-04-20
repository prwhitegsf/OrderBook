//
// Created by prw on 4/19/25.
//

#ifndef MARKETORDERS_H
#define MARKETORDERS_H

#include <chrono>
#include "../QueuedOrders/QueuedOrders.h"
#include "OrderQualifiers.h"

struct MarketOrder
{
    ID id_;
    int qty_;
    double avg_fill_price_{};

    OrderState order_state_;

    MarketOrder(ID id, int qty)
        :   id_(id),qty_(qty),order_state_(OrderState::ACCEPTED){}
};

class BuyMarketOrder : private ClientOrderTag
{
private:
    MarketOrder order_;

public:
    explicit BuyMarketOrder(int qty)
        : order_(std::chrono::utc_clock::now().time_since_epoch().count(), qty){}

    // getters
    ID id() const{return order_.id_;}
    int qty() const{return order_.qty_;}
    double fill_price() const{return order_.avg_fill_price_;}
    OrderState state() const{return order_.order_state_;}



    void update_qty(int qty){order_.qty_ = qty;}
    void update_state(OrderState order_state){order_.order_state_ = order_state;}
    void update_fill_price(double price){order_.avg_fill_price_ = price;}

    QueuedMarketOrder make_market_order()
    {
        return {order_.id_,order_.qty_};
    }

};

class SellMarketOrder : private ClientOrderTag
{
private:
    MarketOrder order_;

public:
    explicit SellMarketOrder(int qty)
        : order_(std::chrono::utc_clock::now().time_since_epoch().count(), qty){}

    // getters
    ID id() const{return order_.id_;}
    int qty() const{return order_.qty_;}
    double fill_price() const{return order_.avg_fill_price_;}
    OrderState state() const{return order_.order_state_;}



    void update_qty(int qty){order_.qty_ = qty;}
    void update_state(OrderState order_state){order_.order_state_ = order_state;}
    void update_fill_price(double price){order_.avg_fill_price_ = price;}

    QueuedMarketOrder make_market_order()
    {
        return {order_.id_,order_.qty_};
    }

};

#endif //MARKETORDERS_H
