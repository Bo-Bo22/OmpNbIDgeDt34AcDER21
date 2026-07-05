#include "LevelManager.hpp"
#include "Map.hpp"

// Costruttore: inizializza i puntatori della lista a NULL
LevelManager::LevelManager() {
    H = NULL;  // Head - puntatore al primo livello
    T = NULL;  // Tail - puntatore all'ultimo livello (livello corrente)
}

// Aggiunge un nuovo livello alla lista e ritorna un riferimento alla mappa
// Parametri: livello (numero livello), YMax (altezza massima dello schermo)
Map& LevelManager::AddLevel(int livello, int YMax) {
    ListaLivelli node = new LivelloNode;
    node->numeroLivello = livello;
    node->map = new Map(YMax);
    node->map->initmap(livello);

    node->next = NULL;
    node->prev = NULL;

    // Se è il primo livello, imposta sia head che tail
    if (T == NULL && H == NULL) {
        T = node;
        H = node;
    }
    // Altrimenti, aggiunge il nuovo livello in coda
    else {
        node->prev = T;
        T->next = node;
        T = node;
        T->next = NULL;
    }

    // Ritorna un riferimento alla mappa del nuovo livello
    return *(node->map);
}

// Passa al livello successivo o crea uno nuovo se non esiste
// Ritorna un riferimento alla mappa del nuovo livello
Map& LevelManager::nextLevel(int yMax) {
    // Se non c'è nessun livello, crea il primo
    if (T == NULL) {
        return AddLevel(1, yMax);
    }

    // Se esiste un livello successivo, passa a esso
    if (T->next != NULL) {
        T = T->next;
    }
    // Altrimenti, crea un nuovo livello
    else {
        AddLevel(T->numeroLivello + 1, yMax);
    }

    return *(T->map);
}

// Torna al livello precedente (non crea nuovi livelli)
// Ritorna un riferimento alla mappa del livello precedente
Map& LevelManager::prevLevel() {
    // Se esiste un livello precedente, passa a esso
    if (T->prev != NULL) {
        T = T->prev;
    }

    return *(T->map);
}

