//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_MATCHERCONCEPT_H
#define ORDERBOOK_MATCHERCONCEPT_H


#include "OrderTypes.h"

template<typename Matcher>
concept Is_Matcher = requires(Matcher matcher)
{


    matcher.match(order::BuyLimit(0,1,1));
    matcher.match(order::SellLimit(0,1,1));
    matcher.match(order::BuyMarketLimit(0,1,1,1,1));
    matcher.match(order::SellMarketLimit(0,1,1,1,1));
    matcher.match(order::BuyMarket(0,1,1));
    matcher.match(order::SellMarket(0,1,1));
    matcher.match(order::Cancel(0,1,1));
    matcher.level(0);
};



#endif //ORDERBOOK_MATCHERCONCEPT_H