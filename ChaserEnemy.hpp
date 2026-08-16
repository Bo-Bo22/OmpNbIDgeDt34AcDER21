#ifndef CHASERENEMY_HPP
#define CHASERENEMY_HPP

#include "Enemy.hpp"

// Sottoclasse 3: ChaserEnemy (L'inseguitore intelligente)
class ChaserEnemy : public Enemy {
protected:
    int MoveInterval;
    std::chrono::steady_clock::time_point lastMove;

public:
    ChaserEnemy(int startX, int startY, char sym, WINDOW* win, Direction dir, int moveInterval);
    
    void update(Map &Mappa, Player &pl);
    void spawna_casuale(Map &Mappa, Enemy** altri, int n);
    
    // Metodo interno che calcola il passo BFS
    Direction calcolaBFS(Player &Pl, Map &Mappa);
};

#endif