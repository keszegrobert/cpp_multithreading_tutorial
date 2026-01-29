#include <iostream>
#include <vector>
#include <numeric>
#include <execution>
#include <chrono>
#include <random>

template<typename Func>
void measure_time(const std::string& name, Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    long long result = func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << name << ": " << elapsed.count() << " ms (Result: " << result << ")\n";
}

int main() {
    const size_t count = 50'000'000;
    std::cout << "Adatok generalasa (" << count << " elem)...\n";
    std::vector<int> numbers(count, 1); // Csupa 1-es, így az összeg 'count' lesz.

    std::cout << "Osszegzes (Summation) teszt:\n--------------------------------\n";

    // 1. Hagyományos accumulate (mindig szekvenciális)
    measure_time("std::accumulate (seq only)", [&](){
        return std::accumulate(numbers.begin(), numbers.end(), 0LL);
    });

    // 2. std::reduce (szekvenciális policy-vel)
    measure_time("std::reduce (seq)", [&](){
        return std::reduce(std::execution::seq, numbers.begin(), numbers.end(), 0LL);
    });

    // 3. std::reduce (párhuzamos policy-vel)
    measure_time("std::reduce (par)", [&](){
        return std::reduce(std::execution::par, numbers.begin(), numbers.end(), 0LL);
    });

    // 4. std::reduce (párhuzamos vektorizált)
    measure_time("std::reduce (par_unseq)", [&](){
        return std::reduce(std::execution::par_unseq, numbers.begin(), numbers.end(), 0LL);
    });

    return 0;
}
