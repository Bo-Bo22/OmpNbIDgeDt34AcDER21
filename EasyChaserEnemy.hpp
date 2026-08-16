#ifndef EASYCHASERENEMY_HPP
#define EASYCHASERENEMY_HPP

#include <ncurses.h>
#include <chrono>
#include "Player.hpp"
#include "Map.hpp"

// Sottoclasse EasyChaserEnemy (L'inseguitore "stupido")
class EasyChaserEnemy : public Enemy {
public:
    EasyChaserEnemy(int x, int y, char c, WINDOW *win, Direction dir, int cp) :
        Enemy(x, y, c, win, dir, cp) {
        }
        
    void update(Map &Mappa, Player &pl);
    bool MoveToPlayer(Map &Mappa, Player &pl); 

};
#endif