#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

// Az atomic_flag az EGYETLEN típus, ami lock-free garantáltan minden C++11 platformon.
// Minden más atomic típus alatt lehet, hogy mutex van alacsony szinten (bár modern CPU-n ritka).
class Spinlock {
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock() {
        // test_and_set() atomi módon:
        // 1. Visszaadja a régi értéket
        // 2. Beállítja true-ra
        // Ha true-t kaptunk vissza, az azt jelenti, MÁS már beállította -> pörgünk tovább.
        // Ha false-t kaptunk vissza, mienk a lock!
        // std::memory_order_acquire: biztosítja, hogy a kritikus szakasz műveletei ne kerüljenek a lock ELÉ.
        while (flag.test_and_set(std::memory_order_acquire)) {
            // Busy wait
            // Opcionális: std::this_thread::yield(); (ha kedvesek akarunk lenni a CPU-val)
        }
    }

    void unlock() {
        // Töröljük a flag-et (false-ra állítjuk).
        // std::memory_order_release: biztosítja, hogy a kritikus szakasz minden írása látható legyen, MIELŐTT a lock felszabadul.
        flag.clear(std::memory_order_release);
    }
};

Spinlock spinlock;
int shared_data = 0;

void worker(int id) {
    for (int i = 0; i < 100; ++i) {
        spinlock.lock();
        // --- KRITIKUS SZAKASZ ---
        shared_data++;
        // ------------------------
        spinlock.unlock();
        
        // Pici szünet, hogy más is sorra kerüljön
        // std::this_thread::sleep_for(std::chrono::microseconds(10)); 
    }
}

int main() {
    std::cout << "Atomic Flag Spinlock Demo\n";
    std::cout << "-------------------------\n";

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Vegso ertek (elvileg 1000): " << shared_data << "\n";
    
    // Ellenőrzés
    if (shared_data == 1000) {
        std::cout << "[SIKER] A Spinlock helyesen mukodott.\n";
    } else {
        std::cout << "[HIBA] A Spinlock nem vedte meg az adatot!\n";
    }

    return 0;
}
