#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

std::once_flag flag;
std::mutex print_mtx; // Mutex a konzol kimenet szinkronizálására

void init_resource() {
    std::lock_guard<std::mutex> lock(print_mtx);
    std::cout << "[INIT] Erőforrás inicializálása... (Csak egyszer futhat le!)\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Szimulált munka
}

void worker(int id) {
    {
        std::lock_guard<std::mutex> lock(print_mtx);
        std::cout << "[Thread " << id << "] Kéri az erőforrást.\n";
    }
    
    // std::call_once: Garantálja, hogy az 'init_resource' csak egyszer hívódik meg.
    // Ha már fut, a többiek megvárják. Ha már lefutott, a többiek azonnal továbbmennek.
    std::call_once(flag, init_resource);
    
    {
        std::lock_guard<std::mutex> lock(print_mtx);
        std::cout << "[Thread " << id << "] Használja az erőforrást.\n";
    }
}

// "Magic Static" Singleton példa (C++11 óta Thread-Safe)
class Singleton {
public:
    static Singleton& get() {
        // Ez a sor garantáltan thread-safe.
        // Az első szál inicializálja, a többi megvárja.
        static Singleton instance;
        return instance;
    }

    void use(int id) {
        std::lock_guard<std::mutex> lock(print_mtx);
        std::cout << "[Thread " << id << "] Singleton hasznalatban.\n";
    }

private:
    Singleton() {
        std::lock_guard<std::mutex> lock(print_mtx);
        std::cout << "[Singleton] Konstruktor fut.\n";
    }
};

void worker_singleton(int id) {
    Singleton::get().use(id);
}

int main() {
    std::cout << "--- 1. std::call_once Demo ---\n";
    std::vector<std::thread> threads;
    for(int i=0; i<5; ++i) threads.emplace_back(worker, i);
    for(auto& t : threads) t.join();

    std::cout << "\n--- 2. Magic Static Singleton Demo ---\n";
    threads.clear();
    for(int i=0; i<5; ++i) threads.emplace_back(worker_singleton, i);
    for(auto& t : threads) t.join();

    return 0;
}
