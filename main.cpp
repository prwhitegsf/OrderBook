#include <iostream>
#include <variant>
#include <algorithm>
#include "OrderTypes/ClientOrders/ClientOrders.h"
#include "PriceLadders/FixedSizeLadder/FixedSizeLadder.h"
#include "Orderbook/Orderbook.h"
#include "Matchers/FIFO/Fifo.h"
#include "Instrument/Instrument.h"

int main()
{

    auto instrument = std::make_shared<Instrument>();
    Fifo<FixedSizeLadder> fifo;
    Orderbook<Fifo<FixedSizeLadder>,FixedSizeLadder> ob(instrument,FixedSizeLadder(instrument),fifo);

    ob.generate_orders(10.0,10.25,4);


    ob.SubmitOrder(std::move(BuyStopOrder(11,10.50,Duration::DAY)));

    ob.SubmitOrder(std::move(BuyMarketOrder(121)));

    ob.print_all_orders();

    std::deque<QueuedLimitOrder>& level = ob.price_ladder_.ask().limit_orders_;


}