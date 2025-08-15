//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_PROCESSEDORDERS_H
#define ORDERBOOK_PROCESSEDORDERS_H




#include <vector>
#include "ValueTypes.h"
#include "OrderTypes.h"

namespace order
{
    struct PartialFill
    {
        ID id{};
        Qty qty{};
    };

    struct MarketFill
    {
        ID id{};
        float fill_price{};
        std::vector<std::pair<Price,Qty>> fills;
        Qty qty{};

    };


    struct OrderFills
    {
        std::vector<ID> limit_fills;
        PartialFill partial_fill;
        MarketFill market_fill;
    };

    struct StateUpdate
    {
        ID id{};
        OrderState state{OrderState::SUBMITTED};
    };

    using Processed =  std::pair<std::vector<OrderFills>,std::vector<StateUpdate>>;

}


#endif //ORDERBOOK_PROCESSEDORDERS_H