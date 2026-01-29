#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>

// Segédfüggvény időméréshez
void measure_sleep(int requested_us) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Kért alvás mikroszekundumban
    std::this_thread::sleep_for(std::chrono::microseconds(requested_us));
    
    auto end = std::chrono::high_resolution_clock::now();
    
    // Tényleges idő
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << "Kert: " << requested_us << " us (" << (requested_us/1000.0) << " ms) | "
              << "Kapott: " << elapsed << " us (" << (elapsed/1000.0) << " ms) | "
              << "Kulonbseg: " << (elapsed - requested_us) << " us\n";
}

int main() {
    std::cout << "Sleep Pontossagi Teszt (MacOS/Linux)\n";
    std::cout << "------------------------------------\n";
    std::cout << "Megfigyelheto, hogy kis idoknel a rendszer 'oscheduler' overheadje jelentős.\n\n";

    std::cout << "--- sleep_for(1 microseconds) ---\n";
    for(int i=0; i<3; ++i) measure_sleep(1);

    std::cout << "\n--- sleep_for(1 millisecond) ---\n";
    for(int i=0; i<3; ++i) measure_sleep(1000);

    std::cout << "\n--- sleep_for(10 milliseconds) ---\n";
    for(int i=0; i<3; ++i) measure_sleep(10000);

    return 0;
}
