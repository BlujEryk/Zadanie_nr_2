# Analiza sygnałów z pliku binarnego

## Opis programu
Program służy do analizy sygnałów odczytywanych z pliku binarnego i zapisuje obliczone parametry sygnałów do drzewa ROOT. Dodatkowo generuje histogramy przedstawiające podstawowe parametry sygnałów.

## Funkcjonalność
- **Odczyt sygnałów** z pliku binarnego.
- **Obliczanie parametrów sygnałów**:
  - Amplituda (maksymalna wartość sygnału po odjęciu linii bazowej).
  - Ładunek (całka z wartości sygnału w przedziale t0 - tmax).
  - T0 (czas, w którym sygnał przekracza 10% swojej amplitudy).
  - TOT (czas powyżej 10% amplitudy sygnału).
- **Zapis danych** do pliku ROOT (`data.root`).
- **Generowanie histogramów** przedstawiających analizowane parametry.

## Wymagania
- **ROOT Framework** (do analizy danych)
- Plik binarny zawierający dane sygnałowe (np. `wave_0.dat`).

## Instalacja i uruchomienie

### **1. Kompilacja i uruchomienie**
```bash
root -l -q "Zadanie_2.C"
```

### **2. Wyniki działania**
Po uruchomieniu programu generowane są następujące pliki:
- `data.root` - plik zawierający przetworzone dane.
- `histograms.png` - obraz zawierający histogramy parametrów sygnału.

## Struktura projektu
```
📁 Repozytorium/
├── 📄 Zadanie_2.C   # Kod programu napisany w C z ROOT
├── 📄 README.md      # Dokumentacja projektu
└── 📄 wave_0.dat    # Przykładowy plik binarny z danymi
```
