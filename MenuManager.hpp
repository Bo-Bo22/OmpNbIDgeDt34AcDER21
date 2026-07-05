#ifndef MENUMANAGER_HPP
#define MENUMANAGER_HPP

#include <ncurses.h>

class MenuManager {
public:
    MenuManager();
    ~MenuManager();
    
    // Mostra il menu principale e ritorna la scelta dell'utente
    // 1: Gioca
    // 2: Lista Livelli
    // 3: Classifica
    int showMainMenu();
    
    // Disegna il titolo BOMBERMAN in grande ASCII
    void drawTitle(WINDOW* win, int startY, int startX);
    
private:
    static const int MENU_WIDTH = 80;
    static const int MENU_HEIGHT = 30;
    
    // Disegna un'opzione del menu
    void drawOption(WINDOW* win, int y, int x, const char* text, bool selected);
    
    // Lampeggia il testo
    void blink(WINDOW* win, int times);
};

#endif
