//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_DEMO_H
#define ORDERBOOK_DEMO_H

#include <iostream>
#include <ranges>

#include "Instrument.h"
#include "Record.h"


namespace printer
{
    std::ostream& print_dom(std::ostream& os, const auto& ob ,Price window=10)
    {

        if (window > ob.num_prices()/10)
        {
            window = ob.num_prices()/10;
            std::cout << "dom_window too large, set to "<< window << std::endl;
        }

        Price low = ob.bid()-window+1;
        constexpr auto fmt_str="{:10} {:>5} {:^3} {:^5} {:^3} {:<5}";
        if (low < 0 || ob.ask() > ob.num_prices() - window)
        {
            os << "out of range"<<"\n";
            return os;
        }

        auto dom = ob.depth(low,ob.ask() + window);

        os<<std::format(fmt_str,"","bid","|","price","|","ask")<<"\n";

        for (int i=dom.size()-1; i >= window; --i)
            os<<std::format(fmt_str,""," ","|", i+low,"|",dom[i])<<"\n";


        for (int i{window-1}; i >=0; --i)
            os<<std::format(fmt_str,"",dom[i],"|",i+low ,"|"," ")<<"\n";
        std::cout<<std::endl;

        return os;
    }

    std::ostream& print_bid_ask(std::ostream& os, const auto& ob)
    {
        os<<std::format("{:13} {:^7} {:^1} {:^7} ","","qty  bid","|","ask  qty")<<"\n";
        os<<std::format("{:13} {:<4} {:<3} {:^1} {:<3} {:<4}",
            "",ob.depth(ob.bid()),ob.bid(),"",ob.ask(),ob.depth(ob.ask()))<<"\n";

        std::cout<<std::endl;
        return os;
    }

    void print_trade_records(auto& rd) {
        std::cout<<std::endl;
        order::record_header(std::cout);
        for (auto& id : rd.last_processed())
        {
            std::cout << rd.find_order_record(id) <<std::endl;
        }
        std::cout<<std::endl;
    }

    void print_completed_orders(auto& rd)
    {
        std::cout<<std::endl;
        std::cout<<"Completed Orders" <<std::endl;
        order::record_header(std::cout);


        for (const auto& record : rd.completed() | std::views::values)
            std::cout<< record <<std::endl;
        std::cout<<std::endl;
    }

    void print_accepted_orders(auto& rd)
    {
        std::cout<<std::endl;
        std::cout<<"Accepted/ Unfilled Orders" <<std::endl;
        order::record_header(std::cout);

        for (const auto& record : rd.accepted() | std::views::values)
            std::cout<< record <<std::endl;
        std::cout<<std::endl;
    }
}





#endif //ORDERBOOK_DEMO_H