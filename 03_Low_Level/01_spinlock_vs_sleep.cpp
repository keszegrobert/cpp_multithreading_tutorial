#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>

std::atomic<bool> ready{false};
std::mutex mtx;
std::condition_variable cv;

// 1. Stratégia: BUSY WAIT (Spinlock)
// Folyamatosan égeti a CPU-t
void strategy_spin() {
    auto start = std::chrono::high_resolution_clock::now();
    while (!ready.load(std::memory_order_acquire)) {
        // Üres ciklus
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[Spin] Reagalt. Latency: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " us\n";
}

// 2. Stratégia: YIELD
// Kedvesebb a CPU-val, de még mindig pörög
void strategy_yield() {
    auto start = std::chrono::high_resolution_clock::now();
    while (!ready.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[Yield] Reagalt. Latency: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " us\n";
}

// 3. Stratégia: SLEEP
// Próbálunk "kicsit" aludni
void strategy_sleep() {
    auto start = std::chrono::high_resolution_clock::now();
    while (!ready.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "[Sleep] Reagalt. Latency: " 
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " us\n";
}

int main() {
    std::cout << "Varakozasi Strategiak Osszehasonlitasa\n";
    std::cout << "--------------------------------------\n";
    std::cout << "A 'Latency' azt mutatja, mennyi ido telt el a jelzes eszlese ota.\n";
    std::cout << "(Megjegyzes: A meres pontossaga fugg a thread inditasi idotol is, de az aranyok lathatok)\n\n";

    // --- SPIN ---
    ready = false;
    std::thread t1(strategy_spin);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Hagyjuk futni
    // Most jelezzük a "ready"-t, a szál elméletileg azonnal látja
    auto signal_time = std::chrono::high_resolution_clock::now();
    ready = true; 
    t1.join();
    
    // --- YIELD ---
    ready = false;
    std::thread t2(strategy_yield);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ready = true;
    t2.join();

    // --- SLEEP ---
    ready = false;
    std::thread t3(strategy_sleep);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ready = true;
    t3.join();

    return 0;
}
