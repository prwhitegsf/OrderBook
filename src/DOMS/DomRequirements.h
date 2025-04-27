//
// Created by prw on 4/27/25.
//

#ifndef DOMREQUIREMENTS_H
#define DOMREQUIREMENTS_H


template<typename D>
concept Is_Dom = requires(D dom, size_t idx)
{
   dom.bid();
   dom.ask();
   dom.limit();
   //dom.idx(ptr);

   dom.dom();
   dom.orders(idx);
   dom.depth(idx);



};
#endif //DOMREQUIREMENTS_H
