#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <variant>
#include <memory>
#include "../OrderTypes/Orders.h"
#include "../Instrument/Instrument.h"

template <typename Matcher>
class Orderbook
{

public:

    std::shared_ptr<Instrument> instrument_;



    Matcher matcher_;


    explicit Orderbook(std::shared_ptr<Instrument> instrument, Matcher matcher)
        : instrument_(instrument),
        matcher_(matcher){}

    template <typename Order>
    OrderUpdate SubmitOrder(Order&& order)
    {
        OrderUpdate update = matcher_.match(std::forward<Order>(order));

        instrument_->update_bid(matcher_.bid_idx());
        instrument_->update_ask(matcher_.ask_idx());
        instrument_->update_client_orders(matcher_.order_updates_);

        matcher_.order_updates_.clear();

        return update;
    }




    void print_all_orders(Matcher matcher)
    {
        std::cout << std::format("{:7}","   Price");
        std::cout << std::format("{:7}","   Depth")<<std::endl;

        matcher_.level_ = matcher_.levels_.end()-1;
        while (matcher_.level_ != matcher_.levels_.begin()-1)
        {

            if ((matcher_.level_+1) == matcher_.ask_)
                std::cout << "......................"<<std::endl;

            if(matcher_.level_->depth_ != 0)
            {
                std::cout << std::format("{:7}", std::distance(matcher_.levels_.begin(),matcher_.level_));//,matcher_.get_level(i-1).depth());
                std::cout << std::format("{:7}",matcher_.level_->depth_);
                for (auto const& order : matcher_.level_->limit_orders_)
                {
                    std::cout << std::format("{:4}", order.qty_);
                }



               std::cout <<std::endl;
            }

            --matcher_.level_;

        }
    }




};




#endif