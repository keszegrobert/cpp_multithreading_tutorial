#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

// Egyszerű Barrier osztály
// Arra jó, hogy megállítsa a szálakat, amíg MINDENKI el nem ér a ponthoz.
class Barrier {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int threshold;
    int count;
    int generation; // Generációs számláló a spurious wakeup és a többszörös használat ellen

public:
    explicit Barrier(int n) : threshold(n), count(n), generation(0) {}

    void wait(int id) {
        std::unique_lock<std::mutex> lock(mtx);
        int gen = generation;

        if (--count == 0) {
            // Mindenki megérkezett!
            generation++; // Új generáció indul
            count = threshold; // Reset
            std::cout << "[Barrier] Mindenki megerkezett (" << id << " zarta a sort). INDULAS!\n";
            cv.notify_all(); // Ébresztő mindenkinek
        } else {
            // Várakozás, amíg nem jön el az új generáció
            std::cout << "[Thread " << id << "] Var a soromponal...\n";
            cv.wait(lock, [this, gen] { return gen != generation; });
            std::cout << "[Thread " << id << "] Atlepte a sorompot.\n";
        }
    }
};

void worker_task(int id, Barrier& barrier) {
    // 1. szakasz
    std::cout << "[Thread " << id << "] 1. fazis: Inicializalas...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * id)); // Eltérő munkaidő
    
    // Várakozás a többiekre
    barrier.wait(id);

    // 2. szakasz (Csak akkor indulhat, ha mindenki kész az 1. szakasszal)
    std::cout << "[Thread " << id << "] 2. fazis: Feldolgozas...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int main() {
    const int NUM_THREADS = 5;
    std::cout << "Barrier (Sorompo) Demo " << NUM_THREADS << " szallal\n";
    std::cout << "--------------------------------------\n";

    Barrier barrier(NUM_THREADS);
    std::vector<std::thread> threads;

    for (int i = 1; i <= NUM_THREADS; ++i) {
        threads.emplace_back(worker_task, i, std::ref(barrier));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Program vege.\n";
    return 0;
}
