#ifndef BASICENEMY_HPP
#define BASICENEMY_HPP

#include <ncurses.h>
#include <chrono>
#include "Player.hpp"
#include "Map.hpp"

// Sottoclasse BasicEnemy (Il nemico standard)
class BasicEnemy : public Enemy {
public:
    BasicEnemy(int x, int y, char c, WINDOW *win, Direction dir, int moveInterval, int cp);
    void update(Map &Mappa, Player &pl);
};

#endif