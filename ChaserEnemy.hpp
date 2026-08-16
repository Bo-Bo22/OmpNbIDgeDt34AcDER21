#ifndef CHASERENEMY_HPP
#define CHASERENEMY_HPP

#include "Enemy.hpp"
#include <ncurses.h>
#include <chrono>

class Map;
class Player;

class ChaserEnemy : public Enemy {
protected:
    std::chrono::steady_clock::time_point lastMove;

public:
    ChaserEnemy(int startX, int startY, char sym, WINDOW* win, Direction dir, int moveInterval, int cp);
    
    void update(Map &Mappa, Player &pl);
    void spawna_casuale(Map &Mappa, Enemy** altri, int n);
    Direction calcolaBFS(Player &Pl, Map &Mappa);
};

#endif