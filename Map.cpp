#include "Map.hpp"
#include "UIManager.hpp"
#include "Enemy.hpp"
#include <cstdlib> //utile per rand e srand

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

    portaY = 3; // Coordinate di sicurezza (se non ci fossero muri)
    portaX = 3;
}

// Inizializza la mappa di un livello con muri, porte e area spawn sicura
// Parametri: livelloReale (numero del livello)
// Inizializza la mappa di un livello con muri, porte e cornice
void Map::initmap(int livelloReale) {
    this->NLivello = livelloReale;
    srand(NLivello + 22);
    int NumeroMuriDis = 0;

    // 1. Azzera l'intera mappa per pulizia
    for (int i = 0; i < righe; i++) {
        for (int j = 0; j < colonne; j++) {
            map[i][j] = 0;
            oggettiNascosti[i][j] = 0; //matrice per gli oggetti nascosti
        }
    }

    // 2. CREA LA CORNICE FISICA (Muri indistruttibili sui bordi esterni)
    for (int i = 0; i < righe; i++) {
        for (int j = 0; j < colonne; j++) {
            // Se siamo sulla prima/ultima riga o sulla prima/ultima colonna
            if (i == 0 || i == righe - 1 || j == 0 || j == colonne - 1) {
                map[i][j] = 1; // 1 = Muro indistruttibile ('█')
            }
        }
    }

    // 3. Posiziona i muri indistruttibili in un pattern a griglia (interni)
    for (int i = 2; i < righe - 1; i += 2) {
        for (int j = 2; j < colonne - 1; j += 3) {
            map[i][j] = 1;  
        }
    }

    // 4. Raccoglie le celle interne per i muri distruttibili (evitando la cornice)
    for (int i = 1; i < righe - 1; i++) {
        for (int j = 1; j < colonne - 1; j++) {
            if (map[i][j] == 0) {
                MuriDistruttibili[NumeroMuriDis] = &map[i][j];
                NumeroMuriDis++;
            }
        }
    }

    // 5. Piazza i muri distruttibili in modo casuale
    int numDaCreare = (NLivello * 3 + 75);
    if (numDaCreare > NumeroMuriDis) {
        numDaCreare = NumeroMuriDis;
    }
    
    for (int k = 0; k < numDaCreare; k++) {
        int i = rand() % NumeroMuriDis;
        if (*(MuriDistruttibili[i]) == 0) {
            *(MuriDistruttibili[i]) = 2; // 2 = muro distruttibile ('+')
        }
    }

    // 6. Crea l'area spawn sicura (puliamo i muri vicino all'angolo 1,1)
    for (int i = 1; i <= 4; i++) {
        for (int j = 1; j <= 5; j++) {
            if (map[i][j] == 2) {
                map[i][j] = 0;  
            }
        }
    }
    map[2][2] = 0; //rimuove il primo muro indistruttibile, sarà la zona per la porta che ci fa tornare indietro con il libello


    // 7. NASCONDI LA PORTA SOTTO UN MURO DISTRUTTIBILE
        // Contiamo quanti muri distruttibili ci sono attualmente sulla mappa
    int totaleMuri = 0;
    for (int i = 1; i < righe - 1; i++) {
        for (int j = 1; j < colonne - 1; j++) {
            if (map[i][j] == 2) {
                totaleMuri++;
            }
        }
    }

    // Se abbiamo almeno un muro, scegliamo quale conterrà la porta
    if (totaleMuri > 0) {
        int indiceMuroScelto = rand() % totaleMuri;
        int muriVisti = 0;
        bool portaPiazzata = false;

        // Scorriamo la mappa e ci fermiamo esattamente sul muro scelto
        for (int i = 1; i < righe - 1 && !portaPiazzata; i++) {
            for (int j = 1; j < colonne - 1 && !portaPiazzata; j++) {
                
                if (map[i][j] == 2) {
                    if (muriVisti == indiceMuroScelto) {
                        portaY = i;
                        portaX = j;
                        portaPiazzata = true; // Fa terminare subito i cicli for
                    }
                    muriVisti++;
                }
                
            }
        }
    }

    // Porta livello precedente (<)
    if (NLivello > 1) {
        map[2][2] = 4;  
    }  
    


    // 8. NASCONDI I POWERUPS
    int numPowerups = 3; 
    
    for(int p = 0; p < numPowerups; p++) {
        bool piazzato = false;
        
        while (!piazzato) {
            int ry = 1 + rand() % (righe - 2);
            int rx = 1 + rand() % (colonne - 2);
            
            // controllo per evitare le coordinate della porta
            if (map[ry][rx] == 2 && oggettiNascosti[ry][rx] == 0 && (ry != this->portaY || rx != this->portaX)) {
                
                oggettiNascosti[ry][rx] = 5; // Nascondiamo l'oggetto
                piazzato = true;
            }
        }
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
    if (y < 0 || y >= righe || x < 0 || x >= colonne) return;
    renderPos(y, x); // Disegnerà lo spazio colorato se è muro, o il carattere se è altro
}

// Ritorna il puntatore alla finestra di ncurses della mappa
WINDOW* Map::getWin() {
    return playwin;
}

// Imposta il valore della cella alla posizione (y, x)
void Map::renderPos(int y, int x){
    int valore = map[y][x];
    
    if (valore == 1) {
        // Disegna il blocco grafico pieno nativo di ncurses
        wattron(getWin(), COLOR_PAIR(10));
        mvwaddch(getWin(), y, x, ' '); // Stampiamo uno spazio vuoto, che apparirà come un blocco pieno
        wattroff(getWin(), COLOR_PAIR(10)); 
    }

    else if (valore == 2) {
        // Disegna il blocco grafico pieno nativo di ncurses
        wattron(getWin(), COLOR_PAIR(11) );
        mvwaddch(getWin(), y, x, ' '); // Stampiamo uno spazio vuoto, che apparirà come un blocco pieno
        wattroff(getWin(), COLOR_PAIR(11) );
     
    } 

    // Disegna la porta del livello successivo (>)
    else if (valore == 3) {
        wattron(getWin(), COLOR_PAIR(3)); 
        mvwaddch(getWin(), y, x, '>');    // La disegniamo colorata
        wattroff(getWin(), COLOR_PAIR(3));
    }
    
    // Disegna la porta del livello precedente (<)
    else if (valore == 4) {
        wattron(getWin(), COLOR_PAIR(3)); 
        mvwaddch(getWin(), y, x, '<');    // La disegniamo colorata
        wattroff(getWin(), COLOR_PAIR(3));
    }

    else if (valore == 5) {
        // Disegna il power-up nascosto (dollaro) con colore speciale
        wattron(getWin(), COLOR_PAIR(5));   // Attiva solo il colore base
        mvwaddch(getWin(), y, x, '$');      // Stampa il dollaro
        wattroff(getWin(), COLOR_PAIR(5));  // Spegne il colore
    }
    
    else {
        // Per tutti gli altri elementi usa la legenda standard
        mvwaddch(getWin(), y, x, legenda[valore]);
    }
}

// Funzione che permette di modificare i valori della matrice di livello.
void Map::setPos(int y, int x, int val) {
    // Controllo di sicurezza per evitare di scrivere fuori dalla memoria della mappa
    if (y >= 0 && y < righe && x >= 0 && x < colonne) {
        map[y][x] = val;
    }
}

// Ridisegna l'intera mappa basandosi SOLO sulla matrice logica
// Cancellando di fatto tutti gli artefatti visivi (scie di nemici e vecchie bombe)
// Ridisegna l'intera mappa basandosi SOLO sulla matrice logica
void Map::renderLevel() {
    for (int y = 0; y < righe; y++) {
        for (int x = 0; x < colonne; x++) {
            renderPos(y, x); 
        }
    }
    wrefresh(playwin);
}