//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_RECORD_H
#define ORDERBOOK_RECORD_H

#include <string>
#include <vector>
#include <chrono>
#include <iostream>

#include "MatchedOrders.h"
#include "RecordDepot.h"


/**
 *  @brief A record is an order and the history of transactions on that order
 *
 *  Record is initialized with a Submitted Order type
 *  In addition to the core record members, id, qty, (limit)price it
 *  has vectors to record changes in order qty, state and the timestamp
 *  those changes occurred.
 *
 *  For market orders filled over several prices, record also calculates
 *  the average fill price
 *
 */
namespace order
{

    class Record
    {

    public:
        std::string type;
        ID id;
        Price limit_price;
        float filled_price{};
        std::vector<Qty> quantities;
        std::vector<OrderState> states;
        std::vector<Time> timestamps;

        Record();

        explicit Record(Submitted order)
        {
            quantities.reserve(4);
            states.reserve(4);
            timestamps.reserve(4);

            std::visit([this](auto o)
            {
                type = (order_types[std::type_index(typeid(o))]);
                id = o.id;
                limit_price = o.price;
                quantities.push_back(o.qty);

            },order);

            states.push_back(OrderState::SUBMITTED);
            timestamps.push_back(timestamp());
        }

        private:

        friend class RecordDepot<Record>;
        friend bool operator==(const Record& lhs, const Record& rhs);
        friend std::ostream& operator<<(std::ostream& os, const Record& o);


        static Time timestamp();
        void update(const Time& ts);
        void update(OrderState state, const Time& ts);
        void update(PartialFill o, const Time& ts);
        void update(const MarketFill& o, const Time& ts);

        [[nodiscard]] bool is_market_to_limit(Qty fill_qty) const;
        void update_market_limit(const MarketFill& o, const Time& ts);


    };

    inline bool operator==(const Record& lhs, const Record& rhs)
    {
        return lhs.id == rhs.id
            && lhs.type == rhs.type
            && lhs.limit_price == rhs.limit_price
            && lhs.quantities== rhs.quantities
            && lhs.states == rhs.states;
    }


    inline std::ostream& operator<<(std::ostream& os, const Record& o)
    {
        Qty prev_quantity = o.quantities.size() >= 2 ? o.quantities[o.quantities.size()-2] : o.quantities.back();
        os
        << std::format("{:<5} {:<16} {:<9} {:>5} {:>5} {:>8} {:>10.2f}",
                o.id, o.type,OrderStateToString(o.states.back()),prev_quantity,o.quantities.back(),o.limit_price,o.filled_price);
        return os;
    }

    inline std::ostream& record_header(std::ostream& os)
    {
        std::cout<<"==================================================================="<<std::endl;
        os << std::format(
            "{:^3} {:^16} {:^10} {:^8} {:^5} {:^8} {:^10}",
                    "ID","Type","State","LastQty","Remain","Limit","FillPrice")<<"\n";

        return os;
    }



}


#endif //ORDERBOOK_RECORD_H