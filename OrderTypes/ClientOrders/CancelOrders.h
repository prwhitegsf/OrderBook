//
// Created by prw on 4/19/25.
//

#ifndef CANCELORDERS_H
#define CANCELORDERS_H

#include <chrono>
#include "OrderQualifiers.h"
#include <iostream>
using ID = unsigned long;

class CancelOrder : private ClientOrderTag{

    ID id_;
    ID cancel_id_;
    double price_;
    OrderState state_;
    int qty_{};

public:

    CancelOrder(const ID id, const double price)
    :id_(std::chrono::utc_clock::now().time_since_epoch().count()),
    cancel_id_(id), price_(price),state_(OrderState::SUBMITTED) {}


    [[nodiscard]] std::pair<ID, double> cancel() const { return std::make_pair(cancel_id_, price_); }
    ID id() const{return id_;}
    int qty() const {return 0;}
    int& qty() {return qty_;}
    double price() const{return price_;}
    double& price() {return price_;}
    OrderState state() const{return state_;}
    OrderState& state() {return state_;}

    void print()
    {
        std::cout<< "CancelOrder: "<< id() << " State: " << OrderStateToString(state()) <<std::endl;
        std::cout << "Price: "<< price() << " Cancel ID: " << cancel_id_ <<std::endl;
    }
};



#endif //CANCELORDERS_H
