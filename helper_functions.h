#pragma once

#include <iostream>
#include <chrono>
#include <fstream>
#include <ranges>
#include <future>
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>

template<typename T>
void print_vector(const std::vector<T>& vec);

inline void set_vector_elements_to_zero(std::vector<int>& vec)
{
    std::fill(vec.begin(), vec.end(), 0);
}

inline auto count_vector_non_zero_elements(const std::vector<int>& vec)
{
    auto it = std::ranges::find_if(vec.rbegin(), vec.rend(), [](char c) { return c != 0; });
    return it == vec.rend() ? 0 : std::distance(vec.begin(), it.base());
}

class ThreadPool
{
    public:
        explicit ThreadPool(size_t num_threads) ;

        template<class F>
        void enqueue(F&& f)
        {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                tasks.emplace(std::forward<F>(f));
            }
            condition.notify_one();
        }

        ~ThreadPool() ;

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop = false;
};
