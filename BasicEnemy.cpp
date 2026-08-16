#include "BasicEnemy.hpp"
#include <cmath>

void BasicEnemy::update(Map &Mappa, Player &pl) {
    if (!isAlive()) {
        erase(Mappa);
        return;
    }
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMove).count();
    
    if (elapsed >= MoveInterval) {
        MoveInCurrDirection(Mappa); 
        lastMove = now;
    }
}