#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>
#include <chrono>
#include <atomic>
#include <numeric>

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
    const int count = 10'000'000;
    std::vector<int> v(count, 1);

    std::cout << "Teljesítmény hiba: Atomi változók túlzott használata\n";
    std::cout << "Feladat: Számoljuk meg a páros számokat (itt mindegyik az...)\n";
    std::cout << "Elemszám: " << count << "\n";
    std::cout << "--------------------------------------------------------\n";

    // 1. Rossz megközelítés: Párhuzamos ciklus + közös atomi változó
    // Ez azért lassú, mert minden szál verseng (contention) ugyanazért a cache line-ért
    // ahol az atomi változó van.
    std::atomic<int> atomic_counter{0};
    measure_time("std::for_each + atomic (High Contention)", [&](){
        std::for_each(std::execution::par, v.begin(), v.end(), [&](int n){
            if (n % 2 != 0) { // Csak a példa kedvéért, bár tudjuk, hogy 1
                atomic_counter++; 
            }
            // Hogy biztos legyen írás, módosítsuk a feltételt
             atomic_counter++;
        });
    });
    // Visszaállítás
    atomic_counter = 0;

    // 2. Jobb megközelítés (Szekvenciális referencia)
    int seq_counter = 0;
    measure_time("Hagyományos ciklus (Seq)", [&](){
        for(int n : v) {
            seq_counter++;
        }
    });

    // 3. Helyes megközelítés: Redukció
    // A szálak lokálisan számolnak, és csak a végén adják össze az eredményeket.
    // Nincs versengés a ciklus magjában.
    measure_time("std::transform_reduce (Correct Parallel)", [&](){
        int result = std::transform_reduce(std::execution::par, 
            v.begin(), v.end(),
            0,
            std::plus<int>{},
            [](int n){ return 1; } // Minden elem 1-et ér a számlálóban
        );
        (void)result;
    });

    return 0;
}
