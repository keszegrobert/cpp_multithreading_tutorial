#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>
#include <random>
#include <chrono>
#include <string>

struct User {
    int id;
    int score;
    std::string name;

    // Összehasonlító operátor rendezéshez
    bool operator<(const User& other) const {
        if (score != other.score) {
            return score > other.score; // Csökkenő sorrend pontszám alapján
        }
        return id < other.id; // Azonos pontszám esetén ID szerint növekvő
    }
};

template<typename Func>
void measure_time(const std::string& name, Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << name << ": " << elapsed.count() << " ms\n";
}

int main() {
    const int count = 1'000'000;
    std::vector<User> users;
    users.reserve(count);

    std::mt19937 gen(42);
    std::uniform_int_distribution<> score_dis(0, 10000);
    std::uniform_int_distribution<> id_dis(1, count * 2);

    for (int i = 0; i < count; ++i) {
        users.push_back({id_dis(gen), score_dis(gen), "User"});
    }

    auto users_par = users; // Másolat a párhuzamos teszthez
    
    std::cout << "Rendezes " << count << " user objektumon:\n--------------------------------\n";

    // 1. Szekvenciális sort
    measure_time("std::sort (seq)", [&](){
        std::sort(std::execution::seq, users.begin(), users.end());
    });

    // 2. Párhuzamos sort
    // Objektumok mozgatása (swap) drágább lehet, mint int-eknél, 
    // de a párhuzamosítás itt is sokat segít.
    measure_time("std::sort (par)", [&](){
        std::sort(std::execution::par, users_par.begin(), users_par.end());
    });


    // 3. Keresés (std::find_if)
    // Keressünk egy bizonyos pontszámú usert.
    int target_score = 9999;
    
    std::cout << "\nKereses (find_if) score == " << target_score << ":\n--------------------------------\n";

    // Szekvenciális keresés
    measure_time("std::find_if (seq)", [&](){
        auto it = std::find_if(std::execution::seq, users.begin(), users.end(), 
            [target_score](const User& u){ return u.score == target_score; });
        
        if (it != users.end()) {
             // Megtaláltuk
        }
    });

    // Párhuzamos keresés
    // A find_if párhuzamos verziója felosztja a tartományt.
    // FIGYELEM: Ha több elem is megfelel a feltételnek, NEM garantált, hogy a legelső elemet találja meg!
    measure_time("std::find_if (par)", [&](){
        auto it = std::find_if(std::execution::par, users_par.begin(), users_par.end(), 
            [target_score](const User& u){ return u.score == target_score; });
            
        if (it != users_par.end()) {
             // Megtaláltuk
        }
    });

    return 0;
}
