# III. Alacsony Szintű Szinkronizáció (Low-Level)

Itt lépünk ki a kényelmi zónából: közvetlenül irányítjuk a szálak futását, várakozását és ébresztését. Ezek az eszközök adják az alapját a magasabb szintű absztrakcióknak is.

## Példák magyarázata

### 1. Spinlock vs Sleep (`01_spinlock_vs_sleep.cpp`)
Két alapvető várakozási stratégia összehasonlítása:
- **Sleep**: A szál visszaadja a CPU-t az operációs rendszernek. Jó energiahatékonyság, de lassú felébredés.
- **Spinlock (Aktív várakozás)**: A szál egy ciklusban pörög (`while(!ready)`). Azonnali reakció, de 100% CPU használat és energiapazarlás.

### 2. Sleep Pontosság (`02_sleep_precision.cpp`)
Demonstráció arról, hogy a `std::this_thread::sleep_for` nem pontos.
- Az operációs rendszer ütemezője (scheduler) miatt a kért 1ms alvásból könnyen lehet 2-3ms vagy több. Valós idejű rendszereknél ezt figyelembe kell venni.

### 3. Condition Variable Alapok (`03_cv_basics.cpp`)
A `std::condition_variable` a leghatékonyabb módja annak, hogy egy szál várjon egy eseményre anélkül, hogy pazarolná a CPU-t.
- Használata mindig egy `std::unique_lock`-kal együtt történik.
- `wait()`: Mutex elengedése és alvás.
- `notify_one()` / `notify_all()`: Alvó szál(ak) ébresztése.

### 4. Szemafor Implementáció (`04_semaphore_impl.cpp`)
Bár a C++20 bevezette a `std::counting_semaphore`-t, ez a példa megmutatja, hogyan építhetünk ilyet C++17-ben `mutex` és `condition_variable` segítségével.
- A szemafor egy számláló, ami engedélyezi N szál egyidejű belépését egy kritikus szakaszba.

### 5. Termelő-Fogyasztó (`05_producer_consumer.cpp`)
A párhuzamos programozás egyik legklasszikusabb mintája.
- **Producer**: Adatokat állít elő és tesz egy sorba (Queue).
- **Consumer**: Adatokat vesz ki a sorból és dolgozza fel őket.
- A példa bemutatja, hogyan kell szinkronizálni a hozzáférést a sorhoz, és hogyan jelezze a termelő, ha van új adat, vagy ha véget ért a termelés.

### 6. Barrier (Gát) (`06_barrier_example.cpp`)
Szinkronizációs pont, ahol a szálak megvárják egymást.
- Egyik szál sem haladhat tovább, amíg az összes meg nem érkezett a gáthoz.
- Hasznos fázisokra osztott algoritmusoknál (pl. mindenki kiszámol egy részfeladatot -> bevárják egymást -> kicserélik az adatokat).
