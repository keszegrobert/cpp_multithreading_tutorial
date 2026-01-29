#include <iostream>
#include <vector>
#include <numeric>
#include <execution>
#include <chrono>
#include <cmath>
#include <iomanip>

template<typename Func>
void measure_time(const std::string& name, Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << name << ": " << elapsed.count() << " ms\n";
}

int main() {
    const size_t count = 10'000'000;
    std::vector<double> v1(count, 1.0);
    std::vector<double> v2(count, 2.0);

    std::cout << "1. Transform Reduce (Map-Reduce) - Dot Product\n";
    std::cout << "----------------------------------------------\n";

    // Skalár szorzat: v1[i] * v2[i] összegzése
    
    // Szekvenciális
    measure_time("std::transform_reduce (seq)", [&](){
        double res = std::transform_reduce(std::execution::seq, 
                                          v1.begin(), v1.end(), 
                                          v2.begin(), 
                                          0.0);
        // std::cout << "Result: " << res << "\n";
    });

    // Párhuzamos
    measure_time("std::transform_reduce (par)", [&](){
        double res = std::transform_reduce(std::execution::par, 
                                          v1.begin(), v1.end(), 
                                          v2.begin(), 
                                          0.0);
        // std::cout << "Result: " << res << "\n";
    });


    std::cout << "\n2. Inclusive Scan (Prefix Sum)\n";
    std::cout << "--------------------------------\n";
    // Mivel a scan írja a kimeneti vektort, és minden elem függ az előzőtől,
    // ez egy nehezen párhuzamosítható műveletnek tűnik, de a modern algoritmusok
    // (pl. Hillis-Steele vagy Blelloch scan) képesek rá.

    std::vector<int> input(count, 1);
    std::vector<int> output_seq(count);
    std::vector<int> output_par(count);

    measure_time("std::inclusive_scan (seq)", [&](){
        std::inclusive_scan(std::execution::seq, input.begin(), input.end(), output_seq.begin());
    });

    measure_time("std::inclusive_scan (par)", [&](){
        std::inclusive_scan(std::execution::par, input.begin(), input.end(), output_par.begin());
    });

    // Ellenőrzés (az utolsó elemnek 'count'-nak kell lennie)
    std::cout << "Seq last elem: " << output_seq.back() << "\n";
    std::cout << "Par last elem: " << output_par.back() << "\n";

    return 0;
}
