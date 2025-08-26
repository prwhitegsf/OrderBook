//
// Created by prw on 8/12/25.
//

//
// Created by prw on 8/11/25.
//
#include "Evaluator.h"

Evaluator::Evaluator(Dom& dom, std::queue<order::Pending>& pending_q)
                    : d_(dom), pending_q_(pending_q){}

void Evaluator::evaluate_order(order::BuyLimit o) const
{
    if (o.price <= d_.bid) // plain limit order
    {
        place_limit(o);
    }
    else if (o.price < d_.ask) // lifting the bid
    {
        place_limit_inside_spread(o,d_.bid);
    }
    else if (o.price < d_.ask + d_.protection) // Conversion to market limit
    {
        evaluate_order(order::BuyMarket(o.id,o.qty,o.price));
    }
    else // rejection
    {
        push_accepted(order::Rejected(o.id,o.qty,o.price));
    }

}

 void Evaluator::evaluate_order(order::BuyMarket o) const {

    Qty qty = o.qty; // local qty variable we can decrement against
    // if no limit price set, ask + protection is the limit
    const Price limit = !o.price || o.price < d_.ask ? d_.ask + d_.protection : o.price;
    o.price = d_.ask; // this is where we will start pulling orders in the matcher

    while (qty >= d_.dom[d_.ask] && d_.ask <= limit)
    {
        d_.ask += fill_level(d_.ask,qty);
    }

    next_non_zero_ask();

    if (!qty) // if we've completely filled the order, we're done
    {
        push_accepted(o);
    }
    else
    {
        if (d_.ask <= limit) // if we're still below limit, fill the rest at market
        {
            subtract_depth(d_.ask,qty);
            next_non_zero_ask();
            push_accepted(o);
        }
        else // but if we reached limit, convert to market limit
        {
            d_.bid = limit;
            set_depth(d_.bid, qty);
            push_accepted(order::BuyMarketLimit(o.id,o.qty - d_.dom[d_.bid],o.price,d_.dom[d_.bid],d_.bid));
        }
    }
}

 void Evaluator::evaluate_order(order::SellLimit o) const
{
    if (o.price >= d_.ask) // plain limit order
    {
        place_limit(o);
    }
    else if (o.price > d_.bid) // dropping the ask
    {
        place_limit_inside_spread(o,d_.ask);
    }
    else if (o.price > d_.bid - d_.protection) // conversion to market limit
    {
        evaluate_order(order::SellMarket(o.id,o.qty,o.price));
    }
    else // rejected
    {
        push_accepted(order::Rejected(o.id,o.qty,o.price));
    }
}

 void Evaluator::evaluate_order(order::SellMarket o) const
{
    Qty qty = o.qty; // local qty variable we can decrement against
    const Price limit = !o.price || o.price > d_.bid ? d_.bid - d_.protection : o.price;
    o.price = d_.bid; // this is where we will start pulling orders in the matcher

    while (qty >= d_.dom[d_.bid] && d_.bid >= limit)
    {
        d_.bid -= fill_level(d_.bid,qty);
    }

    next_non_zero_bid();

    if (!qty) // if we've completely filled the order, we're done
    {
        push_accepted(o);
    }
    else
    {
        if (d_.bid >= limit) // if we're still above limit, fill the rest at market
        {
            subtract_depth(d_.bid,qty);
            next_non_zero_bid();
            push_accepted(o);
        }
        else // but if we reached limit, convert to market limit
        {
            d_.ask = limit;
            set_depth(d_.ask, qty);
            push_accepted(order::SellMarketLimit(o.id,o.qty - d_.dom[d_.ask],o.price, d_.dom[d_.ask],d_.ask));
        }
    }
}

 void Evaluator::evaluate_order(order::Cancel o) const
{
    subtract_depth(o.price,o.qty);

    if (!d_.dom[o.price])
    {
        if (o.price == d_.ask)
        {
            next_non_zero_ask();
        }
        else if (o.price == d_.bid)
        {
            next_non_zero_bid();
        }
    }
    push_accepted(o);
}


 void Evaluator::set_depth(const Price price, const Qty depth) const { d_.dom[price] = depth; }

 void Evaluator::add_depth(const Price price, const Qty depth) const { d_.dom[price] += depth; }

 void Evaluator::subtract_depth(const Price price, const Qty depth) const {d_.dom[price] -=  depth;}

 void Evaluator::push_accepted(order::Pending&& o) const {
    pending_q_.emplace(o);
}

 void Evaluator::place_limit(order::BuyLimit o) const {
    add_depth(o.price,o.qty);
    push_accepted(order::BuyLimit(o));
}

 void Evaluator::place_limit(order::SellLimit o) const {
    add_depth(o.price,o.qty);
    push_accepted(order::SellLimit(o));
}

 void Evaluator::place_limit_inside_spread(order::BuyLimit o, Price& update_price) const {
    add_depth(o.price,o.qty);
    update_price = o.price;
    push_accepted(order::BuyLimit(o));
}

 void Evaluator::place_limit_inside_spread(order::SellLimit o, Price& update_price) const {
    add_depth(o.price,o.qty);
    update_price = o.price;
    push_accepted(order::SellLimit(o));
}

 Price Evaluator::fill_level(const Price price, Qty& qty) const {
    qty -= d_.dom[price];
    set_depth(price, 0);
    return 1;
}

 void Evaluator::next_non_zero_ask() const {
    while(!d_.dom[d_.ask]
        && d_.ask < d_.dom.size()-1 - d_.protection)
    {
        ++d_.ask;
    }
}

 void Evaluator::next_non_zero_bid() const {
    while (d_.dom[d_.bid] == 0
        && d_.bid > d_.protection)
    {
        --d_.bid;
    }

}

