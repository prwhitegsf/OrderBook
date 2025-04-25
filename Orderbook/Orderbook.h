#ifndef ORDERBOOK_H
#define ORDERBOOK_H


#include <memory>
#include "../Instrument/Instrument.h"
#include "../Matchers/MatcherRequirements.h"
#include "../DOMS/MidLadder/MidLadder.h"

template <Is_Matcher Matcher>
class Orderbook
{
    Matcher matcher_;
    MidLadder<Order,DequeLevel> midLadder_{};

public:

    std::shared_ptr<Instrument> instrument_;

    explicit Orderbook(std::shared_ptr<Instrument> instrument, Matcher matcher)
        : instrument_(std::move(instrument)),
        matcher_(matcher){}

    template <typename O>
    requires std::is_base_of_v<QueuedOrderTag,O>
    OrderUpdate SubmitOrder(O&& order)
    {
        const OrderUpdate update = matcher_.match(std::forward<O>(order));

        instrument_->update_bid(matcher_.bid_idx());
        instrument_->update_ask(matcher_.ask_idx());
        instrument_->update_order_records(matcher_.order_updates());

        matcher_.clear_updates();

        std::cout << "Call to DOM: "<< matcher_.idx_from_dom(midLadder_, 50) << std::endl;
        return update;
    }

    // changed this
    void set_bid(size_t idx){ matcher_.set_bid(idx); }
    void set_ask(size_t idx){ matcher_.set_ask(idx); }


    const auto& dom(){ return matcher_.dom(); }
    const auto& get_level(size_t idx){ return matcher_.get_level(idx); }

    size_t num_prices() {return matcher_.num_prices();}

    size_t bid_idx()  {return matcher_.bid_idx();}
    size_t ask_idx()  {return matcher_.ask_idx();}
};




#endif