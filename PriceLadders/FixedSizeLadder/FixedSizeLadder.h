#ifndef FIXEDSIZELADDER_H
#define FIXEDSIZELADDER_H

#include "../../Instrument/Instrument.h"
#include "../../PriceLevels/DequeLevel/DequeLevel.h"
using Level = DequeLevel;
using DomIter = std::vector<Level>::iterator;
class FixedSizeLadder
{
public:

    double min_price_;
    double max_price_;
    double price_increment_;
    size_t num_prices_;

    std::vector<OrderUpdate> order_updates_;


    std::vector<Level> levels_;

    DomIter bid_{ levels_.begin() };
    DomIter ask_{ levels_.end() };
    size_t bid_idx_;
    size_t ask_idx_;

    Level& ask()
    {

        return levels_[ask_idx_];
    }

    Level& bid()
    {

        return levels_[bid_idx_];
    }

    Level& level(size_t idx)
    {

        return levels_[idx];
    }



    FixedSizeLadder()=delete;

    explicit FixedSizeLadder(const std::shared_ptr<Instrument>& instrument)
    :   min_price_(instrument->min_price()),
        max_price_(instrument->max_price()),
        price_increment_(instrument->price_increment()),
        num_prices_(static_cast<size_t>((max_price_ - min_price_)*(1.0/price_increment_))),
        levels_(num_prices_),bid_idx_(instrument->bid()), ask_idx_(instrument->ask())
    {
       // bid_ = (levels_.begin() + bid_idx_);
      //+  ask_ = (levels_.begin() + ask_idx_);
    }

    [[nodiscard]] size_t idx_from_price(double const price) const {
        return static_cast<size_t>((price - min_price_)/(price_increment_));
    }

    template<typename T>
    requires std::is_arithmetic_v<T>
    [[nodiscard]] double price_from_idx(T const index) const {
        return (static_cast<double>(index) * price_increment_) + min_price_;
    }

    double bid_price() const {return price_from_idx(bid_idx_);}
    double ask_price() const {return price_from_idx(ask_idx_);}

    void clear_order_updates()
    {
       // order_updates_.clear();
    }

};




#endif