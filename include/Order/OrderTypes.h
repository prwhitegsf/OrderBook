//
// Created by prw on 8/12/25.
//

#ifndef ORDERBOOK_ORDERTYPES_H
#define ORDERBOOK_ORDERTYPES_H

#include <typeindex>
#include <variant>
#include <unordered_map>
#include <chrono>

#include "ValueTypes.h"



namespace order
{
    /// @brief the base order
    /// injects the structs below into template
    /// to create a specific types
    template<class... O>
    struct Order
    {
        ID id{};
        Qty qty{};
        Price price{};
    };


    struct LIMIT{};
    struct MARKET{};
    struct BUY;
    struct SELL{};
    struct CANCEL{};
    struct REJECT{};



    /// @ brief converts BuyLimit / SellLimit into more generic Limit
    /// once our BuyLimit / SellLimit orders have been evaluated,
    /// we convert into the more generic limit orders to simplify
    /// container operations
    template<>
    struct Order<LIMIT>
    {
        ID id;
        Qty qty;
        Price price;

        explicit Order(const Order<BUY,LIMIT> o)
            : id(o.id), qty(o.qty), price(o.price){}

        explicit Order(const Order<SELL,LIMIT> o)
            : id(o.id), qty(o.qty), price(o.price){}
    };


    template<>
    struct Order<BUY,MARKET,LIMIT>
    {
        ID id;
        Qty qty;
        Qty full_qty;
        Price price;

        Order(const ID id_t, const Qty qty_t, const Qty full_qty_t, const Price price_t)
            : id(id_t), qty(qty_t), full_qty(full_qty_t), price(price_t){}
    };

    template<>
    struct Order<SELL,MARKET,LIMIT>
    {
        ID id;
        Qty qty;
        Qty full_qty;
        Price price;

        Order(const ID id_t, const Qty qty_t, const Qty full_qty_t, const Price price_t)
            : id(id_t), qty(qty_t), full_qty(full_qty_t), price(price_t){}
    };


    using Market = Order<MARKET>;
    using Limit = Order<LIMIT>;
    using BuyLimit = Order<BUY,LIMIT>;
    using SellLimit = Order<SELL,LIMIT>;
    using BuyMarket = Order<BUY,MARKET>;
    using SellMarket = Order<SELL,MARKET>;
    using BuyMarketLimit = Order<BUY,MARKET,LIMIT>;
    using SellMarketLimit = Order<SELL,MARKET,LIMIT>;
    using Cancel = Order<CANCEL>;
    using Rejected = Order<REJECT>;

    /// @ brief state of orders in submitted_q
    using Submitted = std::variant< BuyLimit,SellLimit,
                                    BuyMarket,SellMarket,
                                    Cancel>;


    /// @brief state of orders in pending_q
    using Pending = std::variant<   BuyMarketLimit,SellMarketLimit,
                                    BuyLimit,SellLimit,
                                    BuyMarket,SellMarket,
                                    Cancel>;



    /// @brief the various states an order can exist in
    enum class OrderState : short
    {
        SUBMITTED, /// order has been received, but has not entered processing
        PENDING, /// order is in the process of being evaluated
        ACCEPTED, /// order has been evaluated and entered into order book
        REJECTED, /// order has been evaluated and rejected
        PARTIAL, /// order has filled some, but not all of its contracts
        FILLED, /// order has filled all contracts, it is completed
        CANCELLED, /// order was submitted, then cancelled
        EMPTY

    };


    std::string OrderStateToString(auto state)
    {
        switch (state)
        {
        case OrderState::SUBMITTED:
            return "SUBMITTED";
        case OrderState::ACCEPTED:
            return "ACCEPTED";
        case OrderState::REJECTED:
            return "REJECTED";
        case OrderState::PENDING:
            return "PENDING";
        case OrderState::PARTIAL:
            return "PARTIAL";
        case OrderState::FILLED:
            return "FILLED";
        case OrderState::CANCELLED:
            return "CANCELLED";
        case OrderState::EMPTY:
            return "EMPTY";
        default:
            return "UNKNOWN";
        }

    }

    /// @brief utility for looking up order types
    inline std::unordered_map<std::type_index,std::string> order_types
    {
            {std::type_index(typeid(Limit)),"Limit"},
            {std::type_index(typeid(Market)),"Market"},
            {std::type_index(typeid(BuyLimit)),"BuyLimit"},
            {std::type_index(typeid(SellLimit)),"SellLimit"},
            {std::type_index(typeid(BuyMarket)),"BuyMarket"},
            {std::type_index(typeid(SellMarket)),"SellMarket"},
            {std::type_index(typeid(BuyMarketLimit)),"BuyMarketLimit"},
            {std::type_index(typeid(SellMarketLimit)),"SellMarketLimit"},
            {std::type_index(typeid(Cancel)),"Cancel"},
            {std::type_index(typeid(Rejected)),"Rejected"},
            {std::type_index(typeid(Submitted)),"Submitted"},
            {std::type_index(typeid(Pending)),"Pending"}
    };



}

#endif //ORDERBOOK_ORDERTYPES_H