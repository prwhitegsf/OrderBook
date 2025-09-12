//
// Created by prw on 8/12/25.
//

//
// Created by prw on 8/8/25.
//


#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>

#include "OrderGenerator.h"


namespace gen
{

    OrderGenerator::OrderGenerator() {
        seed_engine(std::time(nullptr));
    }

    OrderGenerator::OrderGenerator(int seed) {
        seed_engine(seed);
    }



    order::Submitted OrderGenerator::record_order(order::Submitted&& o)
    {
        recorded_orders.emplace(o);
        return o;
    }

    std::queue<order::Submitted> OrderGenerator::update_recorded_cancel_order_prices(
        RecordDepot<order::Record>& rd)
    {
        std::queue<order::Submitted> updated_q;

        while (!recorded_orders.empty())
        {
            order::Submitted o = recorded_orders.front();
            if (order::Cancel* co = std::get_if<order::Cancel>(&o))
            {
                auto rec = rd.find_order_record(co->id);

                if (rec.id)
                    co->price = rec.limit_price;
                else
                    throw std::runtime_error("Could not find order record");

                updated_q.emplace(order::Cancel(*co));
            }
            else
                updated_q.emplace(o);

            recorded_orders.pop();
        }



        return updated_q;
    }

    order::Submitted OrderGenerator::make_random_order(
        const OrderBook<Fifo>& ob, RecordDepot<order::Record>& rd,
        const Qty max_qty, const float sweep_chance)
    {

        track_min_and_max_prices(ob);
        ++submitted_stats["total"];
        const Price mid = ob.mid();
        const Price price = generate_price(ob);
        const ID id = next_seq_id();

        order::Submitted o{};
        const unsigned short type = uniform(0,10);

        if (binomial(0.5)) // randomly check if we need to backfill
        {
            o = backfill(ob, id, max_qty);
        }
        else if (type <=1 ) // cancel orders
        {
            o = make_cancel_order(rd);
        }
        else if (type <= 5) // market orders
        {
            o = price >= mid ?
                make_market_order<order::BuyMarket>(ob, id, max_qty,sweep_chance) :
                make_market_order<order::SellMarket>(ob, id, max_qty,sweep_chance);
        }
        else// limit orders
        {
            o = price < mid ?
                make_limit_order<order::BuyLimit>(id, max_qty,price) :
                make_limit_order<order::SellLimit>(id, max_qty,price);
        }

        rd.make_order_record(o); // submit order to record depot

        return o;
    }

    void OrderGenerator::print_submitted_stats()
    {

        auto key_view = submitted_stats | std::views::keys;
        std::vector<std::string> sorted_keys;
        std::ranges::copy(key_view, std::back_inserter(sorted_keys));
        std::ranges::sort(sorted_keys);

        for (auto& k : sorted_keys)
            std::cout<<k<<' '<<submitted_stats.at(k)<<std::endl;
    }


    ID OrderGenerator::next_seq_id()
    {
        return ++id_;
    }

    void OrderGenerator::seed_engine(auto&& s)
    {
        engine_.seed(s);
    }

    Qty OrderGenerator::normal(const int mean, const int st_dev)
    {
        Qty norm{};
        while (!norm)
            norm = static_cast<Qty>(std::normal_distribution<> (mean, st_dev)(engine_));
        return norm;
    }

    Qty OrderGenerator::uniform(const int min, const int max)
    {
        Qty uni{};
        while (!uni)
            uni = static_cast<Qty>(std::uniform_int_distribution<> (min, max)(engine_));
        return uni;
    }

    bool OrderGenerator::binomial(const double prob)
    {
        return std::binomial_distribution<> (1, prob)(engine_) - 1/2;
    }


    void OrderGenerator::make_dom(const OrderBook<Fifo>& ob, const Qty initial_order_count)
    {
        submitted_stats["min_price"] = ob.bid();
        submitted_stats["max_price"] = ob.ask();
        local_dom_.resize(ob.num_prices());

        for (int i{}; i < initial_order_count; ++i)
        {
            const Price price = normal(ob.mid(),ob.mid()/4);

            if (price < local_dom_.size())
                ++local_dom_.at(price);
        }
    }
    void OrderGenerator::initialize_orderbook(
           OrderBook<Fifo>& ob,RecordDepot<order::Record>& rd,
           const Qty initial_order_count, const Qty max_qty)
    {
        make_dom(ob,initial_order_count);
        make_initial_orders(ob,max_qty);

        while (!orders_q.empty())
        {
            rd.make_order_record(orders_q.front());
            ob.submit_order(orders_q.front());
            orders_q.pop();

            ob.evaluate_orders();
            ob.match_orders();
            rd.record_matched_orders(ob.get_matched_orders());
            rd.update_order_records();
        }
    }

    void OrderGenerator::make_initial_orders(const OrderBook<Fifo>& ob, const Qty max_qty)
    {
        auto make_orders = [&] (int i, auto&& o)
        {
            Qty depth{};
            while (depth < local_dom_[i])
            {
                o.id = next_seq_id();
                o.qty =uniform(1,max_qty);;
                depth += o.qty;
                o.price = i;;
                recorded_orders.emplace(o);
                orders_q.emplace(o);
            }
        };

        for (int i{0}; i <= ob.bid(); ++i)
            make_orders(i,order::BuyLimit());

        for (int i = ob.ask(); i < local_dom_.size(); ++i)
            make_orders(i,order::SellLimit());
    }


    void OrderGenerator::track_min_and_max_prices(const OrderBook<Fifo>& ob)
    {
        if (ob.bid() < submitted_stats["min_price"])
            submitted_stats["min_price"] = ob.bid();

        if (ob.ask() > submitted_stats["max_price"])
            submitted_stats["max_price"] = ob.ask();
    }

    Price OrderGenerator::generate_price(const OrderBook<Fifo>& ob)
    {
        Price price{};

        while (price < ob.min_price() || price > ob.max_price())
            price = normal(ob.mid(), ob.protection()*8);

        price < ob.mid() ?  ++submitted_stats["below mid"] :
            price > ob.mid() ?  ++submitted_stats["above mid"] :
            ++submitted_stats["at mid"];
        return price;
    }


    order::Submitted OrderGenerator::backfill_bid(const OrderBook<Fifo>& ob, const ID id, const Qty qty)
    {
        ++submitted_stats["backfill buy limit"];
        const Price first = ob.bid() - ob.protection(), last=ob.mid();
        auto range = ob.depth(first, last);

        for (int i = last; i >= first; --i)
        {
            if (!ob.depth(i))
            {
                return order::BuyLimit(id,qty,i);

            }
            if (ob.depth(i) > ob.depth(i+1))
            {
                return order::BuyLimit(id,qty,i+1);
            }
        }

        auto min_depth = std::ranges::min_element(range);

        return order::BuyLimit(id,qty,first + std::distance(range.begin(),min_depth));
    }

    order::Submitted OrderGenerator::backfill_ask(const OrderBook<Fifo>& ob, const ID id, const Qty qty)
    {
        ++submitted_stats["backfill sell limit"];
        const Price first = ob.mid()+1, last =ob.ask() + ob.protection() + 1;
        auto range = ob.depth(first, last);

        for (int i = first; i < last; ++i)
        {
            if (!ob.depth(i))
            {
                return order::SellLimit(id,qty,(i));
            }

            if (ob.depth(i) > ob.depth(i-1))
            {
                return order::SellLimit(id,qty,(i)-1);
            }
        }

        auto min_depth = std::ranges::min_element(range);

        return order::SellLimit(id,qty,first + std::distance(range.begin(),min_depth));
    }

    order::Submitted OrderGenerator::backfill(const OrderBook<Fifo>& ob, const ID id, const Qty max_limit_qty)
    {
        const Qty qty = uniform(1,max_limit_qty);

        if (binomial(0.5))
        {
            return backfill_bid(ob, id, qty);
        }

        return backfill_ask(ob,id,qty);
    }


    template <typename O>
    order::Submitted OrderGenerator::make_market_order(
        const OrderBook<Fifo>& ob, const ID id, const Qty max_qty, const float sweep_chance)
    {
        Qty qty{};
        std::string type = order::order_types[std::type_index(typeid(O))];
        if (binomial(binomial(sweep_chance)) != 0)
        {
            ++submitted_stats["sweep"];
            qty += type == "SellMarket" ? ob.max_qty_sell() : ob.max_qty_buy();
        }

        qty += uniform(1,max_qty);

        ++submitted_stats[type];
        submitted_stats[type + " qty"] += qty;

        return O(id,qty,0);
    }

    template <typename O>
    order::Submitted OrderGenerator::make_limit_order(const ID id, const Qty max_qty, const Price price)
    {
        std::string type = order::order_types[std::type_index(typeid(O))];
        Qty qty = uniform(1,max_qty);
        ++submitted_stats[type];
        submitted_stats[type + " qty"] += qty;

        return O(id,qty,price);
    }

    order::Cancel OrderGenerator::make_cancel_order(RecordDepot<order::Record>& rd)
    {
        ID id{};
        Qty rnd = uniform(1,rd.accepted().size()-1);
        for (const auto key : std::views::keys(rd.accepted()))
        {
            id = key;
            if (--rnd == 0)
                break;
        }

        const auto o = rd.find_order_record(id);
        ++submitted_stats["cancel"];

        return {o.id,o.quantities.back(),o.limit_price};
    }


}
