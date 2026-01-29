#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>
#include <cmath>
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

// Egy számításigényes művelet szimulációja
// Pl. kiszámoljuk, hogy egy szám prím-e (nem a leghatékonyabb módon)
bool is_prime_heavy(int n) {
    if (n <= 1) return false;
    for (int i = 2; i <= std::sqrt(n); ++i) {
        // Picit lassítunk a műveleten, hogy láthatóbb legyen a különbség
        // (Valós kódban ezt ne csináld!)
        volatile double garbage = std::sin(i) * std::cos(i); 
        (void)garbage;
        
        if (n % i == 0) return false;
    }
    return true;
}

int main() {
    const int count = 100'000;
    std::vector<int> numbers(count);
    
    // Feltöltjük számokkal 1-től count-ig
    std::iota(numbers.begin(), numbers.end(), 1);
    
    // Eredmények tárolása (csak hogy legyen valami output)
    std::vector<bool> results_seq(count);
    std::vector<bool> results_par(count);

    std::cout << "Primszam ellenorzes " << count << " szamon:\n--------------------------------\n";

    // 1. Szekvenciális for_each
    measure_time("std::for_each (seq)", [&](){
        std::for_each(std::execution::seq, numbers.begin(), numbers.end(), [&](int n) {
            results_seq[n-1] = is_prime_heavy(n);
        });
    });

    // 2. Párhuzamos for_each
    // Mivel minden szám függetlenül ellenőrizhető, ez jól párhuzamosítható.
    measure_time("std::for_each (par)", [&](){
        std::for_each(std::execution::par, numbers.begin(), numbers.end(), [&](int n) {
            // Vigyázat: A results_par írása itt biztonságos, mert:
            // - A 'numbers' elemei egyediek
            // - Minden szál más indexet (n-1) ír a results_par vektorban.
            // Ha közös változót írnánk (pl. push_back), az DATA RACE lenne!
            results_par[n-1] = is_prime_heavy(n);
        });
    });

    // 3. std::for_each_n példa
    // Csak az első 100 elemet dolgozzuk fel
    std::cout << "\nstd::for_each_n (elso 100 elem):\n";
    std::for_each_n(std::execution::par, numbers.begin(), 100, [](int n){
        if (is_prime_heavy(n)) {
            // std::cout << n << " "; // IO művelet párhuzamos kódban lassú lehet és összekuszálódhat!
        }
    });
    std::cout << "(Done)\n";

    return 0;
}
