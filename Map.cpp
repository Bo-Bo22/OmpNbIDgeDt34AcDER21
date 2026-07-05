#include "Map.hpp"
#include <cstdlib>

// Costruttore: crea una finestra per la mappa e la centra sullo schermo
Map::Map(int yMax) {
    // Calcola le coordinate per centrare la mappa
    UIManager::getCenterCoordinates(colonne, righe, startY, startX);
    
    // Crea la finestra della mappa
    playwin = newwin(righe, colonne, startY, startX);
    
    // Applica il background nero alla finestra
    UIManager::setBlackBackground(playwin);
    
    // Disegna il bordo della finestra
    box(playwin, 0, 0);

    // Inizializza tutte le celle della mappa a 0 (spazio vuoto)
    for (int i = 0; i < righe; ++i) {
        for (int j = 0; j < colonne; ++j) {
            map[i][j] = 0;
        }
    }

    NLivello = 0;  // Inizializza il numero del livello
}

// Inizializza la mappa di un livello con muri, porte e area spawn sicura
// Parametri: livelloReale (numero del livello)
void Map::initmap(int livelloReale) {
    this->NLivello = livelloReale;
    
    // Inizializza il seed casuale per questo livello (cambia al variare del livello)
    srand(NLivello + 22);

    int NumeroMuriDis = 0;  // Contatore dei muri distruttibili trovati

    // ========== FASE 1: Crea i muri indistruttibili ==========/
    // Posiziona i muri indistruttibili in un pattern a griglia
    for (int i = 0; i < righe - 1; i++) {
        for (int j = 0; j < colonne; j++) {
            // Condizioni: posiziona in modo alternato, evitando i bordi e l'area spawn
            if (i % 2 == 0 && j % 3 == 0 && i != righe && j != 0 && i != 0 && j != colonne - 1) {
                map[i][j] = 1;  // 1 = muro indistruttibile
                mvwaddch(getWin(), i, j, '#');
            }
        }
    }

    // ========== FASE 2: Raccoglie le celle per i muri distruttibili ==========/
    // Scorre la mappa e raccoglie puntatori a tutte le celle vuote (escluso i bordi)
    for (int i = 1; i < righe - 1; i++) {
        for (int j = 1; j < colonne - 1; j++) {
            if (map[i][j] == 0) {
                MuriDistruttibili[NumeroMuriDis] = &map[i][j];
                NumeroMuriDis++;
            }
        }
    }

    // ========== FASE 3: Piazza i muri distruttibili in modo casuale ==========/
    // Calcola quanti muri distruttibili piazzare (aumenta con il livello)
    int numDaCreare = (NLivello * 3 + 100);

    // Assicura di non superare il numero di celle disponibili
    if (numDaCreare > NumeroMuriDis) {
        numDaCreare = NumeroMuriDis;
    }

    // Piazza i muri distruttibili in posizioni casuali
    for (int k = 0; k < numDaCreare; k++) {
        int i = rand() % NumeroMuriDis;           // Indice casuale nell'array
        int* cella = MuriDistruttibili[i];        // Puntatore alla cella

        if (*cella == 0) {                         // Se la cella è ancora vuota
            *cella = 2;                            // 2 = muro distruttibile
            int r = (cella - &map[0][0]) / colonne;  // Calcola la riga
            int c = (cella - &map[0][0]) % colonne;  // Calcola la colonna
            mvwaddch(getWin(), r, c, '+');
        }
    }

    // ========== FASE 4: Crea l'area spawn sicura ==========/
    // Rimuove i muri dall'area di spawn (3x5) per permettere al giocatore di partire
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            if (map[i][j] == 2) {
                map[i][j] = 0;  // Trasforma i muri distruttibili in spazi vuoti
                mvwaddch(getWin(), i, j, ' ');
            }
        }
    }

    // ========== FASE 5: Piazza le porte ==========/
    // Porta per il livello successivo
    map[3][3] = 3;  // 3 = porta livello successivo
    mvwaddch(getWin(), 3, 3, '>');

    // Porta per il livello precedente (solo se non è il primo livello)
    if (NLivello > 1) {
        map[4][4] = 4;  // 4 = porta livello precedente
        mvwaddch(getWin(), 4, 4, '<');
    }
}

// Aggiorna la visualizzazione della mappa
void Map::refresh() {
    wrefresh(playwin);
}

// Ritorna il valore della cella alla posizione (y, x)
int Map::GetPos(int y, int x) {
    return map[y][x];
}

// Ritorna il numero del livello corrente
int Map::GetLvlN() {
    return NLivello;
}

// Ritorna il carattere da visualizzare per una cella specifica
char Map::GetMapChar(int y, int x) {
    if (y < 0 || y >= righe || x < 0 || x >= colonne) {
        return ' ';
    }
    
    int cellValue = map[y][x];
    switch (cellValue) {
        case 0: return ' ';   // Spazio vuoto
        case 1: return '#';   // Muro indistruttibile
        case 2: return '+';   // Muro distruttibile
        case 3: return '>';   // Porta livello successivo
        case 4: return '<';   // Porta livello precedente
        default: return ' ';
    }
}

// Ridisegna una singola cella della mappa con il suo contenuto
void Map::RedrawCell(int y, int x) {
    if (y < 1 || y >= righe - 1 || x < 1 || x >= colonne - 1) {
        return;
    }
    
    char ch = GetMapChar(y, x);
    mvwaddch(playwin, y, x, ch);
}

// Ritorna il puntatore alla finestra di ncurses della mappa
WINDOW* Map::getWin() {
    return playwin;
}
