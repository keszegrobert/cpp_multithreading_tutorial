#include <iostream>
#include <future>
#include <thread>
#include <chrono>

// Ez a függvény fogja előállítani az adatot
// Megkapja a promise-t, amibe bele kell írnia az eredményt
void producer(std::promise<int> prom) {
    std::cout << "[Producer] Adat eloallitasa folyamatban...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    int result = 42;
    std::cout << "[Producer] Kesz. Ertek beallitasa: " << result << "\n";
    
    // Itt teljesítjük az ígéretet.
    // Ez felébreszti a várakozó future-t.
    prom.set_value(result);
}

void consumer(std::future<int> fut) {
    std::cout << "[Consumer] Varakozas az adatra...\n";
    
    // Blokkol, amíg a promise meg nem kapja az értéket
    int value = fut.get();
    
    std::cout << "[Consumer] Megkaptam az adatot: " << value << "\n";
}

int main() {
    std::cout << "std::promise Demo\n";
    std::cout << "-----------------\n";

    // 1. Készítünk egy Promise-t
    std::promise<int> p;

    // 2. Elkérjük a hozzá tartozó Future-t
    std::future<int> f = p.get_future();

    // 3. Elindítjuk a szálakat
    // A promise-t mozgatni kell (move), mert nem másolható
    std::thread t1(producer, std::move(p));
    std::thread t2(consumer, std::move(f));

    t1.join();
    t2.join();

    return 0;
}
