#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex m1;
std::mutex m2;

void threadA_unsafe() {
    // A szál M1-et foglalja, majd M2-t
    std::lock_guard<std::mutex> lg1(m1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Esélyt adunk a másik szálnak
    std::lock_guard<std::mutex> lg2(m2);
    std::cout << "Thread A: Munka kesz\n";
}

void threadB_unsafe() {
    // B szál fordított sorrendben: M2-t, majd M1-et -> DEADLOCK VESZÉLY
    std::lock_guard<std::mutex> lg2(m2);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::lock_guard<std::mutex> lg1(m1);
    std::cout << "Thread B: Munka kesz\n";
}

void threadA_safe() {
    // C++17 megoldás: scoped_lock
    std::scoped_lock lock(m1, m2); // Automatikus deadlock-elkerülő algoritmus
    std::cout << "Thread A (Safe): Munka kesz\n";
}

void threadB_safe() {
    std::scoped_lock lock(m2, m1); // A sorrend itt már mindegy
    std::cout << "Thread B (Safe): Munka kesz\n";
}

int main(int argc, char* argv[]) {
    bool run_unsafe = false;
    if (argc > 1 && std::string(argv[1]) == "unsafe") {
        run_unsafe = true;
    }

    std::cout << "Deadlock Demo\n";
    if (run_unsafe) {
        std::cout << "FIGYELEM: UNSAFE mod. A program valoszinuleg lefagy (deadlock).\n";
        std::cout << "Nyomj Ctrl+C-t a kilepeshez.\n";
        
        std::thread t1(threadA_unsafe);
        std::thread t2(threadB_unsafe);
        t1.join();
        t2.join();
    } else {
        std::cout << "SAFE mod (std::scoped_lock). Minden rendben lesz.\n";
        std::cout << "Futtasd paraméterrel a hibas valtozathoz: ./program unsafe\n";
        
        std::thread t1(threadA_safe);
        std::thread t2(threadB_safe);
        t1.join();
        t2.join();
    }

    return 0;
}
