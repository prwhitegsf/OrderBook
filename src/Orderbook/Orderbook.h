#ifndef ORDERBOOK_H
#define ORDERBOOK_H


#include <memory>
#include "../Instrument/Instrument.h"
#include "../Matchers/MatcherRequirements.h"



template<typename DOM,Is_Matcher<DOM> Matcher>
class Orderbook
{

    Matcher matcher_;
    DOM dom_;

public:
    std::shared_ptr<Instrument> inst_;

    explicit Orderbook(std::shared_ptr<Instrument> instrument, Matcher matcher)
        : inst_(std::move(instrument)), matcher_(matcher),
        dom_({instrument->num_prices(),instrument->bid_idx(),instrument->ask_idx()}){}

    explicit Orderbook(std::shared_ptr<Instrument> instrument, Matcher matcher, DOM dom)
        : inst_(std::move(instrument)), matcher_(matcher),
            dom_(dom){}

    template <typename O>
    requires std::is_base_of_v<QueuedOrderTag,O>
    OrderUpdate SubmitOrder(O&& order)
    {
        const OrderUpdate update = matcher_.match(std::forward<O>(order),dom_);

        inst_->update_bid(dom_.bid());
        inst_->update_ask(dom_.ask());
        inst_->update_order_records(matcher_.order_updates());

        matcher_.clear_updates();


        return update;
    }

    // changed this
    void set_bid(size_t idx){ dom_.set_bid(idx); }
    void set_ask(size_t idx){dom_.set_ask(idx); }


    const auto& dom(){ return dom_.dom(); }
    const auto& orders_at_level(size_t idx){ return dom_.orders(idx); }
    int depth(size_t idx) const { return dom_.depth(idx); }

    size_t num_prices() {return dom_.num_prices();}

    size_t bid_idx()  {return dom_.bid();}
    size_t ask_idx()  {return dom_.ask();}
};




#endif