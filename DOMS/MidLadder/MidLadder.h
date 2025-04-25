//
// Created by prw on 4/25/25.
//

#ifndef MIDLADDER_H
#define MIDLADDER_H

#include <vector>

#include "FifoMatchingStrategy.h"
#include "../../PriceLevels/LevelRequirements.h"
#include "../../Matchers/MatcherRequirements.h"


template<typename Order, template<Is_Level>typename Lev>
class MidLadder {





    using Level = Lev<Order>;
    using Dom = std::vector<Level>;
    using DomIter = typename std::vector<Level>::iterator;

    Dom dom_;
    size_t num_prices_;

    DomIter bid_;
    DomIter ask_;
    DomIter limit_;

public:
    MidLadder()
    : dom_(100), num_prices_(100), bid_(dom_.begin()+50), ask_(dom_.begin()+51), limit_(0) {}

    MidLadder(const size_t num_prices, size_t bid,size_t ask)
        : num_prices_(num_prices), bid_(dom_.begin()+bid), ask_(dom_.begin()+ask) {}

    friend class FifoMatchingStrategy<Order, Lev>;;
    size_t bid() const {return std::distance(dom_.begin(),bid_);}
    size_t ask() const {return std::distance(dom_.begin(),ask_);}
    size_t limit() const {return std::distance(dom_.begin(),limit_);}

    const Dom& dom() const {return dom_;}
    const auto& orders(size_t idx){ return (dom_.begin()+idx)->orders(); }

};



#endif //MIDLADDER_H
