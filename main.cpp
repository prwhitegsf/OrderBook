#include <iostream>
#include <variant>
#include "OrderTypes/ClientOrders/ClientOrders.h"
#include "OrderTypes/QueuedOrders/QueuedOrders.h"
#include <algorithm>
#include "Orderbook/Orderbook.h"
#include "Matchers/FIFO/Fifo.h"
#include "Instrument/Instrument.h"

int main()
{


    BuyLimitOrder buy_order(1,1,10,Duration::DAY);
    SellLimitOrder sell_order(1,1,10.5,Duration::DAY);
    using ClientOrder = std::variant<BuyLimitOrder,SellLimitOrder>;

    using ClientOrderList = std::vector<ClientOrder>;

    ClientOrderList client_orders{buy_order};


    for (int i{}; i < 100; ++i)
    {
        client_orders.emplace_back(BuyLimitOrder(1,1,199-i,Duration::DAY));
        client_orders.emplace_back(SellLimitOrder(1,1,200+i,Duration::DAY));

    }

    ID search_id{};
    std::visit([&](auto& o){search_id=o.id();},client_orders[15]);

    auto lower = std::lower_bound(client_orders.begin(),client_orders.end(),search_id,[](ClientOrder& order, ID id)
    {
        ID oid{};
        std::visit([&](auto& o){oid = o.id();},order);
        return oid < id;
    });

    std::cout <<"Search ID: " <<search_id << " Returned ID: " <<std::endl;
    std::visit([&](auto& o) {o.print();}, client_orders[std::distance(client_orders.begin(),lower)]);





    /*std::visit([&](auto& o){search_id = o.id();},client_orders[15].second);

    auto lb = std::lower_bound(client_orders.begin(),client_orders.end(),search_id,[](const std::pair<ID,ClientOrder>& order, double value)
            {
                ID oid{};

                std::visit([&](auto& o) { oid = o.id();}, order.second);
                return oid < value;
            });

    std::cout <<"Search ID" <<search_id << " Returned ID: "<< lb->first <<std::endl;;*/

    /*for (auto& order : client_orders)
    {
        //std::visit([&](auto& o) {o.print();}, order);
        std::visit([&](auto& o) {std::cout << o.id() <<std::endl;}, order);
    }*/



}

/*,
                    BuyMarketOrder,SellMarketOrder,
                    BuyStopOrder,SellStopOrder*/