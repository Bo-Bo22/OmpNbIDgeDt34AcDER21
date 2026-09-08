#include "RandomEnemy.hpp"
#include <cstdlib>

RandomEnemy::RandomEnemy(int startX, int startY, char sym, WINDOW* win, Direction dir, int moveInterval, int cp)
    : Enemy(startX, startY, sym, win, dir, cp) {
    lastMove = std::chrono::steady_clock::now();
    
    MoveInterval = moveInterval; // Bilanciamento all'IA infallibile
}

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
