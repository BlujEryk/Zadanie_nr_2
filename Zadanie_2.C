#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
using namespace std;


// Klasa do przechowywania sygnałów
class MySignal {
public:
    Int_t signal_number;
    Double_t amplitude;
    Double_t charge;
    Int_t tot;
    Int_t t_0;

    MySignal(){}
    MySignal(Int_t sig, Double_t amp, Double_t ch, Int_t tt, Int_t t0) 
        : signal_number(sig), amplitude(amp), charge(ch), tot(tt), t_0(t0) {}
};



Bool_t Wave(string data_file = "wave_0.dat", const std::string& treeName = "SignalTree", const std::string& rootFileName = "data.root") {
    
    // Tworzenie pliku ROOT i drzewa
    TFile* rootFile = new TFile(rootFileName.c_str(), "RECREATE");
    TTree* tree = new TTree(treeName.c_str(), "Tree with MySignal data");
    
    // Stworzenie w drzewie gałęzi dla obiektów MySignal
    MySignal my_sig;
    tree->Branch("MySignalBranch", &my_sig);
    
    // Otworzenie pliku z danymi
    fstream decector_data;
    decector_data.open(data_file, ios::binary | ios::in);

    // Obsłużenie błędów przy pracy z plikiem  
    if (!decector_data.is_open()) {
        cerr << "Błąd: Nie udało się otworzyć pliku " << data_file << endl;
        return kFALSE;
    }
    decector_data.seekg(0, ios::end);
    if (decector_data.tellg() == 0) {
        cerr << "Błąd: Plik " << data_file << " jest pusty." << endl;
        return kFALSE;
    }
    decector_data.seekg(0, ios::beg);
    
    // Zdefiniowanie długości pojedynczego sygnału
    Int_t signal_length = 1024;
    vector<float> signal_vector(signal_length);

    // Stała konwersji z jednostek ADC na mV
    float conversion_constant = 1/4.096;
    Int_t signal_number = 1;

    // Analiza sygnałów z pliku
    for (Int_t i = 0; i < 100000; i++) {

        // Konwersja sygnału na mV
        for (Int_t j = 0; j < signal_length; j++) {
            float value;
            decector_data.read(reinterpret_cast<char*>(&value), sizeof(float));
            signal_vector[j] = value * conversion_constant;
        }

        // Obliczanie linii bazowej
        Float_t first_50_sum = accumulate(signal_vector.begin(), signal_vector.begin() + 50, 0.0);
        Float_t first_50_average = first_50_sum / 50;
        
        // Odejmowanie linii bazowej i wartość bezwględna sygnału
        for (Int_t j = 0; j < signal_length; j++) {
            signal_vector[j] = abs(signal_vector[j] - first_50_average);
        }

        // Obliczanie amplitudy jako największej na moduł wartości sygnału
        Double_t amp = *max_element(signal_vector.begin(), signal_vector.end());
        
        // Wyznaczanie t0 jako indeksu, dla którego sygnał przekracza wartość 10% amplitudy
        Int_t t0 = -1, tmax = -1;
        for (Int_t j = 0; j < signal_length; j++) {
            if (signal_vector[j] > 0.1 * amp) {
                t0 = j;
                break;
            }
        }

        // Wyznaczanie tmax = t0 + tot (koniec zakresu całkowania) jako indeksu dla którego sygnał znów spada poniżej 10% amplitudy 
        for (Int_t j = t0 + 1; j < signal_length; j++) {
            if (signal_vector[j] < 0.1 * amp) {
                tmax = j;
                break;
            }
        }
        
        // Sprawdzenie czy znaleziono t0 i tmax w zakresie sygnału
        if (t0 == -1 || tmax == -1 || t0 >= signal_length || tmax > signal_length) {
            continue;
        }
        
        // Obliczenie tot
        Int_t tot = tmax - t0;

        // Obliczenie ładunku jako całki po sygnale, w zakresie [t0, tmax] (dt = 1, więc nie trzeba domnażać)
        Double_t ch = accumulate(signal_vector.begin() + t0, signal_vector.begin() + tmax, 0.0);

        // Dodanie sygnału do drzewa
        my_sig = MySignal(signal_number, amp, ch, tot, t0);
        tree->Fill();
        
        // Przejście do następnego sygnału
        signal_number++;
    }
    
    // Zakończenie działąnia na plikach po analizie wszystkich wybranych sygnałów
    decector_data.close();
    rootFile->Write();
    rootFile->Close();
    return kTRUE;
}


// Funkcja do tworzenia histogramów amplitud, ładunków, t0 i tot
Bool_t DrawHistograms(const string& rootFileName = "data.root"){
    
    // Tryb, w którym ROOT nie wyświetla okienek z płótnami
    gROOT->SetBatch(kTRUE);

    // Odczytanie danych z zapisanego pliku
    TFile* file = new TFile(rootFileName.c_str(), "READ");
    TTree* tree = (TTree*)file->Get("SignalTree");
    
    // Tworzenie Histogramów
    TH1F* hAmplitude = new TH1F("hAmplitude", "Histogram Amplitud;Amplituda [mV];Zliczenia", 100, 0, 1000);
    TH1F* hCharge = new TH1F("hCharge", "Histogram Ladunkow;Ladunek [j.u.];Zliczenia", 100, 0, 100000);
    TH1F* hT0 = new TH1F("hT0", "Histogram T0;T0 [ns];Zliczenia", 100, 0, 1024);
    TH1F* hTOT = new TH1F("hTOT", "Histogram TOT;TOT [ns];Zliczenia", 100, 0, 1024);
    
    // Wpisanie w histogramy danych z zapisanego w pliku drzewa
    tree->Draw("amplitude>>hAmplitude");
    tree->Draw("charge>>hCharge");
    tree->Draw("t_0>>hT0");
    tree->Draw("tot>>hTOT");
    
    // Tworzenie płótna z czterema histogramami
    TCanvas* c1 = new TCanvas("c1", "Histograms", 1200, 800);
    c1->Divide(2,2);
    c1->cd(1); hAmplitude->Draw();
    c1->cd(2); hCharge->Draw();
    c1->cd(3); hT0->Draw();
    c1->cd(4); hTOT->Draw();

    // Zapisanie płótna do pliku .png
    c1->SaveAs("histograms.png");

    // Zamknięcie pliku
    file->Close();
    return kTRUE;
}


// Wykonanie programu
Bool_t Zadanie_2() {
    Wave();
    DrawHistograms();
    return kTRUE;
}