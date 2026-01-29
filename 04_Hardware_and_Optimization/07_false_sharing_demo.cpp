#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <new> // hardware_destructive_interference_size

// Segédfüggvény az időméréshez
template<typename Func>
long long measure_time(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// Konstans a cache line mérethez. C++17-ben elvileg van std::hardware_destructive_interference_size,
// de nem minden fordító támogatja még teljesen, ezért biztonságosabb a 64 bájt használata.
constexpr size_t CACHE_LINE_SIZE = 64;

// 1. ROSSZ STRUKTÚRA: False sharing veszély!
// A két atomic változó közvetlenül egymás után van a memóriában.
// Valószínűleg ugyanazon a 64 bájtos cache line-on osztoznak.
struct BadSharedData {
    std::atomic<long> a{0};
    std::atomic<long> b{0};
};

// 2. JÓ STRUKTÚRA: Padding használata
// Az 'alignas' kulcsszóval vagy padding változókkal távol tartjuk egymástól a változókat.
struct GoodSharedData {
    alignas(CACHE_LINE_SIZE) std::atomic<long> a{0};
    alignas(CACHE_LINE_SIZE) std::atomic<long> b{0};
};

// Munkavégző függvény, ami sokat írja a változót
void heavy_work(std::atomic<long>& counter, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    const int ITERATIONS = 100'000'000;

    std::cout << "=== 14. Fejezet: False Sharing Demonstracio ===" << std::endl;
    std::cout << "Iteraciok szama: " << ITERATIONS << std::endl;
    std::cout << "Cache line meret (feltetelezett): " << CACHE_LINE_SIZE << " byte" << std::endl << std::endl;

    // --- Mérés 1: False Sharing (Rossz) ---
    {
        BadSharedData data;
        std::cout << "Meres 1: Egymas melletti valtozok (False Sharing veszely)..." << std::endl;
        std::cout << "Cim 'a': " << &data.a << std::endl;
        std::cout << "Cim 'b': " << &data.b << std::endl;
        std::cout << "Tavolsag: " << std::abs((long)((char*)&data.b - (char*)&data.a)) << " byte" << std::endl;

        auto time = measure_time([&]() {
            std::thread t1(heavy_work, std::ref(data.a), ITERATIONS);
            std::thread t2(heavy_work, std::ref(data.b), ITERATIONS);
            t1.join();
            t2.join();
        });

        std::cout << "Ido (Rossz): " << time << " ms" << std::endl << std::endl;
    }

    // --- Mérés 2: Padding (Jó) ---
    {
        GoodSharedData data;
        std::cout << "Meres 2: Aligned valtozok (Nincs False Sharing)..." << std::endl;
        std::cout << "Cim 'a': " << &data.a << std::endl;
        std::cout << "Cim 'b': " << &data.b << std::endl;
        std::cout << "tavolsag: " << std::abs((long)((char*)&data.b - (char*)&data.a)) << " byte" << std::endl;

        auto time = measure_time([&]() {
            std::thread t1(heavy_work, std::ref(data.a), ITERATIONS);
            std::thread t2(heavy_work, std::ref(data.b), ITERATIONS);
            t1.join();
            t2.join();
        });

        std::cout << "Ido (Jo): " << time << " ms" << std::endl;
    }

    return 0;
}
