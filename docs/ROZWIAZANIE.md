# Rozwiązanie problemu crash testfs

## Podsumowanie

Problem został zidentyfikowany i naprawiony. Crash w testfs był spowodowany błędem w bibliotece DMOD, która nie ustawiała prawidłowych uprawnień pamięci dla modułów większych niż 4KB.

## Przyczyna główna

Bug znajdował się w funkcji `Dmod_AlignedMalloc` w pliku `dmod/src/system/if/dmod_if_mem.c`. Funkcja `mprotect` była wywoływana z parametrem `pagesize` (4096 bajtów) zamiast rzeczywistego rozmiaru alokowanej pamięci.

```c
// Kod przed poprawką - NIEPRAWIDŁOWY
mprotect(mem, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC)
```

To powodowało, że tylko pierwsza strona pamięci (4KB) miała uprawnienia do zapisu, odczytu i wykonania (RWX). Kiedy moduł był większy niż 4KB (jak testfs z 5904 bajtami), następne strony nie miały uprawnień do zapisu.

Podczas ładowania modułu, system DMOD próbował zapisać wskaźniki funkcji do sekcji `.outputs`, która dla testfs znajdowała się poza pierwszymi 4KB. To powodowało segmentation fault z błędem `SEGV_ACCERR` (brak uprawnień do zapisu).

## Dlaczego ramfs działał, a testfs nie?

- **ramfs**: 5464 bajty - większy niż 4KB, ale sekcja `.outputs` przypadkowo znajdowała się w pierwszych 4KB
- **testfs**: 5904 bajty - sekcja `.outputs` znajdowała się poza pierwszymi 4KB

Dodatkowo, testfs używał funkcji `dmod_preinit` z `Dmod_IsFunctionConnected`, co dodawało dodatkowy output API, zwiększając rozmiar modułu i przesuwając sekcje pamięci.

## Rozwiązanie

Poprawka oblicza prawidłowy rozmiar pamięci do ochrony, zaokrąglając rozmiar alokacji do granicy strony:

```c
// Kod po poprawce - PRAWIDŁOWY
size_t prot_size = ((Size + pagesize - 1) / pagesize) * pagesize;
mprotect(mem, prot_size, PROT_READ | PROT_WRITE | PROT_EXEC)
```

## Implementacja w dmvfs

1. **Patch file**: `patches/dmod-mprotect-fix.patch` - zawiera poprawkę do DMOD
2. **Automatyczne stosowanie**: `CMakeLists.txt` automatycznie aplikuje patch po pobraniu DMOD przez FetchContent
3. **Dokumentacja**: `docs/dmod-mprotect-fix.md` - szczegółowy opis problemu po angielsku

## Weryfikacja

Po zastosowaniu poprawki:

```bash
# Testfs działa poprawnie
./tests/dmvfs_example ../tests/testfs/build/dmf/testfs.dmf
# Output:
# API is connected!
# testfs initialized
# Module 'testfs' loaded and enabled successfully.
# ...

# Ramfs nadal działa bez regresji
./tests/dmvfs_example ../tests/testfs/build/dmf/ramfs.dmf
# Output:
# RamFS module initialized
# ...
```

## Dalsze kroki

Ta poprawka powinna zostać zgłoszona do repozytorium upstream DMOD:
- https://github.com/choco-technologies/dmod

Dopóki poprawka nie zostanie zaakceptowana w DMOD, dmvfs będzie automatycznie aplikować lokalny patch podczas budowania.

## Budowanie projektu

```bash
# Zbuduj dmvfs (patch jest aplikowany automatycznie)
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .

# Zbuduj testfs
cd ../tests/testfs/
mkdir build
cd build
cmake .. -DDMOD_DIR=$(pwd)/../../../build/_deps/dmod-src/
cmake --build .

# Uruchom test
cd ../../../build
./tests/dmvfs_example ../tests/testfs/build/dmf/testfs.dmf
```

Wszystko powinno działać bez crashy.
