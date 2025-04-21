//
// Created by prw on 4/19/25.
//

#ifndef MARKETORDERS_H
#define MARKETORDERS_H

#include <chrono>
#include <iostream>
#include "../QueuedOrders/QueuedOrders.h"
#include "OrderQualifiers.h"

struct MarketOrder
{
    ID id_;
    int qty_;
    double avg_fill_price_{};
    double& price_ = avg_fill_price_;

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
    int& qty() {return order_.qty_;}
    double price() const{return order_.avg_fill_price_;}
    double& price() {return order_.price_;}
    double fill_price() const{return order_.avg_fill_price_;}
    double& fill_price() {return order_.avg_fill_price_;}
    OrderState state() const{return order_.order_state_;}
    OrderState& state() {return order_.order_state_;}




    //void update_state(OrderState order_state){order_.order_state_ = order_state;}


    QueuedMarketOrder make_market_order()
    {
        return {order_.id_,order_.qty_};
    }

    void print()
    {
        std::cout<< "BuyMarketOrder: "<< order_.id_ << " State: " << OrderStateToString(order_.order_state_) <<std::endl;
        std::cout << "FillPrice: "<< order_.avg_fill_price_ << " Qty: " << order_.qty_ <<std::endl;

    }

};

class SellMarketOrder : private ClientOrderTag
{
private:
    MarketOrder order_;

public:
    explicit SellMarketOrder(int qty)
        : order_(std::chrono::utc_clock::now().time_since_epoch().count(), qty*-1){}

    // getters
    ID id() const{return order_.id_;}
    int qty() const{return order_.qty_;}
    int& qty() {return order_.qty_;}
    double price() const{return order_.avg_fill_price_;}
    double& price() {return order_.price_;}
    double fill_price() const{return order_.avg_fill_price_;}
    double& fill_price() {return order_.avg_fill_price_;}
    OrderState state() const{return order_.order_state_;}
    OrderState& state() {return order_.order_state_;}



    //void update_qty(int qty){order_.qty_ = qty;}
   // void update_state(OrderState order_state){order_.order_state_ = order_state;}
   // void update_fill_price(double price){order_.avg_fill_price_ = price;}

    QueuedMarketOrder make_market_order()
    {
        return {order_.id_,order_.qty_};
    }

    void print()
    {
        std::cout<< "SellMarketOrder: "<< order_.id_ << " State: " << OrderStateToString(order_.order_state_) <<std::endl;
        std::cout << "FillPrice: "<< order_.avg_fill_price_ << " Qty: " << order_.qty_ <<std::endl;

    }


};

#endif //MARKETORDERS_H
