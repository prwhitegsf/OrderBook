//
// Created by prw on 4/24/25.
//

#ifndef ORDERREQUIREMENTS_H
#define ORDERREQUIREMENTS_H

template<typename O>
concept Is_Core_Order = requires(O o)
{
    o.id_;
    o.qty_;
    o.price_;


};

#endif //ORDERREQUIREMENTS_H
