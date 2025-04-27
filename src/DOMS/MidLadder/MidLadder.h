//
// Created by prw on 4/25/25.
//

#ifndef MIDLADDER_H
#define MIDLADDER_H

#include <vector>


#include "../../Levels/LevelRequirements.h"
#include "../../DOMS/DomRequirements.h"

template<Is_Level Level>
class MidLadder {

    template<Is_Dom U>
    friend class FifoStrategy;

    using Dom = std::vector<Level>;
    using DomIter = typename std::vector<Level>::iterator;

    Dom dom_;
    size_t num_prices_;

    DomIter bid_{ dom_.begin() };
    DomIter ask_{ dom_.end() };
    DomIter limit_{ dom_.begin() };

public:

    MidLadder()
    : dom_(100), num_prices_(100), bid_(dom_.begin()+50), ask_(dom_.begin()+51), limit_(0) {}

    MidLadder(const size_t num_prices, size_t bid,size_t ask)
        : dom_(num_prices),num_prices_(num_prices), bid_(dom_.begin()+bid), ask_(dom_.begin()+ask) {}



    size_t bid()  {return std::distance(dom_.begin(),bid_);}
    size_t ask() {return std::distance(dom_.begin(),ask_);}
    size_t limit()  {return std::distance(dom_.begin(),limit_);}
    size_t idx(const DomIter ptr){return std::distance(dom_.begin(), ptr);};


    void set_bid(size_t idx) { bid_ = dom_.begin()+idx; }
    void set_ask(size_t idx) { ask_ = dom_.begin()+idx; }

    [[nodiscard]]size_t num_prices() const {return num_prices_;};

    const Dom& dom() const {return dom_;}
    const auto& orders(size_t idx){ return (dom_.begin()+idx)->orders(); }
    int depth(size_t idx) const {return (dom_.begin()+idx)->depth();}
};



#endif //MIDLADDER_H
