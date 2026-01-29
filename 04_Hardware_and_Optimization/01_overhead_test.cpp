#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>
#include <chrono>
#include <iomanip>

template<typename Func>
double measure_time_ns(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::nano>(end - start).count();
}

int main() {
    std::cout << "Párhuzamosítási Overhead Teszt\n";
    std::cout << "--------------------------------\n";
    std::cout << "Kicsi adathalmazoknál a párhuzamosítás költsége (szálak indítása, feladatkiosztás)\n";
    std::cout << "gyakran meghaladja a nyereséget.\n\n";

    std::vector<int> sizes = {10, 100, 1000, 10000, 100000};
    
    std::cout << std::left << std::setw(10) << "Size" 
              << std::setw(15) << "Seq (ns)" 
              << std::setw(15) << "Par (ns)" 
              << "Winner" << "\n";
    std::cout << std::string(50, '-') << "\n";

    for (int size : sizes) {
        std::vector<int> v(size, 1);
        
        // Bemelegítés
        std::for_each(std::execution::seq, v.begin(), v.end(), [](int& i){ i++; });

        // Mérés
        double t_seq = measure_time_ns([&](){
            std::for_each(std::execution::seq, v.begin(), v.end(), [](int& i){ 
                i *= 2; 
            });
        });

        double t_par = measure_time_ns([&](){
            std::for_each(std::execution::par, v.begin(), v.end(), [](int& i){ 
                i *= 2; 
            });
        });

        std::cout << std::left << std::setw(10) << size 
                  << std::setw(15) << (long long)t_seq 
                  << std::setw(15) << (long long)t_par;
        
        if (t_par > t_seq) {
            std::cout << "SEQ (Par cost: " << std::fixed << std::setprecision(1) << t_par/t_seq << "x)\n";
        } else {
            std::cout << "PAR\n";
        }
    }

    return 0;
}
