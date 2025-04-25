//
// Created by prw on 4/24/25.
//

#ifndef MATCHERREQUIREMENTS_H
#define MATCHERREQUIREMENTS_H

#include "../OrderTypes/Orders.h"

template<typename M>
concept Is_Matcher = requires(M m, Cancel<Order> o)
{

  //  m.match(std::move(o));
    /*m.dom();
    m.num_prices();
    m.begin();
    m.end();

    m.ask_idx();
    m.bid_idx();*/

    m.order_updates();

};
#endif //MATCHERREQUIREMENTS_H
