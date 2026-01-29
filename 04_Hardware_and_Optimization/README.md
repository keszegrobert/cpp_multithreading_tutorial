# IV. Hardver és Optimalizáció (Hardware & Optimization)

Ez a fejezet a "vas" működésével foglalkozik. Megértjük, mi történik a CPU regisztereiben és cache memóriájában, és hogyan írhatunk maximális sebességű, lock-free kódot.

## Példák magyarázata

### 1. Overhead Teszt (`01_overhead_test.cpp`)
Minden párhuzamosításnak ára van.
- Egy szál elindítása (`std::thread` konstruktor) időbe telik (OS kernel hívás, verem foglalás).
- Ez a teszt megméri ezt az időt, rávilágítva, hogy mikor *nem* érdemes kis feladatokat külön szálba szervezni.

### 2. Atomic vs Int (`02_atomic_vs_int.cpp`)
Összehasonlítja a sima `int` és a `std::atomic<int>` sebességét.
- Az atomi műveletek sokkal lassabbak lehetnek, mint a sima memóriaírás, mivel a CPU-nak biztosítania kell a koherenciát a magok között.
- Demonstrálja azt is, hogy a "sima" int növelése több szálon hibás eredményt ad (adatverseny), míg az atomi helyeset.

### 3. A Volatile Tévhit (`03_volatile_mistake.cpp`)
Nagy a kísértés, hogy a `volatile` kulcsszót használjuk szinkronizációra, de C++-ban ez **helytelen**.
- A példa bemutatja, hogy a `volatile` nem garantálja az atomicitást, és ugyanúgy adatversenyhez vezethet. C++-ban a `std::atomic` a helyes eszköz.

### 4. Atomic Flag Spinlock (`04_atomic_flag_spinlock.cpp`)
A legegyszerűbb szinkronizációs primitív megvalósítása hardver szinten.
- `std::atomic_flag`: Az egyetlen típus, ami garantáltan lock-free minden architektúrán.
- Ezzel építünk egy saját Spinlock-ot, megértve a `test_and_set` utasítás jelentőségét.

### 5. Lock-Free Szorzás CAS Loop-pal (`05_cas_loop_multiply.cpp`)
Hogyan módosítsunk atomi változót komplexebb módon, mint egy egyszerű összeadás?
- A Compare-And-Swap (CAS) hurok bemutatása: `compare_exchange_weak`.
- Ez a lock-free algoritmusok alapköve: "Olvasd ki az értéket, számold ki az újat, és próbáld visszaírni. Ha közben más módosította, kezdd elölről."

### 6. Atomic Contention (`06_atomic_contention.cpp`)
Amikor a párhuzamosítás lassít.
- Ha sok szál próbálja ugyanazt az atomi változót írni, a CPU cache koherencia protokollja (MESI) miatt a magok folyamatosan "huzakodnak" a cache line birtoklásáért.
- Ez drasztikus teljesítményromlást okozhat, amit csak az adatok szétválasztásával lehet megoldani.

### 7. False Sharing (`07_false_sharing_demo.cpp`)
A cache-huzakodás rejtett formája.
- Két *független* változó véletlenül ugyanabba a cache line-ba (64 bájt) kerül.
- Bár logikailag nem osztunk meg adatot, a hardver szintjén mégis ütközés van.
- A példa bemutatja a jelenséget és a megoldását (`alignas` padding).
