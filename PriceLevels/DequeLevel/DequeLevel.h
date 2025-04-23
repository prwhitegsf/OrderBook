#ifndef DEQUELEVEL_H
#define DEQUELEVEL_H

#include <deque>
#include <vector>
//#include "../../OrderTypes/Orders.h"

template <typename Ord>
class DequeLevel
{
public:
    std::deque<Ord> limit_orders_;

    DequeLevel()=default;
    int depth_{};
    size_t count() const {return limit_orders_.size();}


    using OrdPtr = typename std::deque<Ord>::iterator;
    OrdPtr begin() { return limit_orders_.begin(); }
    OrdPtr end() { return limit_orders_.end(); }

    OrdPtr find(ID id)
    {
        return std::lower_bound(limit_orders_.begin(),limit_orders_.end(),id,
           [&](const Order& order, const ID id_){return order.id_< id_;});
    }

    Ord front() { return limit_orders_.front(); }
    Ord back() { return limit_orders_.back(); }

    void append_limit_order(Ord limit_order)
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


    Ord remove(ID id)
    {
        OrdPtr ord = find_order(id);
        Order cancelled = *ord;
        depth_ -= cancelled.qty_;
        limit_orders_.erase(ord);
        return cancelled;
    }



    void clear() { limit_orders_.clear(); }



    typename std::deque<Ord>::iterator find_order(ID id)
    {
        return std::lower_bound(limit_orders_.begin(),limit_orders_.end(),id,
            [&](const Order& order, const ID id_){return order.id_< id_;});
    }


    Ord get_order(ID id) { return *find_order(id); }

    Ord remove_order(ID id)
    {
        std::deque<Order>::iterator ord = find_order(id);
        Order cancelled = *ord;
        depth_ -= cancelled.qty_;
        limit_orders_.erase(ord);
        return cancelled;
    }




};



#endif