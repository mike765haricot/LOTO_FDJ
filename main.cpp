#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <numeric>
#include <set>

using namespace std;

// Structure pour stocker une grille
struct Grille {
    vector<int> numeros;
    int chance;
};

// Fonction récursive pour générer toutes les combinaisons (C(n, k))
void genererCombinaisons(int offset, int k, const vector<int>& pool, vector<int>& courante, vector<vector<int>>& resultats) {
    if (k == 0) {
        resultats.push_back(courante);
        return;
    }
    for (size_t i = offset; i <= pool.size() - k; ++i) {
        courante.push_back(pool[i]);
        genererCombinaisons(i + 1, k - 1, pool, courante, resultats);
        courante.pop_back();
    }
}

// Filtre heuristique sur la piscine complète de 10 numéros
bool estPoolIntelligente(const vector<int>& pool) {
    int somme = accumulate(pool.begin(), pool.end(), 0);
    // Pour 10 numéros, la somme moyenne se situe autour de 250
    if (somme < 200 || somme > 300) return false;

    int pairs = 0;
    for (int num : pool) {
        if (num % 2 == 0) pairs++;
    }
    // Équilibre : entre 4 et 6 numéros pairs sur les 10
    if (pairs < 4 || pairs > 6) return false;

    return true;
}

int main() {
    const int NOMBRE_DE_TIRAGES = [] {
        const char* envRuns = std::getenv("CI_TIRAGES");
        if (envRuns) {
            int value = std::atoi(envRuns);
            if (value > 0) return value;
        }
        return 3'000'000; 
    }();

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distNumeros(1, 49);
    uniform_int_distribution<> distChance(1, 10);

    cout << "=========================================" << endl;
    cout << "  MOTEUR COMBINATOIRE & REDUCTEUR v3.0   " << endl;
    cout << "=========================================\n" << endl;

    // ---------------------------------------------------------
    // PHASE 1 : GENERATION DE LA PISCINE (10 NUMEROS)
    // ---------------------------------------------------------
    cout << "[*] Phase 1 : Creation de la matrice de base (10 numeros)..." << endl;
    vector<int> pool;
    long rejets = 0;

    while (true) {
        pool.clear();
        while (pool.size() < 10) {
            int num = distNumeros(gen);
            if (find(pool.begin(), pool.end(), num) == pool.end()) {
                pool.push_back(num);
            }
        }
        sort(pool.begin(), pool.end());

        if (estPoolIntelligente(pool)) break;
        rejets++;
    }

    int chanceUnique = distChance(gen);

    cout << "[+] Matrice isolee (rejets: " << rejets << ") : ";
    for (int n : pool) cout << n << " ";
    cout << "| Chance: " << chanceUnique << "\n" << endl;

    // ---------------------------------------------------------
    // PHASE 2 : ALGORITHME GLOUTON (SYSTEME REDUCTEUR)
    // ---------------------------------------------------------
    cout << "[*] Phase 2 : Calcul combinatoire et reduction..." << endl;
    
    // 1. Générer les 252 grilles possibles de 5 numéros
    vector<vector<int>> toutesLesGrilles;
    vector<int> tempGrille;
    genererCombinaisons(0, 5, pool, tempGrille, toutesLesGrilles);
    
    // 2. Générer les 120 triplets possibles à couvrir
    vector<vector<int>> tousLesTriplets;
    vector<int> tempTriplet;
    genererCombinaisons(0, 3, pool, tempTriplet, tousLesTriplets);
    
    set<vector<int>> tripletsRestants(tousLesTriplets.begin(), tousLesTriplets.end());
    vector<Grille> systemeReducteur;

    // 3. Réduction : Choisir les grilles qui couvrent le plus de triplets inédits
    while (!tripletsRestants.empty()) {
        int maxCouverts = -1;
        int indexChoisi = -1;
        vector<vector<int>> tripletsCouvertsParChoix;

        for (size_t i = 0; i < toutesLesGrilles.size(); i++) {
            vector<vector<int>> tripletsDeCetteGrille;
            vector<int> temp;
            genererCombinaisons(0, 3, toutesLesGrilles[i], temp, tripletsDeCetteGrille);

            int couverts = 0;
            vector<vector<int>> tempCouverts;
            for (const auto& t : tripletsDeCetteGrille) {
                if (tripletsRestants.find(t) != tripletsRestants.end()) {
                    couverts++;
                    tempCouverts.push_back(t);
                }
            }

            if (couverts > maxCouverts) {
                maxCouverts = couverts;
                indexChoisi = i;
                tripletsCouvertsParChoix = tempCouverts;
            }
        }

        // Ajouter la meilleure grille au système
        Grille nouvelleGrille = {toutesLesGrilles[indexChoisi], chanceUnique};
        systemeReducteur.push_back(nouvelleGrille);
        
        // Retirer les triplets désormais couverts
        for (const auto& t : tripletsCouvertsParChoix) {
            tripletsRestants.erase(t);
        }
        
        // Retirer la grille des choix possibles
        toutesLesGrilles.erase(toutesLesGrilles.begin() + indexChoisi);
    }

    cout << "[+] Optimisation terminee ! " << endl;
    cout << "[>] Grilles theoriques : 252" << endl;
    cout << "[>] Grilles a jouer apres reduction : " << systemeReducteur.size() << " (Garantie 3 si 3)" << endl;
    cout << "-----------------------------------------" << endl;
    for (size_t i = 0; i < systemeReducteur.size(); i++) {
        cout << "Grille " << setw(2) << i+1 << " : ";
        for (int n : systemeReducteur[i].numeros) cout << setw(2) << n << " ";
        cout << endl;
    }
    cout << endl;

    // ---------------------------------------------------------
    // PHASE 3 : SIMULATION DE MONTE-CARLO SUR LE SYSTEME
    // ---------------------------------------------------------
    vector<int> statsNumeros(6, 0);
    int totalJackpots = 0;

    cout << "[*] Phase 3 : Assaut du reseau de la FDJ (" << NOMBRE_DE_TIRAGES << " tirages)..." << endl;
    auto debut = chrono::high_resolution_clock::now();

    for (int i = 0; i < NOMBRE_DE_TIRAGES; ++i) {
        vector<int> tirageActuel;
        while (tirageActuel.size() < 5) {
            int num = distNumeros(gen);
            if (find(tirageActuel.begin(), tirageActuel.end(), num) == tirageActuel.end()) {
                tirageActuel.push_back(num);
            }
        }
        int chanceActuelle = distChance(gen);

        int meilleurScoreDuFilet = 0;
        bool jackpotTouche = false;

        // On teste le tirage contre TOUTES les grilles de notre système
        for (const auto& grilleJouee : systemeReducteur) {
            int bons = 0;
            for (int n : grilleJouee.numeros) {
                if (find(tirageActuel.begin(), tirageActuel.end(), n) != tirageActuel.end()) {
                    bons++;
                }
            }
            if (bons > meilleurScoreDuFilet) {
                meilleurScoreDuFilet = bons;
            }
            if (bons == 5 && chanceActuelle == grilleJouee.chance) {
                jackpotTouche = true;
            }
        }

        // On n'enregistre que la meilleure performance du système pour ce tirage
        statsNumeros[meilleurScoreDuFilet]++;
        if (jackpotTouche) totalJackpots++;
    }

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double> duree = fin - debut;

    cout << "\n=========================================" << endl;
    cout << "   RAPPORT D'EFFICACITE DU SYSTEME       " << endl;
    cout << "=========================================" << endl;
    cout << "Temps d'execution : " << duree.count() << " secondes." << endl;
    cout << "-----------------------------------------" << endl;
    cout << "Resultats consolides (meilleure grille du filet par tirage) :" << endl;
    cout << "Aucun gain        : " << statsNumeros[0] + statsNumeros[1] << endl;
    cout << "2 bons numeros    : " << statsNumeros[2] << endl;
    cout << "3 bons numeros    : " << statsNumeros[3] << endl;
    cout << "4 bons numeros    : " << statsNumeros[4] << endl;
    cout << "5 bons numeros    : " << statsNumeros[5] << " (Sans Chance)" << endl;
    cout << "-----------------------------------------" << endl;
    cout << "JACKPOTS CIBLES   : " << totalJackpots << " FOIS" << endl;
    cout << "=========================================\n" << endl;

    return 0;
}
