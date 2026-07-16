#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cstdlib>

using namespace std;

// Structure pour stocker une grille
struct Grille {
    vector<int> numeros;
    int chance;
};

// Fonction pour comparer le tirage avec la grille du joueur
void analyserTirage(const Grille& tirage, const Grille& joueur, vector<int>& statistiques, int& jackpots) {
    int bonsNumeros = 0;
    
    // Vérifier les 5 numéros
    for (int num : joueur.numeros) {
        if (find(tirage.numeros.begin(), tirage.numeros.end(), num) != tirage.numeros.end()) {
            bonsNumeros++;
        }
    }
    
    bool bonChance = (tirage.chance == joueur.chance);

    // Enregistrement dans les statistiques (index = nombre de bons numéros)
    statistiques[bonsNumeros]++;

    // Vérification du Jackpot absolu (5 numéros + Chance)
    if (bonsNumeros == 5 && bonChance) {
        jackpots++;
    }
}

int main() {
    // 1. Ta grille générée par ton interface web
    Grille maGrille = {{10, 12, 31, 44, 47}, 3}; 
    
    // 2. Configuration de la simulation
    const int NOMBRE_DE_TIRAGES = [] {
        const char* envRuns = std::getenv("CI_TIRAGES");
        if (envRuns) {
            int value = std::atoi(envRuns);
            if (value > 0) return value;
        }
        return 10'000'000; // 10 millions de tirages par défaut
    }();
    
    // Tableaux de statistiques : stat[0] = 0 bon numéro, stat[5] = 5 bons numéros
    vector<int> statsNumeros(6, 0);
    int totalJackpots = 0;

    // 3. Initialisation du moteur aléatoire haute performance
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distNumeros(1, 49);
    uniform_int_distribution<> distChance(1, 10);

    cout << "Démarrage du moteur de simulation (C++)..." << endl;
    
    // Affichage dynamique de la grille testée
    cout << "Grille testée : ";
    for (size_t i = 0; i < maGrille.numeros.size(); ++i) {
        cout << maGrille.numeros[i] << (i < maGrille.numeros.size() - 1 ? "-" : "");
    }
    cout << " | Chance: " << maGrille.chance << endl;
    
    cout << "Execution de " << NOMBRE_DE_TIRAGES << " tirages..." << endl;

    auto debut = chrono::high_resolution_clock::now();

    // 4. Boucle principale de simulation
    for (int i = 0; i < NOMBRE_DE_TIRAGES; ++i) {
        Grille tirageActuel;
        
        // Tirage de 5 numéros uniques
        while (tirageActuel.numeros.size() < 5) {
            int num = distNumeros(gen);
            if (find(tirageActuel.numeros.begin(), tirageActuel.numeros.end(), num) == tirageActuel.numeros.end()) {
                tirageActuel.numeros.push_back(num);
            }
        }

        // Tirage du numéro chance
        tirageActuel.chance = distChance(gen);

        if (i == 0) {
            cout << "Tirage exemple : ";
            for (int n : tirageActuel.numeros) cout << n << " ";
            cout << "| Chance: " << tirageActuel.chance << endl;
        }

        // Analyse instantanée
        analyserTirage(tirageActuel, maGrille, statsNumeros, totalJackpots);
    }

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double> duree = fin - debut;

    cout << "\n=========================================" << endl;
    cout << "          RAPPORT DE SIMULATION          " << endl;
    cout << "=========================================" << endl;
    cout << "Temps d'exécution : " << duree.count() << " secondes." << endl;
    cout << "Vitesse           : " << (int)(NOMBRE_DE_TIRAGES / duree.count()) << " tirages/seconde." << endl;
    cout << "-----------------------------------------" << endl;
    cout << "Résultats sur " << NOMBRE_DE_TIRAGES << " tirages :" << endl;
    cout << "0 bon numéro      : " << statsNumeros[0] << endl;
    cout << "1 bon numéro      : " << statsNumeros[1] << endl;
    cout << "2 bons numéros    : " << statsNumeros[2] << endl;
    cout << "3 bons numéros    : " << statsNumeros[3] << endl;
    cout << "4 bons numéros    : " << statsNumeros[4] << endl;
    cout << "5 bons numéros    : " << statsNumeros[5] << " (Sans le N° Chance)" << endl;
    cout << "-----------------------------------------" << endl;
    cout << "JACKPOTS (5 + 1)  : " << totalJackpots << " FOIS" << endl;
    cout << "=========================================\n" << endl;

    return 0;
}
