#ifndef RANDOMENEMY_HPP
#define RANDOMENEMY_HPP

#include <ncurses.h>
#include <chrono>
#include "Player.hpp"
#include "Map.hpp"

// Sottoclasse RandomEnemy (Il nemico che si muove in modo casuale)
class RandomEnemy : public Enemy {
public:
    RandomEnemy(int x, int y, char c, WINDOW *win, Direction dir, int cp) :
        Enemy(x, y, c, win, dir, cp) {
        }
        
    void update(Map &Mappa, Player &pl);
};

#endif