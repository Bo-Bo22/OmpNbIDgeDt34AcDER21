#ifndef BFS_HPP
#define BFS_HPP

#include "Map.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include <ncurses.h>

// ============================================================================
// CLASSE: BFS
// Modulo di pathfinding a scansione per grafi a griglia non pesati
// ============================================================================
class BFS {
protected:
    // Eventuali parametri di configurazione protetti

public:
    // Calcola il percorso a ritroso e restituisce la direzione ottimale
    Direction getDirection(Player &Pl, Enemy &En, Map &Mappa);
};

#endif