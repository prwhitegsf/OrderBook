//
// Created by prw on 4/22/25.
//

#ifndef ORDERQUEUE_H
#define ORDERQUEUE_H

#include <variant>
#include <deque>

#include "../OrderTypes/Orders.h"
//#include "../Orderbook/Orderbook.h"

using QueueOrder = std::variant<SubmittedBuyLimit,SubmittedSellLimit,SubmittedBuyMarket,SubmittedSellMarket,SubmittedCancel>;
template<class OrderBook>
class OrderQueue {


public:
    std::deque<QueueOrder> q;



    ID front_id()
    {
        ID id{};
        std::visit([&](auto&& o){id = o._.id_;},q.front());

        return id;
    }

    void push(QueueOrder&& order) { q.push_back(order); }

    int front_type()const { return q.front().index(); }

    QueueOrder front() { return q.front(); }

    void pop() { q.pop_front(); }


    void SubmitAll(OrderBook& ob)
    {
        for (auto order : q)
        {
            std::visit([&](auto& o)
           {
                ob.SubmitOrder(o.make_queued_order());

           },order);

            q.pop_front();
        }
    }



    void generate_orders(const double starting_bid, const double starting_ask,
        std::vector<int> depths,OrderBook& ob)
    {
        auto& dom = ob.price_ladder_;
        dom.bid_idx_ = dom.idx_from_price(starting_bid);
        dom.ask_idx_ =  dom.idx_from_price(starting_ask);

        size_t i{dom.bid_idx_};
        for (int depth : depths)
        {
            int j{depth};
            while (j > 0)
            {
                SubmittedBuyLimit buy(1,dom.price_from_idx(i),Duration::DAY);
                push(buy);
                ob.instrument_->client_order_list_.append_order(buy);
                --j;
            }

            --i;
        }

        i = dom.ask_idx_;
        for (int depth : depths)
        {
            int j{depth};
            while (j > 0) {
                SubmittedSellLimit sell(1,dom.price_from_idx(i),Duration::DAY);
                push(sell);
                ob.instrument_->client_order_list_.append_order(sell);
                --j;
            }

            ++i;
        }
    }
};




#endif //ORDERQUEUE_H
