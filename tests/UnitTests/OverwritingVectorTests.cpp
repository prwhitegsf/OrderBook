//
// Created by prw on 8/25/25.
//

#include "gtest/gtest.h"
#include "OverwritingVector.h"


class OVTests : public ::testing::Test
{



};


TEST_F(OVTests, Construct)
{

    const OverwritingVector<int> ov1;
    EXPECT_EQ(ov1.capacity(), 0);
    EXPECT_EQ(ov1.size(), 0);
    EXPECT_TRUE(ov1.empty());

    const OverwritingVector<int> ov2(10);
    EXPECT_EQ(ov2.capacity(), 10);
    EXPECT_EQ(ov2.size(),0);
    EXPECT_TRUE(ov2.empty());

    EXPECT_EQ(ov2[0],0);

}


TEST_F(OVTests, PushBack)
{
    OverwritingVector<int> ov1;
    ov1.push_back(1);

    EXPECT_EQ(ov1.capacity(), 1);
    EXPECT_EQ(ov1.size(), 1);
    EXPECT_FALSE(ov1.empty());
    EXPECT_EQ(ov1[0],1);

    for (int i{2}; i <=10; ++i)
    {
        ov1.push_back(i);
    }


    EXPECT_TRUE(ov1.capacity() >= 10);
    EXPECT_EQ(ov1.size(), 10);
    for (int i{1}; i <=10; ++i)
    {
        EXPECT_EQ(ov1[i-1],i);
    }
}

TEST_F(OVTests, Take)
{
    OverwritingVector<int> ov;
    for (int i{}; i < 10; ++i)
    {
        ov.push_back(i);
    }

    EXPECT_TRUE(ov.capacity() >= 10);
    EXPECT_EQ(ov.size(), 10);
    for (int i{}; i < 10; ++i)
    {
        EXPECT_EQ(ov[i],i);
    }

    std::span s = ov.take();


    EXPECT_EQ(ov.size(), 0);
    EXPECT_EQ(s.size(), 10);
    for (int i{}; i < 10; ++i)
    {
        EXPECT_EQ(s[i],i);
    }
}

TEST_F(OVTests,CopyOp)
{
    OverwritingVector<int> ov1;

    for (int i{}; i < 10; ++i)
    {
        ov1.push_back(i);
    }

    EXPECT_TRUE(ov1.capacity() >= 10);
    EXPECT_EQ(ov1.size(), 10);
    for (int i{}; i < 10; ++i)
    {
        EXPECT_EQ(ov1[i],i);
    }

    OverwritingVector<int> ov2 = ov1;
    EXPECT_TRUE(ov2.capacity() >= 10);
    EXPECT_EQ(ov2.size(), 10);
    for (int i{}; i < 10; ++i)
    {
        EXPECT_EQ(ov2[i],i);
    }
}

TEST_F(OVTests,CopyOpOverwrite)
{
    OverwritingVector<int> ov1;

    for (int i{}; i < 10; ++i)
    {
        ov1.push_back(i);
    }

    OverwritingVector<int> ov2;
    OverwritingVector<int>* ov2_p = &ov2;

    for (int i{}; i < 20; ++i)
    {
        ov2.push_back(i+100);
    }

    EXPECT_TRUE(ov2.capacity() >= 20);
    EXPECT_EQ(ov2.size(), 20);
    for (int i{}; i < 20; ++i)
    {
        EXPECT_EQ(ov2[i],i+100);
    }

    ov2 = ov1;
    EXPECT_EQ(&ov2, ov2_p); // same memory if no capacity-based reallocation
    EXPECT_TRUE(ov2.capacity() >= 20);
    EXPECT_EQ(ov2.size(), 10);
    for (int i{}; i < 10; ++i)
    {
        EXPECT_EQ(ov2[i],i);
    }
}

