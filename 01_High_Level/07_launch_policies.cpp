#include <iostream>
#include <future>
#include <thread>
#include <chrono>

void worker(const char* name) {
    std::cout << "[" << name << "] Fut a kovetkezo szalon: " << std::this_thread::get_id() << "\n";
}

int main() {
    std::cout << "std::launch Peldak\n";
    std::cout << "Foszal ID: " << std::this_thread::get_id() << "\n\n";

    // 1. std::launch::async - Azonnali indítás új szálon
    std::cout << "--- 1. Teszt: std::launch::async ---\n";
    auto f1 = std::async(std::launch::async, worker, "ASYNC_TASK");
    // Itt valószínűleg már fut is vagy le is futott, mielőtt a get-et hívjuk
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    f1.get();

    std::cout << "\n--- 2. Teszt: std::launch::deferred ---\n";
    // 2. std::launch::deferred - Halasztott indítás
    // Nem indul el új szál. A függvény akkor fut le, amikor meghívjuk a .get()-et,
    // és a HÍVÓ szálon fut (jelen esetben a főszálon).
    auto f2 = std::async(std::launch::deferred, worker, "DEFERRED_TASK");
    
    std::cout << "[Main] A 'deferred' task letrehozva, de meg nem fut...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "[Main] Most meghivom a .get()-et:\n";
    
    f2.get(); // Itt fut le szinkron módon!

    return 0;
}
