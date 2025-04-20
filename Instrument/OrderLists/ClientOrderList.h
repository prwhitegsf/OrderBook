//
// Created by prw on 4/19/25.
//

#ifndef CLIENTORDERLIST_H
#define CLIENTORDERLIST_H

#include <variant>
#include <vector>

#include "../../OrderTypes/ClientOrders/LimitOrders.h"
#include "../../OrderTypes/ClientOrders/StopOrders.h"
#include "../../OrderTypes/ClientOrders/MarketOrders.h"

/*template <typename Order>
concept HasPrice = requires(Order order) {order.price();};*/

template <typename Ord>
concept HasPrice = requires (Ord order) {order.price();};

class ClientOrderLog {

    public:
    using ClientOrder = std::variant<   BuyLimitOrder,SellLimitOrder,
                                        BuyStopOrder,SellStopOrder,
                                        BuyMarketOrder,SellMarketOrder>;

//djsjdjds
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
        ClientOrder ord = orders[find_by_id(id)];
        

        std::visit([&](HasPrice<decltype(ord)> auto& o){price=o.price();},orders[find_by_id(id)]);
        return price;
    }

    ClientOrder get_order_id(ID id)
    {
        return orders[find_by_id(id)];
    }
};



#endif //CLIENTORDERLIST_H
