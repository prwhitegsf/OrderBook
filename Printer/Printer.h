//
// Created by prw on 4/21/25.
//

#ifndef PRINTER_H
#define PRINTER_H

#include <string>
#include <iostream>
#include <ranges>
#include <vector>

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

    void operator() (OrderUpdate& item, std::string const& mess="") const
    {
        std::cout<< "ID: " << item._.id_ <<"   State: "<< OrderStateToString(item.state_)<<std::endl;
        std::cout<<" price: " << item._.price_ << " qty: "<< item._.qty_ << std::endl;
    }


};



#endif //PRINTER_H
