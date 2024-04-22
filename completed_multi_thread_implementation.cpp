#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <ranges>
#include <future>
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>

constexpr auto LENGTH = 1000000;
constexpr int BASE = 2;

std::vector<int> base(LENGTH), temp(LENGTH), intermediate(2 * LENGTH), answer(LENGTH);
bool flag = false;

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads)
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

    template<class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for(std::thread &worker: workers) worker.join();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;
};

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

void set_vector_elements_to_zero(std::vector<int>& vec)
{
    std::fill(vec.begin(), vec.end(), 0);
}

auto count_vector_non_zero_elements(const std::vector<int>& vec)
{
    auto it = std::ranges::find_if(vec.rbegin(), vec.rend(), [](char c) { return c != 0; });
    return it == vec.rend() ? 0 : std::distance(vec.begin(), it.base());
}

void multi(std::vector<int>& vec1, std::vector<int>& vec2, std::fstream& file)
{
    auto base_length = count_vector_non_zero_elements(vec1), temp_length = count_vector_non_zero_elements(vec2);

    intermediate.resize(base_length + temp_length + 1); // Resize intermediate to be large enough to hold the result

    ThreadPool pool(std::thread::hardware_concurrency());

    // Calculate the range of indices for each thread
    auto range = base_length / std::thread::hardware_concurrency();
    for(auto t = 0; t < std::thread::hardware_concurrency(); ++t)
    {
        // Calculate the start and end indices for this thread
        auto start = t * range;
        auto end = (t == std::thread::hardware_concurrency() - 1) ? base_length : start + range;

        pool.enqueue([&, start, end]()
        {
            for (auto i = start; i < end; ++i)
            {
                for (auto j = 0LL; j < temp_length; ++j)
                {
                    intermediate[i + j] += vec1[i] * vec2[j];
                    intermediate[i + j + 1] += intermediate[i + j] / 10;
                    intermediate[i + j] %= 10;
                }
            }
        });
    }
}

void quickPow(long long power, std::fstream& file)
{
    set_vector_elements_to_zero(base);
    set_vector_elements_to_zero(temp);
    set_vector_elements_to_zero(intermediate);
    set_vector_elements_to_zero(answer);

    base[0] = BASE;
    answer[0] = 1;

    auto start = std::chrono::high_resolution_clock::now();
    while (power > 0)
    {
        if (power % 2)
        {
            multi(base, answer, file);
            std::swap(intermediate, answer);
            set_vector_elements_to_zero(intermediate);
        }
        set_vector_elements_to_zero(temp);
        std::copy(base.begin(), base.end(), temp.begin());
        multi(base, temp, file);
        std::swap(intermediate, base);
        set_vector_elements_to_zero(intermediate);
        power /= 2;
    }
    auto finish = std::chrono::high_resolution_clock::now();

    std::cout << "\n";
    file << "\n";

    for (auto i = LENGTH - 1; i >= 0; --i)
    {
        if (answer[i] != 0)
        {
            flag = true;
        }
        if (flag)
        {
            std::cout << answer[i];
            file << answer[i];
        }
    }

//    std::cout << "\n";
//    print_vector(answer);
//    std::reverse(answer.begin(), answer.end());
//    print_vector(answer);
//    std::cout << "\n";

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(finish - start);
    std::cout << "\n\nTime: " << duration.count() << " s\n\n";
    file << "\n\nTime: " << duration.count() << " s\n\n";
    flag = false;
}

int main()
{
    std::fstream file("data.txt", std::ios::out);

    long long power;

    while (true)
    {
        file.open("data.txt", std::ios::app);

        std::cout << "Please input the power: ";
        file << "Please input the power: ";

        std::cin >> power;
        file << power << "\n\n";

        quickPow(power, file);

        file.close();
    }
    return 0;
}