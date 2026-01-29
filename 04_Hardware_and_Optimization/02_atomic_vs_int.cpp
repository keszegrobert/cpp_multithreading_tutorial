#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

// Globális számlálók
int g_counter = 0;
std::atomic<int> g_atomic_counter{0};

const int ITERATIONS = 100000;

void increment_counters() {
    for (int i = 0; i < ITERATIONS; ++i) {
        // NEM biztonságos: Data Race
        // Olvasás -> Növelés -> Írás (több utasítás, megszakítható)
        g_counter++; 
        
        // BIZTONSÁGOS: Atomikus művelet
        // Hardveresen garantáltan oszthatatlan
        g_atomic_counter++;
    }
}

int main() {
    std::cout << "Atomic vs. Plain Int teszt\n";
    std::cout << "--------------------------\n";
    std::cout << "Cel: " << (ITERATIONS * 10) << " darab noveles elerese 10 szallal.\n\n";

    std::vector<std::thread> threads;
    
    // 10 szál indítása
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(increment_counters);
    }

    // Várakozás a befejezésre
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Sima int eredmenye (VARHATOLAG HIBAS): " << g_counter << "\n";
    std::cout << "Atomic int eredmenye (HELYES):         " << g_atomic_counter << "\n";

    if (g_counter != ITERATIONS * 10) {
        std::cout << "\nLathato, hogy a sima int eseten " << (ITERATIONS * 10 - g_counter) 
                  << " db noveles elveszett az adatverseny miatt.\n";
    }

    return 0;
}
