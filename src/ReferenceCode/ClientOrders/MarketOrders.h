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
    MarketOrder _;

public:
    explicit BuyMarketOrder(int qty)
        : _(std::chrono::utc_clock::now().time_since_epoch().count(), qty){}

    // getters
    ID id() const{return _.id_;}
    int qty() const{return _.qty_;}
    int& qty() {return _.qty_;}
    double price() const{return _.avg_fill_price_;}
    double& price() {return _.price_;}
    double fill_price() const{return _.avg_fill_price_;}
    double& fill_price() {return _.avg_fill_price_;}
    OrderState state() const{return _.order_state_;}
    OrderState& state() {return _.order_state_;}




    //void update_state(OrderState order_state){order_.order_state_ = order_state;}


    QueuedMarketOrder make_market_order()
    {
        return {_.id_,_.qty_};
    }

    void print()
    {
        std::cout<< "BuyMarketOrder: "<< _.id_ << " State: " << OrderStateToString(_.order_state_) <<std::endl;
        std::cout << "FillPrice: "<< _.avg_fill_price_ << " Qty: " << _.qty_ <<std::endl;

    }

};

class SellMarketOrder : private ClientOrderTag
{
public:
    MarketOrder _;


    explicit SellMarketOrder(int qty)
        : _(std::chrono::utc_clock::now().time_since_epoch().count(), qty*-1){}

    // getters
    ID id() const{return _.id_;}
    int qty() const{return _.qty_;}
    int& qty() {return _.qty_;}
    double price() const{return _.avg_fill_price_;}
    double& price() {return _.price_;}
    double fill_price() const{return _.avg_fill_price_;}
    double& fill_price() {return _.avg_fill_price_;}
    OrderState state() const{return _.order_state_;}
    OrderState& state() {return _.order_state_;}



    //void update_qty(int qty){order_.qty_ = qty;}
   // void update_state(OrderState order_state){order_.order_state_ = order_state;}
   // void update_fill_price(double price){order_.avg_fill_price_ = price;}

    QueuedMarketOrder make_market_order()
    {
        return {_.id_,_.qty_};
    }

    void print()
    {
        std::cout<< "SellMarketOrder: "<< _.id_ << " State: " << OrderStateToString(_.order_state_) <<std::endl;
        std::cout << "FillPrice: "<< _.avg_fill_price_ << " Qty: " << _.qty_ <<std::endl;

    }


};

#endif //MARKETORDERS_H
