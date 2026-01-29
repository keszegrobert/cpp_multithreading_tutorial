# II. Közepes Szintű Koordináció (Mid-Level)

Ezen a szinten már megjelennek a szálkezelés klasszikus objektumai, mint a mutexek és ígéretek. Itt a cél a megosztott erőforrások védelme és a szálak közötti strukturált adatátadás.

## Példák magyarázata

### 1. Mutex Alapok (`01_mutex_example.cpp`)
A "kölcsönös kizárás" (Mutual Exclusion) bemutatása.
- `std::mutex`: Az alapvető lakat.
- `std::lock_guard`: RAII stílusú lezárás, amely garantálja, hogy a blokk elhagyásakor (akár hiba esetén is) a mutex felszabadul.
- Demonstrálja, hogyan előzhető meg a kimenet összekuszálódása (`std::cout` védelme).

### 2. Holtpont (Deadlock) (`02_deadlock_example.cpp`)
A zárolások egyik legnagyobb veszélye: amikor két szál egymásra vár, és a program lefagy.
- Bemutat egy szituációt, ahol deadlock alakul ki.
- Megmutatja a megoldást C++17-ben: `std::scoped_lock`, amely képes egyszerre több mutexet biztonságosan, deadlock-mentesen lezárni.

### 3. Reader-Writer Lock (`03_reader_writer_bench.cpp`)
Teljesítmény-optimalizáció olyan esetekre, amikor az adatot sokan olvassák, de ritkán írják.
- `std::shared_mutex` (C++17): Megosztott mód (olvasás) és exkluzív mód (írás).
- A benchmark összehasonlítja a sima `std::mutex`-szel, demonstrálva a párhuzamos olvasás előnyeit.

### 4. Lusta Inicializálás (`04_call_once_demo.cpp`)
Erőforrások (pl. Singletons, konfigurációk) szálbiztos inicializálása, ami csak az első használatkor történik meg.
- `std::call_once` és `std::once_flag`: Garantálja, hogy a függvény pontosan egyszer fusson le, még ha több szál egyszerre éri is el.
- "Magic Statics": A statikus lokális változók automatikus szálbiztossága C++11 óta.

### 5. Promise és Future (`05_promise_demo.cpp`)
Adatcsatorna létrehozása két szál között mutexek kézi használata nélkül.
- `std::promise`: Az adó oldal, aki "megígéri", hogy lesz adat.
- `std::future`: A fogadó oldal, aki várja az adatot.
- Ez egy egyszeri jelzésátvitelre alkalmas mechanizmus.

### 6. Kivételek Átvitele (`06_exception_propagation.cpp`)
Hogyan kezeljük a hibákat aszinkron környezetben?
- Ha egy szálban kezeletlen kivétel keletkezik, a program általában leáll.
- A `std::promise` segítségével a kivételt "átküldhetjük" a fogadó szálnak (`set_exception`), ahol a `future.get()` újra eldobja azt, így a fő szálban kezelhetjük a hibát.
