#include "multi_thread.h"

#include "global_variables.h"

void threads_multiply(std::vector<int>& vec1, std::vector<int>& vec2, std::fstream& file)
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