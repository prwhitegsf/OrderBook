#ifndef DEQUELEVEL_H
#define DEQUELEVEL_H

#include <deque>
#include <algorithm>
#include "../../OrderTypes/Orders.h"

using ID = unsigned int;

class DequeLevel
{
    std::deque<Order> limit_orders_;

public:
    DequeLevel()=default;
    int depth_{};
    [[nodiscard]] size_t count() const {return limit_orders_.size();}

    const std::deque<Order>& orders() const {return limit_orders_;}

    using OrdPtr = typename std::deque<Order>::iterator;
    OrdPtr begin() { return limit_orders_.begin(); }
    OrdPtr end() { return limit_orders_.end(); }

    Order& front() { return limit_orders_.front(); }
    Order& back() { return limit_orders_.back(); }

    void append_new(Order limit_order) {
        depth_ += limit_order.qty_;
        limit_orders_.push_back(limit_order);
    }

    void copy_to_back(Order limit_order)
    {
        limit_orders_.push_back(limit_order);
    }

    void pop_front(){ limit_orders_.pop_front(); }

    void clear() { limit_orders_.clear(); }

    Order remove(const ID id) {
        OrdPtr ord = find(id);
        Order cancelled = *ord;
        depth_ -= cancelled.qty_;
        limit_orders_.erase(ord);
        return cancelled;
    }

    OrdPtr find(ID id) {
        return std::lower_bound(limit_orders_.begin(),limit_orders_.end(),id,
           [&](const Order& order, const ID id_){return order.id_< id_;});
    }

    int depth() const { return depth_; }



};



#endif