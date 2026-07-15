#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>

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
    Grille maGrille = {{5, 14, 21, 27, 49}, 9}; 
    
    // 2. Configuration de la simulation
    const int NOMBRE_DE_TIRAGES = 10'000'000; // 10 millions de tirages (environ 64 000 ans de Loto)
    
    // Tableaux de statistiques : stat[0] = 0 bon numéro, stat[5] = 5 bons numéros
    vector<int> statsNumeros(6, 0); 
    int totalJackpots = 0;

    // 3. Initialisation du moteur aléatoire haute performance
    // Note d'ingénierie : Pour une simulation massive, on utilise le Mersenne Twister (mt19937) 
    // initialisé par l'entropie matérielle (random_device). C'est ultra-rapide en C++.
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distNumeros(1, 49);
    uniform_int_distribution<> distChance(1, 10);

   
    cout << "Test de build : CMake fonctionne !" << endl;
    cout << "Test : le build fonctionne et le code s’exécute !" << endl;
    cout << "Grille testée : 5-14-21-27-49 | Chance: 9" << endl;
    cout << "Execution de " << NOMBRE_DE_TIRAGES << " tirages..." << endl;
    cout << "Simulation en cours... voici un tirage exemple : " << endl;

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

        if (i == 0) {
    cout << "Tirage exemple : ";
    for (int n : tirageActuel.numeros) cout << n << " ";
    cout << "| Chance: " << tirageActuel.chance << endl;
}

    
        // Tirage du numéro chance
        tirageActuel.chance = distChance(gen);

        // Analyse instantanée
        analyserTirage(tirageActuel, maGrille, statsNumeros, totalJackpots);
    }

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double> duree = fin - debut;

    // 5. Affichage du rapport d'audit
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
