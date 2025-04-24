#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <variant>
#include <memory>
#include "../OrderTypes/Orders.h"
#include "../Instrument/Instrument.h"

template <typename Matcher>
class Orderbook
{
    Matcher matcher_;

public:

    std::shared_ptr<Instrument> instrument_;
    explicit Orderbook(std::shared_ptr<Instrument> instrument, Matcher matcher)
        : instrument_(instrument),
        matcher_(matcher){}

    template <typename O>
    requires std::is_base_of_v<QueuedOrderTag,O>
    OrderUpdate SubmitOrder(O&& order)
    {
        OrderUpdate update = matcher_.match(std::forward<O>(order));

        instrument_->update_bid(matcher_.bid_idx());
        instrument_->update_ask(matcher_.ask_idx());
        instrument_->update_client_orders(matcher_.order_updates_);

        matcher_.order_updates_.clear();

        return update;
    }

    void set_bid(size_t idx){ matcher_.set_bid(matcher_.dom_begin()+idx); }
    void set_ask(size_t idx){ matcher_.set_ask(matcher_.dom_begin()+idx); }


    const auto& get_dom(){ return matcher_.get_dom(); }

    size_t num_prices() {return matcher_.num_prices();}

    size_t bid_idx()  {return matcher_.bid_idx();}
    size_t ask_idx()  {return matcher_.ask_idx();}
};




#endif