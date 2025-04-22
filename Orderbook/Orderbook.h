#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <variant>
#include <memory>
#include "../OrderTypes/Orders.h"
#include "../Instrument/Instrument.h"

template <typename Matcher, typename PriceLadder>
class Orderbook
{

public:




    std::shared_ptr<Instrument> instrument_;

    PriceLadder price_ladder_;

    Matcher matcher_;


    explicit Orderbook(std::shared_ptr<Instrument> instrument, PriceLadder price_ladder,Matcher matcher)
        : instrument_(instrument),
        price_ladder_(std::move(price_ladder)),
        matcher_(matcher){}

    template <typename Order>
    // requires std::is_base_of_v<QueuedOrderTag,Order>
    OrderUpdate SubmitOrder(Order&& order)
    {
        //instrument_->client_order_list_.append_order(order);
        OrderUpdate update = matcher_.match(std::forward<Order>(order),price_ladder_);


        //auto&& update = matcher_.match(std::forward<Order>(order),price_ladder_);

        instrument_->update_bid(price_ladder_.bid_price());
        instrument_->update_ask(price_ladder_.ask_price());
        instrument_->update_client_orders(price_ladder_.order_updates_);

        // price_ladder_.clear_order_updates();


        return update;
    }


    void print_dom()
    {
        std::cout << std::format("{:7}","   Price");
        std::cout << std::format("{:7}","   Depth")<<std::endl;
        for (size_t i{price_ladder_.num_prices_}; i > 0; --i)
        {

            if(price_ladder_.get_level(i-1).depth() != 0)
            {

                std::cout << std::format("{:7}", price_ladder_.price_from_idx(i-1));//,price_ladder_.get_level(i-1).depth());
                std::cout << std::format("{:7}",price_ladder_.get_level(i-1).depth())<<std::endl;;
                //std::cout << get_price_from_index(i-1) << "\t " << price_ladder_.get_level(i-1).depth() << '\n';
            }
        }
    }

    void print_all_orders()
    {
        std::cout << std::format("{:7}","   Price");
        std::cout << std::format("{:7}","   Depth")<<std::endl;

        for (size_t i{price_ladder_.num_prices_}; i > 0; --i)
        {

            if(price_ladder_.level(i-1).depth_ != 0)
            {
                std::cout << std::format("{:7}", price_ladder_.price_from_idx(i-1));//,price_ladder_.get_level(i-1).depth());
                std::cout << std::format("{:7}",price_ladder_.level(i-1).depth_);
                for (auto const& order : price_ladder_.level(i-1).limit_orders_)
                {
                    std::cout << std::format("{:4}", order.qty_);
                }



                std::cout << std::endl;
            }
        }
    }

    /*void generate_orders(const double starting_bid, const double starting_ask, const int levels_to_fill)
    {
        price_ladder_.bid_idx_ = price_ladder_.idx_from_price(starting_bid);
        price_ladder_.ask_idx_ =  price_ladder_.idx_from_price(starting_ask);

        int depth = levels_to_fill * 10;
        for (size_t i{price_ladder_.bid_idx_}; i > 0; --i) {

            for (int j{}; j < depth; ++j)
            {
                SubmitOrder(std::move(BuyLimitOrder((j % 5)+1,(j % 5)+1,price_ladder_.price_from_idx(i),Duration::DAY)));
            }

            depth -= 10;

        }
        depth = levels_to_fill * 10;
        for (size_t i{price_ladder_.ask_idx_}; i <  price_ladder_.idx_from_price(price_ladder_.max_price_); ++i) {

            for (int j{}; j < depth; ++j)
            {
                SubmitOrder(std::move(SellLimitOrder((j % 5)+1,(j % 5)+1,price_ladder_.price_from_idx(i),Duration::DAY)));
            }

            depth -= 10;

        }
    }

    void generate_orders(const double starting_bid, const double starting_ask, std::vector<int> depths)
    {
        price_ladder_.bid_idx_ = price_ladder_.idx_from_price(starting_bid);
        price_ladder_.ask_idx_ =  price_ladder_.idx_from_price(starting_ask);


        size_t i{price_ladder_.bid_idx_};
        for (int depth : depths)
        {
            int j{depth};
            while (j > 0)
            {
                SubmitOrder(BuyLimitOrder(1, 1, price_ladder_.price_from_idx(i), Duration::DAY));
                --j;
            }

            --i;
        }


        i = price_ladder_.ask_idx_;
        for (int depth : depths)
        {
            int j{depth};
            while (j > 0)
            {
                SubmitOrder(SellLimitOrder(1, 1, price_ladder_.price_from_idx(i), Duration::DAY));
                --j;
            }

            ++i;
        }
    }*/
};




#endif