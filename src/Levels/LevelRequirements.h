//
// Created by prw on 4/24/25.
//

#ifndef LEVELREQUIREMENTS_H
#define LEVELREQUIREMENTS_H

template<typename L>
concept Is_Level = requires(L lvl, size_t idx)
{
    lvl.depth();
    lvl.count();

    lvl.begin();
    lvl.end();

    lvl.front();
    lvl.back();

   // lvl.append();
    lvl.pop_front();

    lvl.clear();
    lvl.remove(idx);
    lvl.find(idx);

};



#endif //LEVELREQUIREMENTS_H
