#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

// A volatile CSAK azt mondja a forditonak, hogy ne optimalizalja ki az olvasast/irast.
// NEM teszi a muveletet atomiva!
volatile int g_volatile_counter = 0;
std::atomic<int> g_atomic_counter{0};

const int ITERATIONS = 100000;

void increment_counters() {
    for (int i = 0; i < ITERATIONS; ++i) {
        // HIBA! A volatile int++ ugyanugy read-modify-write szekvencia,
        // ami megszakithato mas szalak altal.
        g_volatile_counter++; 
        
        g_atomic_counter++;
    }
}

int main() {
    std::cout << "Volatile vs. Atomic Int teszt\n";
    std::cout << "-----------------------------\n";
    std::cout << "Sok nyelvben (Java, C#) a volatile adhat szinkronizacot.\n";
    std::cout << "C++-ban VISZONT NEM! Lássuk a bizonyitekot:\n\n";

    std::vector<std::thread> threads;
    
    // 10 szál indítása, hogy biztosan legyen versenyhelyzet
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(increment_counters);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Volatile int eredmenye (VARHATOLAG HIBAS): " << g_volatile_counter << "\n";
    std::cout << "Atomic int eredmenye (HELYES):             " << g_atomic_counter << "\n";

    if (g_volatile_counter != ITERATIONS * 10) {
        std::cout << "\nTanulsag: A volatile C++-ban NEM alkalmas szalkozi szinkronizaciora!\n";
    }

    return 0;
}
