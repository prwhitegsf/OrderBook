//
// Created by prw on 9/2/25.
//

//
// Created by prw on 9/1/25.
//
#include <chrono>


#include "include/Benchmarks.h"

int main()
{
    using namespace bench;


    const Instrument instrument{"demo",200,99,100,2};


    for (int i{}; i < 4; ++i)
    {
        size_t order_count{1000000};
        constexpr size_t warm_up_count{1000};
        auto orders =
            make_order_sequence(instrument, order_count + warm_up_count);

        OrderBook<Fifo> ob(instrument);
        RecordDepot<order::Record> rd;

        cache_warmup(ob, orders, warm_up_count);

        const auto start = std::chrono::high_resolution_clock::now();

        // Time to process orders
        while (order_count-- && !orders.empty())
        {
            ob.submit_order(orders.front());
            orders.pop();
            ob.evaluate_orders();

            ob.match_orders();

            rd.record_matched_orders(ob.get_matched_orders());


            //rd.update_order_records();
            rd.clear_matched();
        }

        const long dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();

        std::cout<<"Full Processing"<<std::endl;
        std::cout<<"1M orders / "<<dur<<"ms"<<std::endl;
        std::cout<<std::endl;
        std::cout<<"Accepted: "<<rd.accepted().size()<<" Completed: "<<rd.completed().size()<<std::endl;
    }

    return 0;
}
