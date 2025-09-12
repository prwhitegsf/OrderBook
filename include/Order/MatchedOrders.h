//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_PROCESSEDORDERS_H
#define ORDERBOOK_PROCESSEDORDERS_H




#include <vector>
#include "ValueTypes.h"
#include "OrderTypes.h"
#include "OverwritingVector.h"

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
        Qty qty{};
        Price limit{};
        float fill_price{};
    };

    struct StateUpdate
    {
        ID id{};
        OrderState state{OrderState::SUBMITTED};
    };

    struct Matched
    {
        OverwritingVector<ID> limit_fills;
        PartialFill partial_fill;
        MarketFill market_fill;
        StateUpdate state_update;

        Matched() : limit_fills(8) {}


        Matched& operator=(const Matched& other)
        {
            if (this != &other)
            {
                limit_fills = other.limit_fills;
                partial_fill = other.partial_fill;
                market_fill = other.market_fill;
                state_update = other.state_update;
            }

            return *this;
        }


        void clear()
        {
            limit_fills.clear();
            market_fill.id = 0, market_fill.fill_price = 0;
            partial_fill.id = 0;
            state_update.id = 0;
        }

    };




    using Processed =  std::pair<std::vector<Matched>,std::vector<StateUpdate>>;

}


#endif //ORDERBOOK_PROCESSEDORDERS_H