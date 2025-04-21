//
// Created by prw on 4/19/25.
//

#ifndef CLIENTORDERLIST_H
#define CLIENTORDERLIST_H

#include <variant>
#include <vector>
#include "ClientOrders/ClientOrders.h"




class ClientOrderList {

    public:
    using ClientOrder = std::variant<   BuyLimitOrder,SellLimitOrder,
                                        BuyStopOrder,SellStopOrder,
                                        BuyMarketOrder,SellMarketOrder,
                                        CancelOrder>;


    std::vector<ClientOrder> orders;


    void append_order(ClientOrder order){orders.push_back(order);}

    size_t find_by_id(ID id)
    {
        auto lower = std::lower_bound(orders.begin(),
            orders.end(),id,
            [](ClientOrder& order, ID id_)
                   {
                       ID oid{};
                       std::visit([&](auto& o){oid = o.id();},order);
                       return oid < id_;
                   });

        return std::distance(orders.begin(),lower);


    }




    double get_price(ID id)
    {
        double price{};

        std::visit([&](auto& o){price=o.price();},orders[find_by_id(id)]);
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

    void update_order_list(auto&& order_updates)
    {
        std::cout << "order update size: "<< order_updates.size() << std::endl;
        /*for (auto& order : order_updates)
            std::cout << "Price: " << order.price_ << std::endl;*/

        for (auto& order : order_updates)
        {
            std::cout << "Price: " << order.price_ << std::endl;
            std::visit([&](auto& o)
            {
                o.price()=order.price_;
                o.state()=order.state_;
                o.qty()=order.qty_;

            },orders[find_by_id(order.id_)]);

        }
    }
};



#endif //CLIENTORDERLIST_H
