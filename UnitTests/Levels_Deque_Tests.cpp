//
// Created by prw on 4/24/25.
//
#include "gtest/gtest.h"
#include <random>
#include <vector>
#include "../src/OrderTypes/SubmittedOrderTypes.h"
#include "../src/Levels/DequeLevel/DequeLevel.h"

unsigned int rand_int(unsigned int min, unsigned int max)
{

    std::random_device rd;
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rd);
}

class DequeLevelTest : public testing::Test
{

public:
    DequeLevel dq;

    Order make_random_order(ID id)
    {
        Qty qty = rand_int(1, 100);

        return {id, qty, 0};
    }

    Order make_show_all_order(ID id, Qty qty)
    {
        return {id, qty, 0};
    }

    void fill_dq(size_t count)
    {
        ID id = rand_int(1, 100);
        while (count--)
        {
            dq.append(make_random_order(id));
            id += rand_int(1, 100);
        }
    }

    std::vector<ID> get_id_list()
    {
        std::vector<ID> ids;
        auto it = std::begin(dq);
        while (it != std::end(dq))
        {
            ids.push_back(it->id_);
            ++it;
        }
        return ids;
    }


};


TEST_F(DequeLevelTest,AppendNew)
{
    Order o1{1,2,3};

    Order o2{10,20,30};

    Order o3{100,200,300};


    dq.append(o1);
    EXPECT_EQ(dq.back().id_,1);
    EXPECT_EQ(dq.back().qty_,2);
    EXPECT_EQ(dq.back().price_,3);


    dq.append(o2);
    EXPECT_EQ(dq.back().id_,10);
    EXPECT_EQ(dq.back().qty_,20);
    EXPECT_EQ(dq.back().price_,30);


    dq.append(o3);
    EXPECT_EQ(dq.back().id_,100);
    EXPECT_EQ(dq.back().qty_,200);
    EXPECT_EQ(dq.back().price_,300);



    EXPECT_EQ(dq.front().id_,1);
    EXPECT_EQ(dq.front().qty_,2);
    EXPECT_EQ(dq.front().price_,3);

    EXPECT_EQ(dq.depth_,222);

}

TEST_F(DequeLevelTest,Count)
{
    fill_dq(10);
    EXPECT_EQ(dq.count(), 10);
}

TEST_F(DequeLevelTest,Depth)
{
    dq.append(make_show_all_order(1,10));
    dq.append(make_show_all_order(2,11));
    dq.append(make_show_all_order(3,12));

    EXPECT_EQ(dq.depth_,33);
}

TEST_F(DequeLevelTest,PopFront)
{
    dq.append(make_show_all_order(1,10));
    dq.append(make_show_all_order(2,11));
    dq.append(make_show_all_order(3,12));

    EXPECT_EQ(dq.front().id_,1);
    EXPECT_EQ(dq.front().qty_,10);

    dq.pop_front();
    EXPECT_EQ(dq.front().id_,2);
    EXPECT_EQ(dq.front().qty_,11);
}

TEST_F(DequeLevelTest,CopyToBack)
{
    dq.append(make_show_all_order(1,10));
    dq.append(make_show_all_order(2,11));
    dq.append(make_show_all_order(3,12));

    EXPECT_EQ(dq.front().id_,1);
    EXPECT_EQ(dq.front().qty_,10);

    EXPECT_EQ(dq.back().id_,3);
    EXPECT_EQ(dq.back().qty_,12);

    dq.copy_to_back(dq.front());

    EXPECT_EQ(dq.back().id_,1);
    EXPECT_EQ(dq.back().qty_,10);
}

TEST_F(DequeLevelTest, Clear)
{
    dq.append(make_show_all_order(1,10));
    dq.append(make_show_all_order(2,11));
    dq.append(make_show_all_order(3,12));

    EXPECT_EQ(dq.front().id_,1);
    EXPECT_EQ(dq.front().qty_,10);

    dq.clear();
    EXPECT_EQ(dq.count(),0);
}

TEST_F(DequeLevelTest,BeginEnd)
{
    dq.append(make_show_all_order(1,10));
    dq.append(make_show_all_order(2,11));
    dq.append(make_show_all_order(3,12));

    EXPECT_EQ(dq.begin()->id_,1);
    EXPECT_EQ((dq.end()-1)->id_,3);
}

TEST_F(DequeLevelTest,Find)
{
    fill_dq(100);
    std::vector<ID> ids = get_id_list();

    for (auto id : ids)
        EXPECT_NE(dq.find(id),dq.end());
}


TEST_F(DequeLevelTest,Remove)
{
    dq.append(make_show_all_order(1,10));
    dq.append(make_show_all_order(2,11));
    dq.append(make_show_all_order(3,12));

    EXPECT_EQ(dq.count(),3);
    EXPECT_EQ((dq.begin()+1)->id_,2);

    dq.remove(2);
    EXPECT_EQ((dq.begin()+1)->id_,3);
}




