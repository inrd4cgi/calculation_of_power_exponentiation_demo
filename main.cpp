#include "multi_thread.h"
#include "opencl_implementation.h"
#include <functional>

#include "global_variables.h"

std::vector<int> base(LENGTH), temp(LENGTH),  answer(LENGTH);
std::vector<int> intermediate(2 * LENGTH);
bool flag = false;

void quickPow(std::function<void(std::vector<int>&, std::vector<int>&, std::fstream&)> multiFunc, long long power, std::fstream& file)
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
            multiFunc(base, answer, file);
            std::swap(intermediate, answer);
            set_vector_elements_to_zero(intermediate);
        }
        set_vector_elements_to_zero(temp);
        std::copy(base.begin(), base.end(), temp.begin());
        multiFunc(base, temp, file);
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
    int implementation_choice;

    while (true)
    {
        file.open("data.txt", std::ios::app);

        std::cout << "Please input the power: ";
        file << "Please input the power: ";

        std::cin >> power;
        file << power << "\n\n";

        std::cout << "Choose the implementation: 1 for multi-threading, 2 for parallel computation with OpenCL: ";
        std::cin >> implementation_choice;

        if (implementation_choice == 1)
        {
            quickPow(threads_multiply, power, file);
        }
        else if (implementation_choice == 2)
        {
            quickPow(opencl_multiply, power, file);
        }
        else
        {
            std::cout << "Invalid choice. Please choose 1 or 2.\n";
            continue;
        }

        file.close();
    }
    return 0;
}