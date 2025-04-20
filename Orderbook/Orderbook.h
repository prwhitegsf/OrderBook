#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <variant>
#include <memory>
#include "../OrderTypes/ClientOrders/ClientOrders.h"
//#include "../PriceLadders/FixedSizeLadder/FixedSizeLadder.h"
//#include "Fifo.h"
#include "../Instrument/Instrument.h"

template <typename Matcher, typename PriceLadder>
class Orderbook
{

public:
    double min_price_;
    double max_price_;
    double price_increment_;
    size_t num_prices_;

    std::vector<ID> filled_limit_orders_;
    std::vector<QueuedMarketOrder> triggered_stops_;


    std::vector<QueuedMarketOrder> filled_market_orders_;
    std::vector<CancelOrders> eod_cancel_orders_;

    std::shared_ptr<Instrument> instrument_;

    PriceLadder price_ladder_;

    Matcher matcher_;


    explicit Orderbook(std::shared_ptr<Instrument> instrument, PriceLadder price_ladder,Matcher matcher)
        :min_price_(instrument->min_price()),
        max_price_(instrument->max_price()),
        price_increment_(instrument->price_increment()),
        num_prices_(static_cast<size_t>((max_price_ - min_price_)*(1.0/price_increment_))),
        instrument_(instrument), price_ladder_(price_ladder),matcher_(matcher){}

    template <typename Order>
    requires std::derived_from<ClientOrderTag,Order>
    void SubmitOrder(Order&& order)
    {
        matcher_.match(std::forward<Order>(order),*this);

    }

    [[nodiscard]] size_t get_index_from_price(double const price) const {
        return static_cast<size_t>((price - min_price_)/(price_increment_));
    }

    [[nodiscard]] double get_price_from_index(size_t const index) const {
        return (index * price_increment_) + min_price_;
    }

};




#endif