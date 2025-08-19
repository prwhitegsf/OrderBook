//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_ORDERGENERATOR_H
#define ORDERBOOK_ORDERGENERATOR_H


//
// Created by prw on 7/30/25.
//

#ifndef ORDERGENERATORS_H
#define ORDERGENERATORS_H

#include <random>

#include "Fifo.h"
#include "OrderBook.h"
#include "OrderTypes.h"
#include "Record.h"
#include "RecordDepot.h"

/// @brief random order generator




namespace gen
{


    class OrderGenerator
    {

    public:
        std::queue<order::Submitted> orders_q;
        std::queue<order::Submitted> recorded_orders;

        std::unordered_map<std::string, int> submitted_stats;

        OrderGenerator();

        explicit OrderGenerator(int seed);

        void initialize_orderbook(OrderBook<Fifo>& ob,RecordDepot<order::Record>& rd, Qty initial_order_count, Qty max_qty);
        order::Submitted record_order(order::Submitted&& o);
        order::Submitted make_random_order(const OrderBook<Fifo>& ob, RecordDepot<order::Record>& rd, Qty max_qty=10, float sweep_chance =0.1);
        order::Submitted make_pending_order(const OrderBook<Fifo>& ob, RecordDepot<order::Record>& rd, Qty max_qty=10, float sweep_chance =0.1);
        void print_submitted_stats();

    private:

        ID id_{1};
        ID next_seq_id();

        std::mt19937 engine_;
        void seed_engine(auto&& s);
        Qty normal(int mean, int st_dev);
        Qty uniform(int min, int max);
        bool binomial(double prob);

        std::vector<Qty> local_dom_;
        void make_dom(const OrderBook<Fifo>& ob, Qty initial_order_count=1000);
        void make_initial_orders(const OrderBook<Fifo>& ob, Qty max_qty);

        void track_min_and_max_prices(const OrderBook<Fifo>&  ob);
        Price generate_price(const OrderBook<Fifo>& ob);
        Price reactive_price_generation(const OrderBook<Fifo>& ob, const size_t margin);

        order::Submitted backfill_bid(const OrderBook<Fifo>& ob, ID id, Qty qty);
        order::Submitted backfill_ask(const OrderBook<Fifo>& ob, ID id, Qty qty);
        order::Submitted backfill(const OrderBook<Fifo>& ob, ID id,Qty max_limit_qty);

        template<typename O>
        order::Submitted make_market_order(const OrderBook<Fifo>&, ID id, Qty max_qty, float sweep_chance=0.1);

        template<typename O>
        order::Submitted make_limit_order(ID id, Qty max_qty, Price price);
        order::Cancel make_cancel_order(RecordDepot<order::Record>& rd);
    };
}



#endif //ORDERGENERATORS_H



#endif //ORDERBOOK_ORDERGENERATOR_H