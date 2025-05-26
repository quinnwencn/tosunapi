//
// Created by quinn on 5/26/2025.
// Copyright (c) 2025 All rights reserved.
//

#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <mutex>
#include <array>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <optional>

namespace Tosun {

template <typename T, size_t Capacity>
class ThreadSafeQue {
public:
    bool Push(T&& item) {
        std::unique_lock<std::mutex> lock {mutex_};
        if (size_.load() >= Capacity) {
            return false;
        }

        buffer_[tail_] = std::move(item);
        tail_ = (tail_ + 1) % Capacity;
        size_.fetch_add(1);
        cv_.notify_one();
        return true;
    }

    T Pop() {
        std::unique_lock<std::mutex> lock {mutex_};
        cv_.wait(lock, [this] { return size_.load() > 0;});
        return PopInternal();
    }

private:
    T PopInternal() {
        auto item = std::move(buffer_[head_]);
        head_ = (head_ + 1) % Capacity;
        size_.fetch_sub(1);
        return item;
    }

    std::mutex mutex_;
    std::atomic<size_t> size_ {0};
    size_t head_ {0};
    size_t tail_ {0};
    std::condition_variable cv_;
    std::array<T, Capacity> buffer_;
};

}

#endif //THREAD_SAFE_QUEUE_H
