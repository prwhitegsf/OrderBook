//
// Created by prw on 4/21/25.
//

#ifndef PRINTER_H
#define PRINTER_H

#include <string>
#include <iostream>
#include <ranges>
#include <vector>
#include <format>

#include "../OrderTypes/Orders.h"


template<typename T>
concept QueuedOrder = requires(T t) { t.reset(); };

template<typename T>
concept QueuedList = std::ranges::range<T> &&
        QueuedOrder<std::ranges::range_value_t<T>>;


struct Print {

    void operator()(std::string item, std::string const& mess="") const {
        std::cout << mess << item <<std::endl;
    }

    template<class T>
    requires std::is_arithmetic_v<T>
    void operator()(T item, std::string const& mess="") const {
        std::cout << mess << item <<std::endl;
    }

    template<class T>
    requires std::predicate<T>
    void operator()(bool item, std::string const& mess="") const {

        std::cout << mess << std::boolalpha <<item <<std::endl;
    }

    template<class T>
    requires std::is_trivially_copyable_v<T>
    void operator()(std::vector<T> item, std::string const& mess="") const {
        std::cout << mess;
        for (auto& i : item)
            std::cout << i << ' ';
        std::cout<<std::endl;
    }

    void operator() (QueuedOrder auto& item, std::string const& mess="") const
    {
        std::cout<< "ID: " << item.id_ << " qty: " << item.qty_ << std::endl;
    }

    void operator() (QueuedList auto& item, std::string const& mess="") const
    {
        for (auto& ord : item)
            std::cout<< "ID: " << ord.id_ << " qty: " << ord.qty_ << std::endl;
    }

    // sent order
    void operator() (OrderUpdate& item, std::string const& mess="") const
    {
        std::cout<< "ID: " << item.order.id_ <<"   State: "<< OrderStateToString(item.state_)<<std::endl;
        std::cout<<" price: " << item.order.price_ << " qty: "<< item.order.qty_ << std::endl;
    }

    // orderbook with all orders
    void ob_all(auto& ob)
    {
        std::cout<<"==================="<<std::endl;
        std::cout << "Ask " << ob.ask_idx() <<" | " <<
            ob.bid_idx() << " Bid" <<std::endl;
        std::cout<<"==================="<<std::endl;
        std::cout << std::format("{:7}","   Price");
        std::cout << std::format("{:7}","   Depth")<<std::endl;

        const auto& dom = ob.dom();
        size_t idx = ob.num_prices();

        while (idx > 0)
        {
            --idx;

            if (idx + 1 == ob.ask_idx())
                std::cout << "......................"<<std::endl;

            if(dom.at(idx).depth_ != 0)
            {
                std::cout << std::format("{:7}", idx);
                std::cout << std::format("{:7}",dom.at(idx).depth_);
                for (auto const& order : dom.at(idx).orders())
                {
                    std::cout << std::format("{:4}", order.qty_);
                }

                std::cout <<std::endl;
            }
        }
    }

    // order from client list
    void submitted_order(ID id, auto instr ,std::string const& mess="") const
    {
        auto c = instr->order_records_;

        std::visit([&](auto& o){o.print();},c.orders[c.find_by_id(id)]);

    }

    void last_n_orders(auto instr, size_t n, std::string const& mess="")
    {
        auto c = instr->order_records_;

        std::for_each_n(c.orders.rbegin(),n,[](auto& order)
        {
            std::cout<<std::endl;
            std::cout << "====================="<<std::endl;
                std::visit([&](auto& o) { o.print(); }, order);
            std::cout << "====================="<<std::endl;
        });
    }

    void all_submitted(auto instr, std::string const& mess="")
    {
        auto c = instr->order_records_;

        for (auto order : c.orders)
        {
            std::visit([&](auto& o){o.print();},order);
            std::cout << std::endl;

            std::cout<<"---------------------"<<std::endl;
            std::cout << std::endl;
        }
    }



};



#endif //PRINTER_H
