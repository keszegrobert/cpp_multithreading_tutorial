#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;
int data = 0;

void worker_thread() {
    std::cout << "[Worker] Elindult. Varakozas a jelzesre...\n";
    
    // 1. Lock megszerzése
    // Fontos: unique_lock kell, nem lock_guard!
    std::unique_lock<std::mutex> lck(mtx);
    
    // 2. Várakozás
    // A lambda a predikátum, ami megvédi a "Spurious Wakeup"-tól.
    // Ez ekvivalens ezzel a ciklussal: while (!ready) { cv.wait(lck); }
    cv.wait(lck, []{ return ready; });
    
    // Amikor ide érünk, a 'ready' biztosan true, és a lock a mienk.
    std::cout << "[Worker] Jelzes megerkezett. Adat feldolgozasa: " << data << "\n";
}

int main() {
    std::cout << "Condition Variable Basics Demo\n";
    std::cout << "------------------------------\n";

    std::thread t(worker_thread);

    std::cout << "[Main] Adat elokeszitese (2 mp)...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    {
        // Kritikus szakasz: adat módosítása
        std::lock_guard<std::mutex> lck(mtx);
        data = 42;
        ready = true;
        std::cout << "[Main] Adat kesz, jelzes kuldese...\n";
    } // Lock elengedése

    // Jelzés küldése
    // Megjegyzés: A notify hívható lock birtoklása nélkül is (ezért van a zárójel után)
    cv.notify_one();

    t.join();
    return 0;
}
