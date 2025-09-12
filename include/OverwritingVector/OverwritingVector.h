//
// Created by prw on 8/23/25.
//

#ifndef ORDERBOOK_OVERWRITINGVECTOR_H
#define ORDERBOOK_OVERWRITINGVECTOR_H

#include <vector>
#include <iostream>
#include <span>



template<typename T>
class OverwritingVector
{
    std::vector<T> data_;
    size_t end_;
    size_t begin_;

public:
    /*explicit OverwritingVector(const size_t size) : data_(size) {}
    explicit OverwritingVector(const size_t size, const size_t end) : data_(size), end_(end) {}*/

    explicit OverwritingVector(const size_t size, const size_t end = 0) : data_(size), end_(end), begin_(0) {}

    OverwritingVector() : data_(), end_(0), begin_(0) {};
    ~OverwritingVector() = default;

    // copy
    OverwritingVector(const OverwritingVector& other) : data_(other.data_), end_(other.end_), begin_(other.begin_) {}

    // no moving allowed
    OverwritingVector(OverwritingVector&& other) = delete;

    // copy assign
    OverwritingVector& operator=(const OverwritingVector& other);

    const T& operator[](const size_t index) const { return data_[index]; }
    T& operator[](const size_t index) { return data_[index]; }

    auto begin()const { return data_.begin(); }
    auto end() const { return data_.begin() + end_; }

    [[nodiscard]]
    size_t capacity() const { return data_.size(); }

    [[nodiscard]]
    size_t size()const { return end_ - begin_; }

    [[nodiscard]]
    bool empty() const { return end_ - begin_ == 0;  }

    void clear() { begin_ = end_ = 0; }

    const T& front() const { return data_[begin_]; }
    const T& back() const { return data_[end_-1]; }

    void push_back(const T& item);
    void pop()
    {
        ++begin_;
        if (begin_ >= end_) clear();
    }

    T& next();

    //unused, but still thinking about it
    std::span<T> take();
};

template <typename T>
OverwritingVector<T>& OverwritingVector<T>::operator=(const OverwritingVector& other) {

    if (this != &other)
    {
        end_ = 0;
        for (const auto& item : other)
            push_back(item);
    }

    return *this;
}


template <typename T>
void OverwritingVector<T>::push_back(const T& item)
{
    if (end_ >= data_.size()) // trigger vector resize
    {
        data_.push_back(item);
    }
    else
    {
        data_[end_] = item;
    }

    ++end_;
}

template <typename T>
T& OverwritingVector<T>::next() {

    ++end_;
    if (end_ == data_.size()) // trigger vector resize
    {
        data_.push_back({});
    }

    return data_[end_-1];
}

template <typename T>
std::span<T> OverwritingVector<T>::take()
{
    auto s = std::span{data_}.subspan(0,end_);
    clear();
    return s;
}


#endif //ORDERBOOK_OVERWRITINGVECTOR_H
