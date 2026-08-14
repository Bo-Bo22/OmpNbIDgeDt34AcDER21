#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <ncurses.h>
#include <chrono>
#include "Player.hpp"
#include "Map.hpp"

class Map;

// Enumerazione esplicitamente concessa dalle specifiche per gestire le direzioni
enum class Direction { UP, DOWN, RIGHT, LEFT };

// Struttura ausiliaria per coordinate e valori
struct cursore {
    int x, y;
    int val;
};
typedef cursore* pcurs;

// ============================================================================
// Classe base: Enemy
// ============================================================================
class Enemy {
protected:
    int XLoc, YLoc;         // Coordinate cartesiane attuali del nemico sulla griglia
    char character;         // Carattere ASCII usato per renderizzare il nemico (es. 'E', 'R', 'C')
    WINDOW *curwin;         // Puntatore alla finestra ncurses su cui disegnare
    bool alive;             // Flag di stato: indica se il nemico è vivo o è stato eliminato
    int speed;              // Parametro di velocità
    Direction Dir;          // Direzione attuale di marcia del nemico
    
    // Gestione temporale nativa C++ per evitare l'uso di thread esterni vietati
    std::chrono::time_point<std::chrono::steady_clock> lastMove; 
    int MoveInterval;       // Tempo di attesa in millisecondi tra un passo e il successivo
    int colorPair;          // Indice della coppia di colori ncurses assegnata al nemico

public:
    // Costruttore della classe base
    Enemy(int x, int y, char c, WINDOW *win, Direction dir, int cp);
    
    // Primitives di movimento predittivo
    bool mvup(Map &Mappa);
    bool mvdown(Map &Mappa);
    bool mvright(Map &Mappa);
    bool mvleft(Map &Mappa);
    
    // Getters stabili in sola lettura
    int getX() const;
    int getY() const;
    bool isAlive() const { return alive; }
    
    // Funzioni logiche, di pulizia e di rendering sul terminale
    bool MoveInCurrDirection(Map &Mappa);
    void kill(Map &Mappa);
    void display();
    void erase(Map &Mappa);
    
    // Metodi virtuali polimorfici specializzati nelle sottoclassi
    virtual void update(Map &Mappa, Player &pl);
    virtual void spawna_casuale(Map &Mappa, Enemy** altri, int n);
};


// ============================================================================
// Sottoclasse 1: BasicEnemy (Il nemico standard)
// ============================================================================
class BasicEnemy : public Enemy {
public:
    BasicEnemy(int x, int y, char c, WINDOW *win, Direction dir, int cp) :
        Enemy(x, y, c, win, dir, cp) {}
        
    void update(Map &Mappa, Player &pl);
};

// ============================================================================
// Sottoclasse 2: RandomEnemy (Il nemico imprevedibile)
// ============================================================================
class RandomEnemy : public Enemy {
public:
    RandomEnemy(int x, int y, char c, WINDOW *win, Direction dir, int cp) :
        Enemy(x, y, c, win, dir, cp) {
        }
        
    void update(Map &Mappa, Player &pl);
};

// ============================================================================
// Sottoclasse 3: ChaserEnemy (L'inseguitore intelligente)
// ============================================================================
class ChaserEnemy : public Enemy {
public:
    ChaserEnemy(int x, int y, char c, WINDOW *win, Direction dir, int cp) :
        Enemy(x, y, c, win, dir, cp) {
            MoveInterval = 250; // Leggermente più lento per bilanciare l'IA infallibile, in realtà è facile da
    }
    // Metodo privato che esegue l'algoritmo del percorso minimo
    Direction performBFS(Player &Pl, Map &Mappa);
    void update(Map &Mappa, Player &Pl) override;
    void spawna_casuale(Map &Mappa, Enemy** altri, int n) override;
};

// ============================================================================
// Sottoclasse 4: EasyChaserEnemy (L'inseguitore "stupido")
// ============================================================================
class EasyChaserEnemy : public Enemy {
public:
    EasyChaserEnemy(int x, int y, char c, WINDOW *win, Direction dir, int cp) :
        Enemy(x, y, c, win, dir, cp) {
        }
        
    void update(Map &Mappa, Player &pl);
    bool MoveToPlayer(Map &Mappa, Player &pl); 

};
#endif