# Pokročilé programování v jazyce C pro Embedded

Tento repozitář obsahuje kompletní výukové materiály a zdrojové kódy pro třídenní intenzivní kurz zaměřený na pokročilé programování v jazyce C pro embedded systémy. Kombinuje teorii moderního C s architektonickými vzory a reálnou praxí na mikrokontrolérech a v RTOS.

## Obsah repozitáře

* **Prezentace (LaTeX Beamer):** 200+ slidů pokrývajících teorii, architekturu a osvědčené postupy.
* **Zdrojové kódy (C23):** Více než 70 praktických ukázek propojených s prezentací.
* **Samostatné úkoly:** 24 praktických cvičení (včetně šablon a vzorových řešení) pro ověření znalostí v každé probrané sekci.
* **Finální projekt (Capstone):** Komplexní dvouhodinový úkol simulující vývoj reálného průmyslového telemetrického uzlu.

## Hlavní témata kurzu

1. **Moderní C (C23) a paměť:** Vlastnosti standardu C23, pointer provenance, strict aliasing, memory pooly s časovou složitostí O(1).
2. **Embedded specifika:** Memory-mapped I/O (`volatile`), obsluha přerušení (ISR), bezzámkové datové struktury (SPSC Ring Buffer).
3. **RTOS (Real-Time Operating Systems):** Synchronizace úloh, prevence deadlocků, priority inheritance a event groups.
4. **Architektura hardware:** DMA přenosy, cache koherence (L1 D-Cache Clean/Invalidate), memory protection unit (MPU), false sharing a zarovnání paměti (`alignas`).
5. **Softwarový design a abstrakce:** Opaque pointers, Hardware Abstraction Layer (HAL), Dependency Injection v C.
6. **Objektově orientované principy v C:** Intruzivní seznamy (`container_of`), Tagged Unions pro události, virtuální tabulky metod (VMT) a simulace dědičnosti.
7. **Testování a kvalita:** Unity testovací framework, mockování hardwaru přes GNU linker (`--wrap`), regresní testování (TDD), semihosting a diagnostika HardFault.
8. **Korektnost kódu:** Undefined Behavior (UB), statická analýza, použití sanitizerů (ASan, UBSan) a průmyslový standard MISRA C.

## Požadavky na sestavení

### C kódy
* **Kompilátor:** GCC, Clang nebo MSVC s podporou standardu **C23**.
* **Sestavovací systém:** CMake verze 3.24 a vyšší.
* Závislosti třetích stran (SDL3, libpng, libcurl, SQLite) jsou automaticky stahovány a sestavovány pomocí modulu `FetchContent` definovaného v `CMakeLists.txt`.

## Sestavení a spuštění ukázek

Projekt je konfigurován pro snadné otevření v moderních vývojových prostředích (CLion, VS Code). Pro sestavení z příkazové řádky:

```bash
mkdir build
cd build
cmake ..
cmake --build .
