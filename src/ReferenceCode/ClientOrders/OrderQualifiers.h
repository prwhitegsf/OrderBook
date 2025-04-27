//
// Created by prw on 4/19/25.
//

#ifndef ORDERQUALIFIERS_H
#define ORDERQUALIFIERS_H

#include <string>


struct ClientOrderTag {};
using ID = unsigned long;
enum class OrderState
{

    SUBMITTED,
    ACCEPTED,
    REJECTED,
    PENDING,
    PARTIALLY_FILLED,
    FILLED


};

struct OrderUpdate {

    ID id_;
    double price_;
    double qty_;
    OrderState state_;
    long update_ts{};

    OrderUpdate(ID id,double price,int qty,OrderState state)
        : id_(id),price_(price),qty_(qty),state_(state) {}

};


inline std::string OrderStateToString(OrderState state)
{
    switch (state)
    {
        case OrderState::SUBMITTED:
            return "SUBMITTED";
        case OrderState::ACCEPTED:
            return "ACCEPTED";
        case OrderState::REJECTED:
            return "REJECTED";
        case OrderState::PENDING:
            return "PENDING";
        case OrderState::PARTIALLY_FILLED:
            return "PARTIALLY_FILLED";
        case OrderState::FILLED:
            return "FILLED";
        default:
            return "UNKNOWN";
    }

}



enum class Duration
{
    DAY,
    GTC,
    GTD

};

inline std::string DurationToString(Duration duration)
{
    if (duration == Duration::DAY)
        return"Day";
    else if (duration == Duration::GTC)
        return"GTC";
    else
        return"GTD";

}


#endif //ORDERQUALIFIERS_H
