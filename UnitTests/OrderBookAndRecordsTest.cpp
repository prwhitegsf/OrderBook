//
// Created by prw on 4/25/25.
//
#include <random>
#include <vector>
#include "gtest/gtest.h"
#include "../Orderbook/Orderbook.h"
#include "../Instrument/Instrument.h"

#include "../OrderTypes/SubmittedOrderTypes.h"
#include "../PriceLevels/DequeLevel/DequeLevel.h"
#include "../Matchers/FIFO/FifoMatchingStrategy.h"
#include "Printer.h"

using Fifo = FifoMatchingStrategy<Order, DequeLevel>;

class OrderBookAndRecordsTest : public ::testing::Test
{


public:


    Orderbook<Fifo> ob();

    static Orderbook<Fifo> orderbook(const size_t num_prices)
    {
        Fifo const fifo(num_prices);
        return Orderbook<Fifo>(std::make_shared<Instrument>(),fifo);
    }


};

TEST_F(OrderBookAndRecordsTest,SubmitOrder)
{

}