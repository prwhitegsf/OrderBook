//
// Created by prw on 8/26/25.
//
#include <iostream>


#include "OverwritingVector.h"
#include "MatchedOrders.h"


int main()
{

    OverwritingVector<order::Matched> ov1(10);

    std::cout << ov1[0].limit_fills.size()<<'\n';

    for (auto& lf : ov1[0].limit_fills)
        std::cout << lf << '\n';

    for (auto elm : ov1)
        std::cout << elm.limit_fills.size() << '\n';


    return 0;
}