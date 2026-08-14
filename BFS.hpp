#ifndef BFS_HPP
#define BFS_HPP

#include <ncurses.h>
// Includi SOLO il file dove è definito l'enum Direction (presumo sia in Enemy o GameEngine)
#include "Enemy.hpp" 

// Forward declarations: diciamo al compilatore "Fidati, queste classi esistono"
class Player;
class Map;
class Enemy;

class BFS {
public:
    Direction getDirection(Player &Pl, Enemy &En, Map &Mappa);
};

#endif