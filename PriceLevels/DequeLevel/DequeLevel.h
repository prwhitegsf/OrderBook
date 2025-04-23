#ifndef DEQUELEVEL_H
#define DEQUELEVEL_H

#include <deque>
#include <vector>
#include "../../OrderTypes/Orders.h"

class DequeLevel
{
public:
    std::deque<Order> limit_orders_;

    int depth_{};


    DequeLevel()=default;


    size_t count() const {return limit_orders_.size();}

    void append_limit_order(Order limit_order)
    {

        depth_ += limit_order.qty_;
        limit_orders_.push_back(limit_order);
    }



    ID fully_fill_next_order()
    {
        ID id =  limit_orders_.front().id_;
        limit_orders_.pop_front();
        return id;
    }

    std::deque<Order>::iterator find_order(ID id)
    {
        return std::lower_bound(limit_orders_.begin(),limit_orders_.end(),id,
            [&](const Order& order, const ID id_){return order.id_< id_;});
    }


    Order get_order(ID id)
    {
        return *find_order(id);
    }

    Order remove_order(ID id)
    {
        std::deque<Order>::iterator ord = find_order(id);
        Order cancelled = *ord;
        depth_ -= cancelled.qty_;
        limit_orders_.erase(ord);
        return cancelled;
    }

    void clear()
    {
        limit_orders_.clear();
    }


};



#endif