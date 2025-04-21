//
// Created by prw on 4/19/25.
//

#ifndef QUEUEDORDERS_H
#define QUEUEDORDERS_H

using ID = unsigned long;

struct QueuedLimitOrder {
    ID id_;
    int qty_;
    int display_;

    QueuedLimitOrder()
        : id_(0),qty_(0),display_(0){}

    QueuedLimitOrder(ID id, int qty, int display)
        : id_(id),qty_(qty),display_(display){}

    void reset()
    {
        id_ = 0;
        qty_ = 0;
        display_ = 0;
    }
};


struct QueuedMarketOrder {
    ID id_;
    int qty_;
    double price_;

    QueuedMarketOrder()
        : id_(0),qty_(0),price_(0){}

    QueuedMarketOrder(ID id, int qty)
        : id_(id),qty_(qty),price_(0){}

    void reset()
    {
        id_ = 0;
        qty_ = 0;
        price_ = 0;
    }
};


#endif //QUEUEDORDERS_H
