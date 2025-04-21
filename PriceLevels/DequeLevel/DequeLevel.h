#ifndef DEQUELEVEL_H
#define DEQUELEVEL_H

#include <deque>
#include <vector>
#include "../../OrderTypes/QueuedOrders/QueuedOrders.h"

class DequeLevel
{
public:
    std::deque<QueuedLimitOrder> limit_orders_;
    std::deque<QueuedMarketOrder> stop_orders_;
    int depth_{};


    DequeLevel()=default;


    int depth() const {return depth_;}
    int& depth() {return depth_;}
    //  void update_depth(int depth){depth_ = depth;}

    size_t count() const {return limit_orders_.size();}

    void append_limit_order(QueuedLimitOrder limit_order)
    {

        depth_ += limit_order.qty_;
        limit_orders_.push_back(limit_order);
    }

    void append_stop_order(QueuedMarketOrder stop_order)
    {
        stop_orders_.push_back(stop_order);
    }

    void reset_stop_orders()
    {
        for (auto& order : stop_orders_)
            order.reset();
    }

    void zero_out_limit_orders()
    {
        for (auto& order : limit_orders_)
        {
            order.qty_ = 0;
            order.display_=0;
        }
    }

    ID fully_fill_next_order()
    {
        ID id =  limit_orders_.front().id_;
        limit_orders_.pop_front();
        return id;
    }

    std::deque<QueuedLimitOrder>::iterator find_order(ID id)
    {
        return std::lower_bound(limit_orders_.begin(),limit_orders_.end(),id,
            [&](const QueuedLimitOrder& order, const ID id_){return order.id_< id_;});
    }


    QueuedLimitOrder get_order(ID id)
    {
        return *find_order(id);
    }

    QueuedLimitOrder remove_order(ID id)
    {
        std::deque<QueuedLimitOrder>::iterator ord = find_order(id);
        QueuedLimitOrder cancelled = *ord;
        limit_orders_.erase(ord);
        return cancelled;
    }



};



#endif