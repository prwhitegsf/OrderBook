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
#include <fstream>




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
        std::queue<order::Submitted> order_playback_q;


        std::unordered_map<std::string, int> submitted_stats;

        OrderGenerator();

        explicit OrderGenerator(int seed);

        void initialize_orderbook(OrderBook<Fifo>& ob,RecordDepot<order::Record>& rd, Qty initial_order_count, Qty max_qty);
        order::Submitted record_order(order::Submitted&& o);
        order::Submitted make_random_order(OrderBook<Fifo>& ob, RecordDepot<order::Record>& rd, Qty max_qty=10, float sweep_chance =0.1);

        void write_recorded_to_csv()
        {
            std::ofstream orders;
            orders.open("../data/orders.csv");
            while (!recorded_orders.empty())
            {

                std::visit([&](auto& ord)
                {
                        orders<<order::order_types[std::type_index(typeid(ord))]<<"," << ord.id<<","<<ord.qty<<","<<ord.price<<"\n";

                }, recorded_orders.front());

                recorded_orders.pop();
            }
            orders.close();
        }

        void read_recorded_orders_from_csv()
        {
            std::ifstream file("../data/orders.csv");

            if (!file.is_open())
            {
                std::cerr << "Error opening file!"<<std::endl;
                return;
            }

            std::string line;
            std::vector<std::vector<std::string>> data; // To store all rows and columns

            while (std::getline(file, line)) { // Read line by line
                std::stringstream ss(line);  // Create a stringstream from the line
                std::string cell;
                std::vector<std::string> order_string; // To store cells of the current row

                while (std::getline(ss, cell, ',')) { // Extract cells separated by comma
                    order_string.push_back(cell);
                }
                data.push_back(order_string); // Add the parsed row to the data vector
            }

            for (auto& order : data)
            {
                auto type = order.front();
                ID id = std::stoi(order[1]);
                Qty qty = std::stoi(order[2]);
                Price price = std::stoi(order[3]);
                if (type == "BuyLimit")
                {
                    order_playback_q.emplace(order::BuyLimit(id, qty,price));
                }
                else if (type == "SellLimit")
                {
                    order_playback_q.emplace(order::SellLimit(id, qty,price));
                }
                else if (type == "BuyMarket")
                {
                    order_playback_q.emplace(order::BuyMarket(id, qty,price));
                }
                else if (type == "SellMarket")
                {
                    order_playback_q.emplace(order::SellMarket(id, qty,price));
                }
                else if (type == "Cancel")
                {
                    order_playback_q.emplace(order::Cancel(id, qty,price));
                }


            }


        }

        Price reactive_price_generation(const OrderBook<Fifo>& ob)
        {
            Price price{};
            size_t margin=ob.num_prices()/10;
            while (price < margin || price > ob.num_prices() - margin)
            {

                price = normal(ob.mid(), ob.protection()*8);

            }


            price < ob.mid() ?  ++submitted_stats["below mid"] :
                price > ob.mid() ?  ++submitted_stats["above mid"] :
                ++submitted_stats["at mid"];
            return price;
        }

        order::Submitted make_pending_order(OrderBook<Fifo>& ob, RecordDepot<order::Record>& rd, Qty max_qty=10, float sweep_chance =0.1)
        {
            track_min_and_max_prices(ob);
            ++submitted_stats["total"];
            const Price mid = ob.mid();
            const Price price = generate_price(ob);
            const ID id = next_seq_id();

            size_t margin=ob.num_prices()/10;
;
            order::Submitted o{};
            const unsigned short type = uniform(0,10);

            /*if (binomial(0.9)) // randomly check if we need to backfill
            {
                o = backfill(ob, id, max_qty);
            }*/
            /*else if (type <=1 )
            {
                o = make_cancel_order(rd);
            }*/
            if (ob.depth(mid) == 0)
            {
                if (price < mid)
                {
                    o = make_limit_order<order::BuyLimit>(id, max_qty,mid);
                }
                else
                {
                    o = make_limit_order<order::SellLimit>(id, max_qty,mid);
                }
            }
            else if (type > 5 )// types >= 6 are limit orders
            {
                if (ob.bid() < margin )
                {
                    if (ob.count(price) < 100)
                        o = make_market_order<order::BuyMarket>(ob, id, max_qty,0);
                    else
                        o = make_market_order<order::SellMarket>(ob, id, max_qty,0);
                }
                else if (ob.ask() > ob.num_prices()-margin )
                {
                    if (ob.count(price) < 100)
                        o = make_market_order<order::SellMarket>(ob, id, max_qty,0);
                    else
                        o = make_market_order<order::BuyMarket>(ob, id, max_qty,0);

                }
                else if (price < mid)
                {
                    if (ob.count(price) < 100)
                        o = make_limit_order<order::BuyLimit>(id, max_qty,price);
                    else
                        o = make_market_order<order::SellMarket>(ob, id, max_qty,0);


                }
                else
                {
                    if (ob.count(price) < 100)
                        o = make_limit_order<order::SellLimit>(id, max_qty,price);
                    else
                        o = make_market_order<order::BuyMarket>(ob, id, max_qty,0);


                }
            }
            else
            {

                if (mid < margin)
                {
                    o = make_market_order<order::BuyMarket>(ob, id, max_qty,0);
                }
                else if (mid > ob.num_prices()-margin)
                {
                    o = make_market_order<order::SellMarket>(ob, id, max_qty,0);
                }
                else if (price >= mid)
                {
                    o = make_market_order<order::BuyMarket>(ob, id, max_qty,sweep_chance);
                }
                else
                {
                    o = make_market_order<order::SellMarket>(ob, id, max_qty,sweep_chance);
                }

            }

            rd.make_order_record(o);

            return o;

        }
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