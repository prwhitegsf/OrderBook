//
// Created by prw on 8/23/25.
//

#ifndef ORDERBOOK_OVERWRITINGVECTOR_H
#define ORDERBOOK_OVERWRITINGVECTOR_H

#include <vector>
#include <ranges>
#include <span>
#include <iostream>


template<typename T>
class OverwritingVector : public std::ranges::view_interface<OverwritingVector<T>>
{
    std::vector<T> data_;

    size_t end_{};

public:

    OverwritingVector() = default;
    explicit OverwritingVector(const size_t size) : data_(size) {}


    //OverwritingVector(const OverwritingVector& other) : data_(other.data_), end_(other.end_) {}
    auto begin()const { return data_.begin(); }
    auto end() const { return data_.begin() + end_; }

    //const T& operator[](const size_t index) const { return data_[index]; }

    OverwritingVector& operator=(const OverwritingVector& other)
    {
        if (this != &other)
        {
            end_ = 0;
            for (const auto& item : other.data_)
                push_back(item);
        }

        return *this;
    }

    size_t capacity() const { return data_.size(); }
    bool empty() const { return end_ == 0; }
    void clear() { end_ = 0; }

    /*T& operator[](const size_t idx)
    {
        return data_[idx];
    }*/

    void push_back(const T& item)
    {
        if (end_ >= data_.size()) // trigger vector resize
        {

            //data_[end_] = item;
            data_.push_back(item);
        }
        else
        {

            data_[end_] = item;
        }

        ++end_;
    }

    std::span<T> take()
    {
        auto s = std::span{data_}.subspan(0,end_);
        clear();
        return s;
    }

};


#endif //ORDERBOOK_OVERWRITINGVECTOR_H