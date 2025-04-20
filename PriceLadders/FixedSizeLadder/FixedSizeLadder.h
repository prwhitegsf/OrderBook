#ifndef FIXEDSIZELADDER_H
#define FIXEDSIZELADDER_H

#include "../PriceLevels/DequeLevel/DequeLevel.h"

class FixedSizeLadder
{

    using Level = DequeLevel;
    std::vector<Level> levels_;

    size_t bid_idx_;
    size_t ask_idx_;


    Level& bid()
    {
        return levels_[bid_idx_];
    }


    Level& ask()
    {
        return levels_[ask_idx_];
    }

    Level& get_level(size_t idx)
    {
        return levels_[idx];
    }


public:
    FixedSizeLadder()=delete;
    FixedSizeLadder(size_t ladder_size, size_t initial_bid, size_t initial_ask)
        : levels_(ladder_size), bid_idx_(initial_bid), ask_idx_(initial_ask){}


};




#endif