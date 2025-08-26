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
        double fill_price{};

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

        /*Matched(const Matched& other)
            : partial_fill(other.partial_fill), market_fill(other.market_fill), state_update(other.state_update)
        {
            if (!other.limit_fills.empty())
            {
                limit_fills.clear();
                for (auto id : other.limit_fills)
                    if (id)
                        limit_fills.push_back(id);
            }
            else
                limit_fills.clear();
        }*/
        Matched& operator=(const Matched& other)
        {
            if (this != &other)
            {
                if (!other.limit_fills.empty())
                {
                    limit_fills.clear();
                    for (auto id : other.limit_fills)
                        if (id)
                            limit_fills.push_back(id);
                }
                else
                    limit_fills.clear();

                partial_fill = other.partial_fill;
                market_fill = other.market_fill;
                state_update = other.state_update;
            }

            return *this;
        }


        void clear()
        {
            limit_fills.clear();
            market_fill.id = 0;
            partial_fill.id = 0;
            state_update.id = 0;
        }

    };




    using Processed =  std::pair<std::vector<Matched>,std::vector<StateUpdate>>;

}


#endif //ORDERBOOK_PROCESSEDORDERS_H