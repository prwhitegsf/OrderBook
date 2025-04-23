//
// Created by prw on 4/19/25.
//

#ifndef CLIENTORDERLIST_H
#define CLIENTORDERLIST_H

#include <variant>
#include <vector>




class ClientOrderList {

    public:
    using ClientOrder = std::variant<
        SubmittedBuyLimit, SubmittedSellLimit,
        SubmittedBuyMarket,SubmittedSellMarket,
        SubmittedCancel>;


    std::vector<ClientOrder> orders;

    void append_order(ClientOrder order)
    {

        orders.push_back(order);
    }

    size_t find_by_id(ID id)
    {
        auto lower = std::lower_bound(orders.begin(),
            orders.end(),id,
            [](ClientOrder& order, ID id_)
                   {
                       ID oid{};
                       std::visit([&](auto& o){oid = o.order.id_;},order);
                       return oid < id_;
                   });

        return std::distance(orders.begin(),lower);

    }




    double get_price(ID id)
    {
        double price{};

        std::visit([&](auto& o){price=o.order.price_;},orders[find_by_id(id)]);
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

        for (auto order : order_updates)
        {
            std::visit([&](auto& o)
            {
                o.order.price_=order.order.price_;
                o.order.state_=order.order.state_;
                o.order.qty_=order.order.qty_;

            },orders[find_by_id(order.order.id_)]);

        }
    }
};



#endif //CLIENTORDERLIST_H
