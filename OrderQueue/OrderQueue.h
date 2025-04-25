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

    void push_back(QueueOrder&& order) { q.push_back(order); }

    [[nodiscard]] int front_type()const { return q.front().index(); }

    [[nodiscard]] QueueOrder front() const { return q.front(); }

    void pop_front() { q.pop_front(); }

    void submit_front(OrderBook& ob)
    {
        std::visit([&](auto& o)
       {
            ob.SubmitOrder(o.make_queued_order());

       },q.front());

        q.pop_front();
    }

    void submit_all(OrderBook& ob)
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
