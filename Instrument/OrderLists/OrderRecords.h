//
// Created by prw on 4/19/25.
//

#ifndef CLIENTORDERLIST_H
#define CLIENTORDERLIST_H

#include <variant>
#include <vector>
#include "../../OrderTypes/SubmittedOrderTypes.h"

using ID = unsigned int;

class OrderRecords {

    public:
    using ClientOrder = std::variant<
        SubmittedBuyLimit<Order>, SubmittedSellLimit<Order>,
        SubmittedBuyMarket<Order>,SubmittedSellMarket<Order>,
        SubmittedCancel<Order>>;


    std::vector<ClientOrder> orders;

    void append_order(ClientOrder order) { orders.push_back(order); }

    void print_order(const ID id) { std::visit([&](auto& o){o.print();},orders[find_by_id(id)]); }

    ClientOrder get_order(const ID id) { return orders[find_by_id(id)]; }

    double get_price(const ID id)
    {
        double price{};
        std::visit([&](auto& o){price=o.order.price_;},orders[find_by_id(id)]);
        return price;
    }

    size_t find_by_id(const ID id)
    {
        const auto lower = std::lower_bound(orders.begin(), orders.end(),id,
        [](ClientOrder& order, const ID id_)
               {
                   ID oid{};
                   std::visit([&](auto& o){oid = o.order.id_;},order);
                   return oid < id_;
               });

        return std::distance(orders.begin(),lower);

    }

    void update_order_list(auto& order_updates)
    {

        for (auto order : order_updates)
        {
            std::visit([&](auto& o)
            {
                o.order.price_=order.order.price_;
                o.state_=order.state_;
                o.order.qty_=order.order.qty_;

            },orders[find_by_id(order.order.id_)]);

        }
    }
};



#endif //CLIENTORDERLIST_H
