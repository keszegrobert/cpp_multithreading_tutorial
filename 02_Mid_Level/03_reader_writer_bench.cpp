#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <chrono>

// Közös adat
int data_store = 0;

// Hagyományos Mutex
std::mutex simple_mtx;

// C++17 Shared Mutex
std::shared_mutex shared_mtx;

const int NUM_READERS = 10;
const int NUM_WRITERS = 1;
const int OPERATIONS = 50000;

// Segédfüggvény picit lassabb munka szimulálására
void simulate_work() {
    volatile int dummy = 0;
    // Pár száz ciklus, hogy legyen értelme a párhuzamosításnak
    for (int i = 0; i < 5000; ++i) { 
        dummy += i;
    }
}

void reader_simple() {
    for (int i = 0; i < OPERATIONS; ++i) {
        std::lock_guard<std::mutex> lock(simple_mtx);
        // Szimulált olvasás (kicsit lassú)
        simulate_work();
    }
}

void writer_simple() {
    for (int i = 0; i < OPERATIONS; ++i) {
        std::lock_guard<std::mutex> lock(simple_mtx);
        data_store++;
        simulate_work();
    }
}

void reader_shared() {
    for (int i = 0; i < OPERATIONS; ++i) {
        // Shared Lock: Többen is bemehetnek ide egyszerre
        std::shared_lock<std::shared_mutex> lock(shared_mtx);
        simulate_work();
    }
}

void writer_shared() {
    for (int i = 0; i < OPERATIONS; ++i) {
        // Unique Lock: Csak ő lehet bent
        std::unique_lock<std::shared_mutex> lock(shared_mtx);
        data_store++;
        simulate_work();
    }
}

int main() {
    std::cout << "Mutex vs Shared_Mutex Benchmark\n";
    std::cout << "Readers: " << NUM_READERS << ", Writers: " << NUM_WRITERS << "\n\n";

    // --- 1. Teszt: Simple Mutex ---
    {
        data_store = 0;
        std::vector<std::thread> threads;
        auto start = std::chrono::high_resolution_clock::now();

        // Indítjuk az írót
        for(int i=0; i<NUM_WRITERS; ++i) threads.emplace_back(writer_simple);
        // Indítjuk az olvasókat
        for(int i=0; i<NUM_READERS; ++i) threads.emplace_back(reader_simple);

        for(auto& t : threads) t.join();

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Simple Mutex ido: " 
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
                  << " ms\n";
    }

    // --- 2. Teszt: Shared Mutex ---
    {
        data_store = 0;
        std::vector<std::thread> threads;
        auto start = std::chrono::high_resolution_clock::now();

        for(int i=0; i<NUM_WRITERS; ++i) threads.emplace_back(writer_shared);
        for(int i=0; i<NUM_READERS; ++i) threads.emplace_back(reader_shared);

        for(auto& t : threads) t.join();

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Shared Mutex ido: " 
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
                  << " ms\n";
    }

    return 0;
}
