#include "Enemy.hpp"
#include <cstdlib>
#include <cmath>
using namespace std;

// Costruttore base 
Enemy::Enemy(int x, int y, char c, WINDOW *win, Direction dir, int cp) {
    XLoc = x;
    YLoc = y;
    character = c; // Modificato per supportare il carattere passato dinamicamente
    curwin = win;
    alive = true;
    Dir = dir;
    MoveInterval = 150; 
    colorPair = COLOR_PAIR(cp);
    
    // Inizializza il tempo di partenza, altrimenti i nemici si freezano!
    lastMove = std::chrono::steady_clock::now();
}

//Genera un nemico in un punto casuale della mappa
void Enemy::spawna_casuale(Map &Mappa, Enemy** altri, int n) {
    bool occupato = false;
    int h = Map::getHeight(); // Recupera l'altezza massima della mappa
    int w = Map::getWidth();  // Recupera la larghezza massima della mappa
    
    // Estrazione casuale di una coordinata libera da ostacoli
    do {
        occupato = false;
        
        // rand() % (max - min + 1) + min calcola un punto casuale in un quadrato 5x5 al centro della mappa
        YLoc = rand() % 7 + (h / 2) - 5 ; // Centra verticalmente il range di spawn
        XLoc = rand() % 7 + (w / 2) - 5 ; // Centra orizzontalmente il range di spawn
        
        // Ciclo for per controllare che la nuova posizione non si sovrapponga a un altro nemico esistente
        for(int i = 0; i < n; i++) {
            if(altri[i]->getX() == XLoc && altri[i]->getY() == YLoc) {
                occupato = true; // Cella già occupata da un altro nemico
            }
        }
        
    // Il ciclo si ripete se: la cella è un muro/ostacolo, se siamo nella Safe Zone (in alto a sx) o se la cella è già occupata
    } while(Mappa.GetPos(YLoc, XLoc) != 0 || (YLoc <= 5 && XLoc <= 5) || occupato);
    
    //Scansione sequenziale delle 4 caselle vicine per scegliere dove guardare all'inizio:
    // Controllo sopra
    if (Mappa.GetPos(YLoc - 1, XLoc) == 0) {
        Dir = Direction::UP; // Imposta la direzione verso l'alto
        return;              
    }

    // Controllo sotto
    if (Mappa.GetPos(YLoc + 1, XLoc) == 0) {
        Dir = Direction::DOWN; // Imposta la direzione verso il basso
        return;                
    }

    // Controllo sinistra
    if (Mappa.GetPos(YLoc, XLoc - 1) == 0) {
        Dir = Direction::LEFT; // Imposta la direzione verso sinistra
        return;                
    }

    // Controllo destra
    if (Mappa.GetPos(YLoc, XLoc + 1) == 0) {
        Dir = Direction::RIGHT; // Imposta la direzione verso destra
        return;                 
    }
}

bool Enemy::MoveInCurrDirection(Map &Mappa) {
    // Variabili per calcolare in anticipo le coordinate del passo successivo
    int nextY = YLoc;
    int nextX = XLoc;
    
    // Recupera le dimensioni calpestabili della mappa dalle costanti globali
    int h = Map::getHeight();
    int w = Map::getWidth();
    
    // Calcolo di nextY in base alla direzione attuale
    if (Dir == Direction::DOWN) {
        nextY = YLoc + 1; 
    } else if (Dir == Direction::UP) {
        nextY = YLoc - 1; 
    }
    // Calcolo di nextX in base alla direzione attuale
    if (Dir == Direction::RIGHT) {
        nextX = XLoc + 1; 
    } else if (Dir == Direction::LEFT) {
        nextX = XLoc - 1; 
    }
    // Se la direzione è UP o DOWN, nextX rimane invariato (uguale a XLoc)
    
    // Controllo macroscopico anti-collisione per verificare se il passo è illegale
    if (nextY <= 0 || nextY >= h - 1 ||                 // Collisione con i bordi esterni superiore o inferiore
        nextX <= 0 || nextX >= w - 1 ||                 // Collisione con i bordi esterni sinistro o destro
        Mappa.GetPos(nextY, nextX) == 1 ||              // Collisione con un muro indistruttibile
        Mappa.GetPos(nextY, nextX) == 2 ||              // Collisione con un muro distruttibile
        Mappa.GetPos(nextY, nextX) > 9 ||               // Collisione con elementi speciali (es. bombe attive o power-up raccolti)
        (nextY <= 3 && nextX <= 3)) {                   // Impedisce l'accesso alla Safe Zone iniziale del giocatore
        
        // Se il passo è illegale, il nemico attiva la logica di rimbalzo invertendo la marcia
        switch(Dir) {
            case Direction::DOWN:  
                Dir = Direction::UP;    
                break;
            case Direction::UP:    
                Dir = Direction::DOWN;  
                break;
            case Direction::LEFT:  
                Dir = Direction::RIGHT; 
                break;
            case Direction::RIGHT: 
                Dir = Direction::LEFT;  
                break;
        }
        return false; // Ritorna false per segnalare che il movimento fisico è fallito
    }
    
    // Se la cella è perfettamente calpestabile, esegue lo spostamento a schermo
    erase(Mappa);     // Ridisegna la cella precedente per cancellare la vecchia scia del nemico
    YLoc = nextY;     // Aggiorna la coordinata reale Y
    XLoc = nextX;     // Aggiorna la coordinata reale X
    display();        // Disegna l'icona del nemico nella nuova posizione
    
    return true;      // Ritorna true per confermare il successo dello spostamento
}

//Rimuove il nemico dalla mappa ridisegnando la cella precedente
void Enemy::erase(Map &Mappa) {
    Mappa.renderPos(YLoc, XLoc);
}

//Disegna l'icona del nemico
void Enemy::display() {
    wattron(curwin, colorPair);
    mvwaddch(curwin, YLoc, XLoc, character);
    wattroff(curwin, colorPair);
}

//Imposta lo stato alive a false
void Enemy::kill(Map &Mappa) {
    alive = false;
    erase(Mappa);
}

//
void Enemy::update(Map &Mappa, Player &pl) {
    return;
}

int Enemy::getY() const { return YLoc; }
int Enemy::getX() const { return XLoc; }


// ============================================================================
// METODI DI GESTIONE NEMICI SPECIFICI (SOTTOCLASSI ENEMY)
// ============================================================================

void RandomEnemy::update(Map &Mappa, Player &pl) {
    if (!isAlive()) {
        erase(Mappa);
        return;
    }

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now(); //si potrebbe usare anche una variabile di tipo auto
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMove).count();
    
    if (elapsed < MoveInterval) return;
    lastMove = now;
    
    if (rand() % 3 == 0) { 
        Direction dirs[4] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
        Dir = dirs[rand() % 4];
    }
    
    MoveInCurrDirection(Mappa);
}
