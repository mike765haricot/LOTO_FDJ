#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <numeric>

using namespace std;

// Structure pour stocker une grille
struct Grille {
    vector<int> numeros;
    int chance;
};

// ==========================================
// MODULE 1 : LE CERVEAU HEURISTIQUE (IA)
// ==========================================
bool estGrilleIntelligente(const vector<int>& grille) {
    // 1. Filtre de somme (Eviter les dates de naissance 1-31)
    int somme = accumulate(grille.begin(), grille.end(), 0);
    if (somme < 140 || somme > 210) return false;

    // 2. Filtre de parite (Equilibre statistique)
    int pairs = 0;
    for (int num : grille) {
        if (num % 2 == 0) pairs++;
    }
    if (pairs < 2 || pairs > 3) return false;

    // 3. Filtre de suites (Anti-comportement humain, ex: 12-13-14)
    vector<int> triee = grille;
    sort(triee.begin(), triee.end());
    int suitesConsecutives = 0;
    for (size_t i = 0; i < triee.size() - 1; i++) {
        if (triee[i + 1] == triee[i] + 1) suitesConsecutives++;
    }
    if (suitesConsecutives >= 2) return false;

    return true; // La grille est validee par la theorie des jeux
}

// ==========================================
// MODULE 2 : LE MOTEUR DE SIMULATION
// ==========================================
void analyserTirage(const Grille& tirage, const Grille& joueur, vector<int>& statistiques, int& jackpots) {
    int bonsNumeros = 0;
    for (int num : joueur.numeros) {
        if (find(tirage.numeros.begin(), tirage.numeros.end(), num) != tirage.numeros.end()) {
            bonsNumeros++;
        }
    }
    bool bonChance = (tirage.chance == joueur.chance);
    statistiques[bonsNumeros]++;
    
    if (bonsNumeros == 5 && bonChance) {
        jackpots++;
    }
}

int main() {
    // Configuration GitHub Actions / Locale
    const int NOMBRE_DE_TIRAGES = [] {
        const char* envRuns = std::getenv("CI_TIRAGES");
        if (envRuns) {
            int value = std::atoi(envRuns);
            if (value > 0) return value;
        }
        return 10'000'000; // 10 millions par defaut en local
    }();

    // Initialisation du generateur cryptographique haute performance
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distNumeros(1, 49);
    uniform_int_distribution<> distChance(1, 10);

    cout << "=========================================" << endl;
    cout << "   GENERATEUR HYBRIDE & SIMULATEUR v2.0  " << endl;
    cout << "=========================================\n" << endl;

    // ---------------------------------------------------------
    // PHASE 1 : GENERATION DE LA GRILLE OPTIMISEE
    // ---------------------------------------------------------
    cout << "[*] Phase 1 : Initialisation du moteur heuristique..." << endl;
    Grille maGrille;
    long rejets = 0;

    while (true) {
        maGrille.numeros.clear();
        
        while (maGrille.numeros.size() < 5) {
            int num = distNumeros(gen);
            if (find(maGrille.numeros.begin(), maGrille.numeros.end(), num) == maGrille.numeros.end()) {
                maGrille.numeros.push_back(num);
            }
        }

        if (estGrilleIntelligente(maGrille.numeros)) {
            break; // On a trouve une grille qui bat les comportements humains !
        }
        rejets++;
    }
    
    sort(maGrille.numeros.begin(), maGrille.numeros.end());
    maGrille.chance = distChance(gen);

    cout << "[+] Succes ! Grille isolee apres " << rejets << " rejets heuristiques." << endl;
    cout << "[>] GRILLE JOUEE : ";
    for (size_t i = 0; i < maGrille.numeros.size(); ++i) {
        cout << maGrille.numeros[i] << (i < maGrille.numeros.size() - 1 ? "-" : "");
    }
    cout << " | Chance: " << maGrille.chance << "\n" << endl;


    // ---------------------------------------------------------
    // PHASE 2 : SIMULATION DE MONTE-CARLO
    // ---------------------------------------------------------
    vector<int> statsNumeros(6, 0);
    int totalJackpots = 0;

    cout << "[*] Phase 2 : Lancement du simulateur (" << NOMBRE_DE_TIRAGES << " tirages)..." << endl;
    
    auto debut = chrono::high_resolution_clock::now();

    for (int i = 0; i < NOMBRE_DE_TIRAGES; ++i) {
        Grille tirageActuel;
        
        while (tirageActuel.numeros.size() < 5) {
            int num = distNumeros(gen);
            if (find(tirageActuel.numeros.begin(), tirageActuel.numeros.end(), num) == tirageActuel.numeros.end()) {
                tirageActuel.numeros.push_back(num);
            }
        }
        tirageActuel.chance = distChance(gen);

        analyserTirage(tirageActuel, maGrille, statsNumeros, totalJackpots);
    }

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double> duree = fin - debut;

    // ---------------------------------------------------------
    // PHASE 3 : AFFICHAGE DES RESULTATS
    // ---------------------------------------------------------
    cout << "\n=========================================" << endl;
    cout << "          RAPPORT DE SIMULATION          " << endl;
    cout << "=========================================" << endl;
    cout << "Temps d'execution : " << duree.count() << " secondes." << endl;
    cout << "Vitesse           : " << (int)(NOMBRE_DE_TIRAGES / duree.count()) << " tirages/sec." << endl;
    cout << "-----------------------------------------" << endl;
    cout << "0 bon numero      : " << statsNumeros[0] << endl;
    cout << "1 bon numero      : " << statsNumeros[1] << endl;
    cout << "2 bons numeros    : " << statsNumeros[2] << endl;
    cout << "3 bons numeros    : " << statsNumeros[3] << endl;
    cout << "4 bons numeros    : " << statsNumeros[4] << endl;
    cout << "5 bons numeros    : " << statsNumeros[5] << " (Sans Chance)" << endl;
    cout << "-----------------------------------------" << endl;
    cout << "JACKPOTS (5 + 1)  : " << totalJackpots << " FOIS" << endl;
    cout << "=========================================\n" << endl;

    return 0;
}
