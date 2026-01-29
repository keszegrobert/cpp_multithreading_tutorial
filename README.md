# C++17 Párhuzamos Programozás Tutorial STL Segítségével

Üdvözöllek ebben a tutorial sorozatban! A célunk, hogy gyakorlati példákon keresztül megismerjük a C++ párhuzamos programozási modelljét, a magas szintű algoritmusoktól egészen a hardver-közeli optimalizációkig.

## Absztrakciós Szintek és Példák

A tananyagot 4 fő könyvtárba rendeztük. A példák **nehézségi sorrendben** vannak számozva (01-től kezdődően).

### I. Magas Szintű Absztrakciók (High-Level)
**Könyvtár:** `01_High_Level/`

*   `01_execution_policies.cpp`: Bevezetés a `seq`, `par`, `par_unseq` házirendekbe.
*   `02_parallel_foreach.cpp`: Egyszerű `for_each` művelet párhuzamosítása.
*   `03_parallel_sort_find.cpp`: Rendezés és keresés.
*   `04_parallel_reduce.cpp`: Numerikus összeadás (Reduce).
*   `05_parallel_scan_transform.cpp`: Összetett transzformációk (Map-Reduce).
*   `06_basic_async.cpp`: `std::async` alapok (Fire-and-forget).
*   `07_launch_policies.cpp`: `std::launch::async` vs `deferred`.
*   `08_data_race_example.cpp`: Mire kell vigyázni? (Adatverseny példa).

---

### II. Közepes Szintű Koordináció (Mid-Level)
**Könyvtár:** `02_Mid_Level/`

*   `01_mutex_example.cpp`: A `std::mutex` alapvető használata.
*   `02_deadlock_example.cpp`: Holtpont (Deadlock) és elkerülése.
*   `03_reader_writer_bench.cpp`: `std::shared_mutex` teljesítményteszt.
*   `04_call_once_demo.cpp`: Szálbiztos egyszeri inicializálás (Singleton).
*   `05_promise_demo.cpp`: Adatküldés szálak között (`std::promise`).
*   `06_exception_propagation.cpp`: Kivételek átvitele szálak között.

---

### III. Alacsony Szintű Szinkronizáció (Low-Level)
**Könyvtár:** `03_Low_Level/`

*   `01_spinlock_vs_sleep.cpp`: Várakozási stratégiák összehasonlítása.
*   `02_sleep_precision.cpp`: Mennyire pontos a `sleep_for`?
*   `03_cv_basics.cpp`: `std::condition_variable` alapok.
*   `04_semaphore_impl.cpp`: Szemafor implementálása saját kézzel.
*   `05_producer_consumer.cpp`: Termelő-Fogyasztó probléma megoldása.
*   `06_barrier_example.cpp`: Szálak szinkronizálása egy ponton (Barrier).

---

### IV. Hardver és Optimalizáció (Hardware-Level)
**Könyvtár:** `04_Hardware_and_Optimization/`

*   `01_overhead_test.cpp`: Mennyibe kerül egy szál indítása?
*   `02_atomic_vs_int.cpp`: `std::atomic` teljesítménye vs sima `int`.
*   `03_volatile_mistake.cpp`: Miért NEM jó a volatile szinkronizációra?
*   `04_atomic_flag_spinlock.cpp`: Spinlock építése `atomic_flag`-ből.
*   `05_cas_loop_multiply.cpp`: Lock-free szorzás (CAS loop).
*   `06_atomic_contention.cpp`: Amikor túl sok szál akarja ugyanazt az atomi változót.
*   `07_false_sharing_demo.cpp`: Cache line thrashing és `alignas`.

---

## Rendszerkövetelmények (Compiler Support)

A "High-Level" (01) és egyes hardveres (04) példák a **C++17 Parallel Algorithms** (`std::execution`) könyvtárat használják. Ennek támogatása fordítónként eltérő:

### Szükséges csomagok
*   **CMake**: 3.15+
*   **Intel TBB (Threading Building Blocks)**: A párhuzamos algoritmusok (pl. `std::sort(std::execution::par, ...)`) futtatásához elengedhetetlen backend a GCC és Clang rendszereken.
    *   **Ubuntu/Debian**: `sudo apt install libtbb-dev`
    *   **macOS**: `brew install tbb`

### Fordítók kompatibilitása
1.  **GCC (Ajánlott Linux/macOS)**: 
    *   Verzió: **9.1+**
    *   Teljes támogatás TBB-vel.
    *   *macOS tipp*: `brew install gcc`
    *   **FONTOS:** macOS Sequoia (és újabb Xcode) alatt a GCC hibát adhat (`_bounds.h missing`).
    *   **Megoldás:** Használd a mellékelt CMake presetet, ami kikapcsolja a rendszer header bekeverését:
        ```bash
        cmake --preset gcc
        cmake --build --preset gcc
        ```
2.  **MSVC (Windows)**: 
    *   Visual Studio 2017 (15.7) vagy újabb.
    *   Kiváló beépített támogatás.
3.  **Clang / Apple Clang**:
    *   **Apple Clang (macOS default)**: Jelenleg **NEM támogatja** a C++17 parallel algoritmusokat.
    *   **LLVM Clang**: A Clang fordító (frontend) az **5.0** verziótól támogatja a C++17 nyelvi elemeit, de a hozzá tartozó *libc++* könyvtárban a párhuzamos algoritmusok támogatása még **kísérleti / hiányos**.
        *   Ha Clang-et szeretnél használni, érdemes a GCC szabványos könyvtárával (`libstdc++`) linkelni.

---

## Fordítás és Futtatás

```bash
mkdir build
cd build
cmake ..
make
```

Futtatás példa:
```bash
./01_High_Level/01_execution_policies
```
