//
// Created by prw on 4/22/25.
//

#ifndef QUALIFIERS_H
#define QUALIFIERS_H

#include <string>

using ID = unsigned long;
enum class OrderState : short
{

    SUBMITTED,
    ACCEPTED,
    REJECTED,
    PENDING,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED


};

// helper for printing
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
    case OrderState::CANCELLED:
        return "CANCELLED";
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

// helper for printing
inline std::string DurationToString(Duration duration)
{
    if (duration == Duration::DAY)
        return"Day";
    else if (duration == Duration::GTC)
        return"GTC";
    else
        return"GTD";

}



#endif //QUALIFIERS_H
