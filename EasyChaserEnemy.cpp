#include "EasyChaserEnemy.hpp"

void EasyChaserEnemy::update(Map &Mappa, Player &pl) {
    if (!isAlive()) {
        erase(Mappa);
        return;
    }
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMove).count();
    
    if (elapsed >= MoveInterval) {
        MoveToPlayer(Mappa, pl); 
        lastMove = now;
    }
}

bool EasyChaserEnemy::MoveToPlayer(Map &Mappa, Player &pl) {
    int deltaX = pl.getX() - XLoc; 
    int deltaY = pl.getY() - YLoc; 

    // Variabili temporanee per decidere il "Piano A" (via diretta) e il "Piano B" (scivolamento)
    Direction dirPrimaria;
    Direction dirSecondaria;

    // DECISIONE DELLE TRAIETTORIE
    if (abs(deltaX) >= abs(deltaY)) {

        // L'asse predominante è quello Orizzontale (X)
        if (deltaX > 0) 
            { dirPrimaria = Direction::RIGHT; } 
        
            else 
                { dirPrimaria = Direction::LEFT; }
        
        // L'asse secondario è quello Verticale (Y) - serve se trova un muro
        if (deltaY > 0) 
            { dirSecondaria = Direction::DOWN; } 
            
            else 
                { dirSecondaria = Direction::UP; }
    } 
        else {
        // L'asse predominante è quello Verticale (Y)
        if (deltaY > 0) 
            { dirPrimaria = Direction::DOWN; } 
            
            else 
                { dirPrimaria = Direction::UP; }
        
        // L'asse secondario è quello Orizzontale (X) - serve se trova un muro
        if (deltaX > 0) 
            { dirSecondaria = Direction::RIGHT; }
            
            else 
                { dirSecondaria = Direction::LEFT; }
    }

    // Tentativo 1 (Via Diretta)
    Dir = dirPrimaria;
    if (MoveInCurrDirection(Mappa) == true) {
        return true; // Se la strada è libera, ha fatto il passo. Fine.
    }

    // Tentativo 2 (Scivolamento)
    // Sul Piano A c'era un muro. Si prova ad aggirarlo muovendosi sull'altro asse
    Dir = dirSecondaria;
    
    // Restituisce il risultato del Piano B (se anche questo è un muro, la MoveInCurrDirection 
    // farà il suo normale rimbalzo invertendo la marcia come ultima spiaggia).
    return MoveInCurrDirection(Mappa);
}