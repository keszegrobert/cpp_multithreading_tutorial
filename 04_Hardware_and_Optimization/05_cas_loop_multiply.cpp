#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

std::atomic<int> value{1};

// Atomi szorzás CAS (Compare-And-Swap) ciklussal
// Mivel nincs std::atomic::fetch_multiply, ezt manuálisan kell megírni.
void atomic_multiply(int multiplier) {
    int expected = value.load(); // 1. Az érték, amire számítunk
    int desired;

    // CAS Loop
    // A compare_exchange_weak atomi módon:
    // - Összehasonlítja 'value'-t 'expected'-del.
    // - HA EGYEZNEK: 'value' = 'desired', visszatér true-val. (Siker!)
    // - HA NEM EGYEZNEK: 'expected' = 'value' (frissíti a mi helyi másolatunkat a legújabbra), visszatér false-val. (Újra kell próbálni)
    do {
        desired = expected * multiplier; // Kiszámoljuk az új értéket a jelenlegi alapján
    } while (!value.compare_exchange_weak(expected, desired));
}

int main() {
    std::cout << "Atomic Compare-And-Swap (CAS) Demo\n";
    std::cout << "----------------------------------\n";
    std::cout << "Kezdo ertek: " << value << "\n";
    
    // 10 szál, mindegyik 2-vel szorozza az értéket
    // Eredmény: 2^10 = 1024
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(atomic_multiply, 2);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Vegso ertek: " << value << "\n";

    if (value == 1024) {
        std::cout << "[SIKER] A CAS hurok helyesen mukodott (2^10 = 1024).\n";
    } else {
        std::cout << "[HIBA] Valami felrement (Elveszett frissites).\n";
    }

    return 0;
}
