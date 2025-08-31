//
// Created by prw on 8/23/25.
//

#ifndef ORDERBOOK_OVERWRITINGVECTOR_H
#define ORDERBOOK_OVERWRITINGVECTOR_H

#include <vector>

#include <span>



template<typename T>
class OverwritingVector
{
    std::vector<T> data_;
    size_t end_{};

public:

    explicit OverwritingVector(const size_t size) : data_(size) {}
    explicit OverwritingVector(const size_t size, const size_t end) : data_(size), end_(end) {}

    OverwritingVector() = default;
    ~OverwritingVector() = default;

    // copy
    OverwritingVector(const OverwritingVector& other) : data_(other.data_), end_(other.end_) {}

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
    size_t size()const { return end_; }

    [[nodiscard]]
    bool empty() const { return end_ == 0; }

    void clear() { end_ = 0; }

    void push_back(const T& item);


    T& next()
    {
        ++end_;
        return data_[end_-1];
    }
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
std::span<T> OverwritingVector<T>::take()
{
    auto s = std::span{data_}.subspan(0,end_);
    clear();
    return s;
}


#endif //ORDERBOOK_OVERWRITINGVECTOR_H
