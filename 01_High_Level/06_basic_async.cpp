#include <iostream>
#include <future>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>

// Egy "nehéz" számítási feladat
long long heavy_computation(int id, int depth) {
    std::cout << "[Task " << id << "] Szamitas inditasa a " << std::this_thread::get_id() << " szalon...\n";
    
    // Szimulált terhelés
    long long result = 0;
    for (int i = 0; i < depth; ++i) {
        result += (i * i);
        // Kicsit lassítunk, hogy látszódjon a párhuzamosság
        if (i % (depth / 10) == 0) std::this_thread::yield(); 
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    std::cout << "[Task " << id << "] KESZ. Eredmeny: " << result << "\n";
    return result;
}

int main() {
    std::cout << "std::async Alapok Demo\n";
    std::cout << "Foszal ID: " << std::this_thread::get_id() << "\n\n";

    // 1. Indítsunk el egy feladatot aszinkron módon
    // std::launch::async garantálja, hogy új szálon fut
    std::future<long long> result_future = std::async(std::launch::async, heavy_computation, 1, 10000000);

    // 2. Csináljunk valami mást a főszálon
    std::cout << "[Main] A hatterben folyik a munka, en kozben dolgozom...\n";
    for(int i=0; i<3; ++i) {
        std::cout << "[Main] Tic-Tac " << i << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "[Main] Varok az aszinkron feladat eredmenyere...\n";

    // 3. Eredmény elkérése (ez blokkol, ha a task még nem végzett)
    long long value = result_future.get();

    std::cout << "[Main] Megkaptam az eredmenyt: " << value << "\n\n";

    // ---------------------------------------------------------
    // 4. Kivételkezelés demonstrációja
    // ---------------------------------------------------------
    std::cout << "--- Kivetelkezeles demo ---\n";
    
    // Indítunk egy olyan feladatot, ami szándékosan hibát dob
    auto error_future = std::async(std::launch::async, [](){
        std::cout << "[Task Error] Hiba elokeszitese...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        throw std::runtime_error("Valami sulyos hiba tortent a hatterszalon!");
        return 0; // Sosem érünk ide
    });

    std::cout << "[Main] Hiba-task elinditva, varom a kivetelt...\n";

    try {
        // A .get() hívásakor "újratermelődik" a kivétel a hívó szálon
        error_future.get();
    } catch (const std::exception& e) {
        std::cout << "[Main] Elkaptam a kivetelt: " << e.what() << "\n";
    }

    return 0;
}
