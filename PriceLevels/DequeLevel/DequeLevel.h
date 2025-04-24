#ifndef DEQUELEVEL_H
#define DEQUELEVEL_H

#include <deque>

using ID = unsigned int;
template <typename Ord>
class DequeLevel
{
    std::deque<Ord> limit_orders_;

public:
    DequeLevel()=default;
    int depth_{};
    size_t count() const {return limit_orders_.size();}

    const std::deque<Ord>& get_limit_orders() const {return limit_orders_;}

    using OrdPtr = typename std::deque<Ord>::iterator;
    OrdPtr begin() { return limit_orders_.begin(); }
    OrdPtr end() { return limit_orders_.end(); }

    Ord& front() { return limit_orders_.front(); }
    Ord& back() { return limit_orders_.back(); }

    void append_new(Ord limit_order) {
        depth_ += limit_order.qty_;
        limit_orders_.push_back(limit_order);
    }

    void copy_to_back(Ord limit_order)
    {
        limit_orders_.push_back(limit_order);
    }

    void pop_front(){ limit_orders_.pop_front(); }

    void clear() { limit_orders_.clear(); }

    Ord remove(const ID id) {
        OrdPtr ord = find(id);
        Ord cancelled = *ord;
        depth_ -= cancelled.qty_;
        limit_orders_.erase(ord);
        return cancelled;
    }

    OrdPtr find(ID id) {
        return std::lower_bound(limit_orders_.begin(),limit_orders_.end(),id,
           [&](const Ord& order, const ID id_){return order.id_< id_;});
    }




};



#endif