//
// Created by prw on 4/24/25.
//

#ifndef MATCHERREQUIREMENTS_H
#define MATCHERREQUIREMENTS_H

#include "../OrderTypes/Orders.h"




template<typename M, typename D>
concept Is_Matcher = requires(M m,D dom,Cancel<Order> o)
{
   m.match(std::move(o),dom);


   m.order_updates();

};
#endif //MATCHERREQUIREMENTS_H
