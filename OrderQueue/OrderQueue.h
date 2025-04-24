//
// Created by prw on 4/22/25.
//

#ifndef ORDERQUEUE_H
#define ORDERQUEUE_H

#include <variant>
#include <deque>


using QueueOrder = std::variant<SubmittedBuyLimit<Order>,
                                SubmittedSellLimit<Order>,
                                SubmittedBuyMarket<Order>,
                                SubmittedSellMarket<Order>,
                                SubmittedCancel<Order>>;
template<class OrderBook>
class OrderQueue {


public:
    std::deque<QueueOrder> q;



    ID front_id()
    {
        ID id{};
        std::visit([&](auto&& o){id = o.order.id_;},q.front());

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




};




#endif //ORDERQUEUE_H
