#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <ncurses.h>

class UIManager {
public:
    UIManager();
    ~UIManager();
    
    // Inizializza ncurses con colori e background nero
    static void initializeUI();
    
    // Applica il background nero a una finestra
    static void setBlackBackground(WINDOW* win);
    
    // Calcola le coordinate per centrare una finestra
    static void getCenterCoordinates(int width, int height, int& startY, int& startX);
    
    // Disegna una cornice ASCII attorno alla mappa
    static void drawBorder(int width, int height, int startY, int startX);
    
private:
    static const int COLOR_PAIR_DEFAULT = 1;
};

#endif
