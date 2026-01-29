#include <iostream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <chrono>

// Egyszerű C++17 Counting Semaphore implementáció
class Semaphore {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;

public:
    explicit Semaphore(int count_ = 0) : count(count_) {}

    // P (Proberen/Wait/Acquire) - Csökkenti a számlálót, ha > 0. Ha 0, várakozik.
    void acquire() {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Várakozunk, amíg a számláló > 0 lesz.
        // A cv.wait() atomi módon elengedi a lockot, és várakozó állapotba teszi a szálat.
        // Amikor felébred, visszaszerzi a lockot és újra ellenőrzi a feltételt (spurious wakeup ellen).
        cv.wait(lock, [this] { return count > 0; });
        
        count--;
    }

    // V (Verhogen/Signal/Release) - Növeli a számlálót és felébreszt egy várakozót.
    void release() {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        // lock.unlock(); // Opcionális: hamarabb elengedhetjük a lockot, de nem kötelező itt
        cv.notify_one(); 
    }
};

Semaphore sm(3); // Egyszerre maximum 3 szál léphet be a kritikus szakaszba

void access_resource(int id) {
    std::cout << "[Thread " << id << "] Varakozik...\n";
    
    sm.acquire(); // Belépés kérése
    
    std::cout << "[Thread " << id << "] BELEPETT.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Munka szimulálása
    std::cout << "[Thread " << id << "] KILEPETT.\n";
    
    sm.release(); // Kilépés jelzése
}

int main() {
    std::cout << "Semaphore Demo C++17\n";
    std::cout << "Max 3 szal futhat egyszerre a kritikus szakaszban.\n";
    std::cout << "------------------------------------------------\n";

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(access_resource, i);
        // Pici késleltetés az indításnál, hogy szebb legyen a log
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
