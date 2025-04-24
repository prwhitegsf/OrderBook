//
// Created by prw on 4/24/25.
//

#ifndef LEVELREQUIREMENTS_H
#define LEVELREQUIREMENTS_H

template<typename L>
concept Is_Level = requires(L lvl)
{
    lvl.depth_;
    lvl.count();

    lvl.begin();
    lvl.end();

    lvl.front();
    lvl.back();

    lvl.append_new();
    lvl.pop_front();

    lvl.clear();
    lvl.remove();
    lvl.find();

};



#endif //LEVELREQUIREMENTS_H
