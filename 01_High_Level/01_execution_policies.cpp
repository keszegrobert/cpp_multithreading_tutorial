#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>
#include <random>
#include <chrono>

// Segédfüggvény időméréshez
template<typename Func>
void measure_time(const std::string& name, Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << name << ": " << elapsed.count() << " ms\n";
}

int main() {
    // 1. Adatok előkészítése
    // Nagyobb adathalmazra van szükség, hogy a párhuzamosítás előnye látszódjon
    const size_t size = 10'000'000; 
    std::vector<int> data(size);
    
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(1, 100);
    
    std::generate(data.begin(), data.end(), [&](){ return dis(gen); });

    // Másolatok készítése a tesztekhez
    auto data_seq = data;
    auto data_par = data;
    auto data_unseq = data;

    std::cout << "Rendezes " << size << " elemen:\n" << "--------------------------------\n";

    // 2. std::execution::seq (Szekvenciális)
    // Ez a hagyományos működés, egy szálon fut.
    measure_time("std::execution::seq", [&](){
        std::sort(std::execution::seq, data_seq.begin(), data_seq.end());
    });

    // 3. std::execution::par (Párhuzamos)
    // Több CPU magot használ. Nagy adatmennyiségnél gyorsabb.
    // Overhead: szálak indítása (thread pool).
    measure_time("std::execution::par", [&](){
        std::sort(std::execution::par, data_par.begin(), data_par.end());
    });

    // 4. std::execution::par_unseq (Párhuzamos és Vektorizált)
    // Több mag + SIMD (Single Instruction Multiple Data).
    // A leggyorsabb elméletben, de a legszigorúbb szabályok vonatkoznak rá.
    measure_time("std::execution::par_unseq", [&](){
        std::sort(std::execution::par_unseq, data_unseq.begin(), data_unseq.end());
    });

    return 0;
}
