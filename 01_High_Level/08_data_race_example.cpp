#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>
#include <atomic>

int main() {
    std::vector<int> v(1000, 1);
    
    // 1. Példa: DATA RACE (HIBÁS KÓD)
    // Ha ezt a blokkot kikommentelnénk és futtatnánk, az eredmény véletlenszerű lenne.
    /*
    int counter = 0;
    std::for_each(std::execution::par, v.begin(), v.end(), [&](int i){
        if (i > 0) {
            counter++; // HIBA: Több szál írja egyszerre a counter változót védelem nélkül
        } 
    });
    */

    // 2. Példa: Helyes megoldás atomi változóval (std::execution::par esetén)
    std::atomic<int> atomic_counter{0};
    
    std::for_each(std::execution::par, v.begin(), v.end(), [&](int i){
        if (i > 0) {
            atomic_counter++; // OK: Az atomic művelet thread-safe
        }
    });
    
    std::cout << "Atomic counter result: " << atomic_counter << " (Expected: 1000)" << std::endl;

    // 3. Példa: std::execution::par_unseq korlátja
    // A par_unseq esetében MÉG az atomi változók használata is kerülendő lehet,
    // mert a vektorizáció miatt nem garantált a sorrendiség, és bizonyos architektúrákon
    // vagy implementációkban nem lock-free atomi műveletek problémát okozhatnak.
    // HELYES MEGOLDÁS par_unseq-hez: Olyan algoritmus választása, ami nem igényel mellékhatást (pl. reduce)
    
    return 0;
}
