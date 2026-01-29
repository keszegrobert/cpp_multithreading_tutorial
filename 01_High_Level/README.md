# I. Magas Szintű Absztrakciók (High-Level)

Ebben a fejezetben a modern C++ szabványos könyvtárának (STL) magas szintű párhuzamosító eszközeivel ismerkedünk meg. Itt nem kell szálakat manuálisan létrehoznunk vagy szinkronizálnunk; a rendszer "elfedi" előlünk a részleteket.

## Példák magyarázata

### 1. Execution Policies (`01_execution_policies.cpp`)
Ez a példa bemutatja a C++17-ben bevezetett végrehajtási házirendeket:
- `std::execution::seq`: Szekvenciális (soros) végrehajtás.
- `std::execution::par`: Párhuzamos végrehajtás (több szálon).
- `std::execution::par_unseq`: Párhuzamos és vektorizált végrehajtás (SIMD).
A kód ugyanazt a rendezési algoritmust futtatja mindhárom móddal, és méri a teljesítménykülönbséget.

### 2. Párhuzamos Ciklusok (`02_parallel_foreach.cpp`)
A `std::for_each` párhuzamos változatának használata. Bemutatja, hogyan lehet egy egyszerű műveletet (pl. minden elem négyzetre emelése) párhuzamosítani egyetlen extra paraméter hozzáadásával.

### 3. Rendezés és Keresés (`03_parallel_sort_find.cpp`)
Gyakori algoritmusok párhuzamosítása:
- `std::sort`: Nagy adathalmazok rendezése.
- `std::find`: Elem keresése párhuzamosan.
Megmutatja, hogy bizonyos méret alatt a párhuzamosítás overheadje miatt a soros megoldás gyorsabb lehet.

### 4. Numerikus Redukció (`04_parallel_reduce.cpp`)
A `std::reduce` a `std::accumulate` párhuzamos megfelelője.
- Lényeges különbség: A műveletnek asszociatívnak és kommutatívnak kell lennie a helyes végeredményhez, mivel a részösszegek sorrendje nem garantált.

### 5. Transzformáció és Scan (`05_parallel_scan_transform.cpp`)
Komplexebb transzformációk:
- `std::transform_reduce`: A Map-Reduce minta megvalósítása (átalakítás + összegzés egy lépésben).
- `std::inclusive_scan` / `std::exclusive_scan`: Prefix sum (halmozott összeg) számítása párhuzamosan.

### 6. Aszinkron Futtatás Alapjai (`06_basic_async.cpp`)
A `std::async` bemutatása, amely a legmagasabb szintű absztrakció egy feladat aszinkron elindítására.
- Visszatérési értéke egy `std::future`, amin keresztül később elkérhetjük az eredményt.
- Kivételkezelés: Ha a háttérszál kivételt dob, azt a `future.get()` hívásakor kapjuk el.

### 7. Indítási Stratégiák (`07_launch_policies.cpp`)
A `std::async` kétféle indítási módjának összehasonlítása:
- `std::launch::async`: Azonnal új szálat indít (kényszerített párhuzamosság).
- `std::launch::deferred`: Csak akkor fut le a kód (az aktuális szálon), amikor kérjük az eredményt (lusta kiértékelés).

### 8. Adatverseny Veszélyei (`08_data_race_example.cpp`)
Egy "ellenpélda", ami bemutatja, mi történik, ha párhuzamos algoritmusokat használunk megosztott, védelem nélküli változók írására. Demonstrálja a `std::atomic` szükségességét vagy a szinkronizáció hiányának következményeit.
