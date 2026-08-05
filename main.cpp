#include <ncurses.h>
#include "UIManager.hpp"
#include "GameEngine.hpp"

int main() {
    // Inizializza l'interfaccia e i colori[cite: 1]
    UIManager::initializeUI();
    keypad(stdscr, TRUE);
    
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    
    // Creiamo il motore di gioco e avviamo la partita
    GameEngine engine(yMax, xMax);
    engine.run();
    
    // Chiusura pulita[cite: 1]
    endwin();
    return 0;
}