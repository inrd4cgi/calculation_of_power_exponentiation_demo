#include "helper_functions.h"


template<typename T>
void print_vector(const std::vector<T>& vec)
{
    std::cout << "[";
    if (!vec.empty())
    {
        auto end = vec.end();
        --end;
        for(auto it = vec.begin(); it != end; ++it)
        {
            std::cout << *it << ", ";
        }
        std::cout << *end;
    }
    std::cout << "]\n";
}


ThreadPool::ThreadPool(size_t num_threads)
{
    for(size_t i = 0; i < num_threads; ++i)
    {
        workers.emplace_back([this]
        {
            while(true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if(this->stop && this->tasks.empty()) return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers) worker.join();
}

