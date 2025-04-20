#ifndef DEQUELEVEL_H
#define DEQUELEVEL_H

#include <deque>
#include <vector>
#include "../OrderTypes/QueuedOrders/QueuedOrders.h"

class DequeLevel
{

    std::deque<QueuedLimitOrder> limit_orders_;
    std::vector<QueuedMarketOrder> stop_orders_;
    int depth_{};

    public:
        DequeLevel();

    int depth() const{return depth_;}
    void update_depth(int depth){depth_ = depth;}

    size_t count() const {return limit_orders_.size();}

    void append_limit_order(QueuedLimitOrder limit_order)
    {
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

};



#endif