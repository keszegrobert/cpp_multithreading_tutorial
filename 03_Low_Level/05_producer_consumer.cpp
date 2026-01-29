#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <atomic>

// Thread-safe Queue implementáció (ez a lényegében a Producer-Consumer minta magja)
template<typename T>
class SafeQueue {
private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cv_producer; // Teli van-e a sor? (Ha lenne méretkorlát)
    std::condition_variable cv_consumer; // Van-e adat?

public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(value);
        cv_consumer.notify_one(); // Jelezzük a fogyasztónak, hogy van adat
    }

    bool try_pop(T& value, int wait_ms = 100) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Várakozunk, amíg lesz adat, vagy letelik az idő
        if (cv_consumer.wait_for(lock, std::chrono::milliseconds(wait_ms), [this] { return !queue.empty(); })) {
            value = queue.front();
            queue.pop();
            return true;
        }
        return false; // Timeout
    }
};

SafeQueue<int> job_queue;
std::atomic<bool> done{false};

void producer(int id) {
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Munka előállítása
        int job = id * 100 + i;
        job_queue.push(job);
        std::cout << "[Producer " << id << "] Produced job " << job << "\n";
    }
}

void consumer(int id) {
    while (!done || true) { // A 'true' csak azért, hogy a queue ürítését is elvégezzük
        int job;
        if (job_queue.try_pop(job)) {
            std::cout << "  [Consumer " << id << "] Processing job " << job << "...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Feldolgozás
        } else {
            if (done) break; // Ha nincs több producer és a sor is üres, kilépünk
        }
    }
    std::cout << "  [Consumer " << id << "] DONE.\n";
}

int main() {
    std::cout << "Producer-Consumer problem megoldasa C++17 eszkozokkel\n";
    std::cout << "-----------------------------------------------------\n";

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // 2 Producer indítása
    for (int i = 1; i <= 2; ++i) producers.emplace_back(producer, i);
    
    // 3 Consumer indítása
    for (int i = 1; i <= 3; ++i) consumers.emplace_back(consumer, i);

    // Várakozás a producerekre
    for (auto& p : producers) p.join();
    
    // Jelzés, hogy nincs több új munka
    done = true;

    // Várakozás a consumerekre
    for (auto& c : consumers) c.join();

    std::cout << "Minden munka elvegezve.\n";
    return 0;
}
