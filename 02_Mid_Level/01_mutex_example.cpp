#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>
#include <chrono>

// Egy egyszerű bankszámla osztály
struct BankAccount {
    int id;
    int balance;
    std::mutex mtx; // Minden számlának saját mutex-e van

    BankAccount(int id, int balance) : id(id), balance(balance) {}

    // A mutex nem másolható, így a copy konstruktort tiltani kell vagy egyedileg implementálni
    BankAccount(const BankAccount&) = delete; 
    BankAccount& operator=(const BankAccount&) = delete;
};

int main() {
    std::cout << "Szinkronizacio: Mutex es lock_guard demo\n";
    std::cout << "5 szal probal penzt befizetni egy kozos szamlara.\n";
    std::cout << "--------------------------------------------------\n";

    BankAccount shared_account(1, 1000);
    std::vector<int> deposits(100, 10); // 100 db 10-es befizetés

    // Párhuzamosan csinálunk 100 befizetést
    std::vector<std::thread> threads;
    for (int i = 0; i < 100; ++i) {
        threads.emplace_back([&shared_account](){
            // Késleltetés szimulálása
            // std::this_thread::sleep_for(std::chrono::microseconds(1));

            // CRITICAL SECTION KEZDETE
            std::lock_guard<std::mutex> lock(shared_account.mtx);
            
            int amount = 10;
            int old_balance = shared_account.balance;
            // Pici munka szimulálása
            shared_account.balance = old_balance + amount;
            
            // CRITICAL SECTION VÉGE
        });
    }

    // Várunk minden szálra
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Vegso egyenleg: " << shared_account.balance << "\n";
    std::cout << "Elvart egyenleg: " << 1000 + (100 * 10) << "\n";

    if (shared_account.balance == 2000) {
        std::cout << "SIKER: Az egyenleg helyes.\n";
    } else {
        std::cout << "HIBA: Adatverseny tortent! (Ha kikapcsoltad a mutexet)\n";
    }

    return 0;
}
