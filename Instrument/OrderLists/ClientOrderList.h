//
// Created by prw on 4/19/25.
//

#ifndef CLIENTORDERLIST_H
#define CLIENTORDERLIST_H

#include <variant>
#include <vector>
#include "../../OrderTypes/Orders.h"




class ClientOrderList {

    public:
    using ClientOrder = std::variant<
        SubmittedBuyLimit, SubmittedSellLimit,
        SubmittedBuyMarket,SubmittedSellMarket,
        SubmittedCancel>;


    std::vector<ClientOrder> orders;


    void append_order(ClientOrder order){orders.push_back(order);}

    size_t find_by_id(ID id)
    {
        auto lower = std::lower_bound(orders.begin(),
            orders.end(),id,
            [](ClientOrder& order, ID id_)
                   {
                       ID oid{};
                       std::visit([&](auto& o){oid = o._.id_;},order);
                       return oid < id_;
                   });

        return std::distance(orders.begin(),lower);


    }




    double get_price(ID id)
    {
        double price{};

        std::visit([&](auto& o){price=o._.price_;},orders[find_by_id(id)]);
        return price;
    }

    ClientOrder get_order(ID id)
    {
        return orders[find_by_id(id)];
    }

    void print_order(ID id)
    {
        std::visit([&](auto& o){o.print();},orders[find_by_id(id)]);
    }

    void update_order_list(auto& order_updates)
    {
        //std::cout << "order update size: "<< order_updates.size() << std::endl;


        for (auto order : order_updates)
        {
            //std::cout << "Price: " << order._.price_ << std::endl;
            //std::cout << "Idx: " << find_by_id(order._.id_)<< std::endl;
            //size_t my_id = find_by_id(order._.id_);
            //find_by_id(order._.id_);
            std::visit([&](auto& o)
            {
                //std::cout << "LambdaPrice: " << order._.id_ << std::endl;
                o._.price_=order._.price_;
                o.state_=order.state_;
                o._.qty_=order._.qty_;

            },orders[find_by_id(order._.id_)]);

        }
    }

    void print_all()
    {
        for (auto order : orders)
        {
            std::visit([&](auto& o){o.print();},order);
            std::cout << std::endl;

            std::cout<<"---------------------"<<std::endl;
            std::cout << std::endl;
        }
    }
};



#endif //CLIENTORDERLIST_H
