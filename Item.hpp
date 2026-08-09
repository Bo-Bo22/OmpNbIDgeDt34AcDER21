#ifndef ITEM_HPP
#define ITEM_HPP

#include <ncurses.h>

class Item {
private:
    int x, y;
    int type;      // Identifica quale potenziamento è (es. 1 = Vita, 2 = Bomba, 3 = Raggio Fiamma)
    bool active;   // True se è a schermo, False se è stato raccolto

public:
    // Costruttore
    Item(int startY, int startX, int itemType);

    // Getters
    int getX() ;
    int getY() ;
    int getType() ;
    bool isActive() ;

    // Setters
    void setInactive(); // Chiama questa quando il giocatore raccoglie l'oggetto

    // Disegna l'oggetto sulla mappa
    void display(WINDOW* win);
};

#endif