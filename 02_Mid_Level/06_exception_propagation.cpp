#include <iostream>
#include <future>
#include <thread>
#include <stdexcept>

// Egy függvény, ami néha hibát dob
int risky_calculation(bool fail) {
    if (fail) {
        throw std::runtime_error("Hiba tortent a hatterben!");
    }
    return 123;
}

void manual_promise_exception(std::promise<int> prom) {
    try {
        // Valami munka, ami elromolhat
        throw std::logic_error("Manualis kivetel a promise-bol");
    } catch (...) {
        // Ahelyett, hogy set_value-t hívnánk, beállítjuk a kivételt
        prom.set_exception(std::current_exception());
    }
}

int main() {
    std::cout << "Kivetelkezeles Future-okkel\n";
    std::cout << "---------------------------\n";

    // 1. Eset: std::async kivétel
    std::cout << "\n--- 1. std::async Exception ---\n";
    auto f1 = std::async(std::launch::async, risky_calculation, true);

    try {
        // A .get() fogja újradobni a kivételt
        int res = f1.get();
        std::cout << "Eredmeny: " << res << "\n";
    } catch (const std::exception& e) {
        std::cout << "[ELKAPVA main-ben]: " << e.what() << "\n";
    }

    // 2. Eset: std::promise set_exception
    std::cout << "\n--- 2. std::promise set_exception ---\n";
    std::promise<int> p;
    auto f2 = p.get_future();
    std::thread t(manual_promise_exception, std::move(p));

    try {
        f2.get();
    } catch (const std::exception& e) {
        std::cout << "[ELKAPVA main-ben]: " << e.what() << "\n";
    }
    t.join();

    return 0;
}
