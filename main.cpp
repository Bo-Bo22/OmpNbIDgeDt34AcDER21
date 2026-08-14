#include <ncurses.h>
#include "UIManager.hpp"
#include "GameEngine.hpp"

int main() {
    // Inizializza l'interfaccia e i colori[cite: 1]
    UIManager::initializeUI();
    keypad(stdscr, TRUE);

    int termY, termX;
    getmaxyx(stdscr, termY, termX);

    // Dimensioni minime necessarie per contenere mappa (20x40) e HUD
    const int MIN_RIGHE = 24;
    const int MIN_COLONNE = 50;

    if (termY < MIN_RIGHE || termX < MIN_COLONNE) {
    endwin();
    printf("ERRORE: La finestra del terminale e' troppo piccola!\n");
    printf("Dimensioni minime richieste: %dx%d (Attuali: %dx%d)\n", MIN_COLONNE, MIN_RIGHE, termX, termY);
    printf("Ingrandisci il terminale e riavvia il gioco.\n");
    return 1;
    }
    
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    
    // Creiamo il motore di gioco e avviamo la partita
    GameEngine engine(yMax, xMax);
    engine.run();
    
    // Chiusura pulita[cite: 1]
    endwin();
    return 0;
}