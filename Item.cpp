#include "Item.hpp"

// Costruttore: inizializza l'oggetto rendendolo attivo
Item::Item(int startY, int startX, int itemType) {
    y = startY;
    x = startX;
    type = itemType;
    active = true;
}

void Item::setInactive() {
    active = false;
}

void Item::display(WINDOW* win) {
    if (active) {
        // Scegliamo il carattere in base al tipo di power-up
        char simbolo;
        
        switch (type) {
            case 1: 
                simbolo = 'V'; // Esempio: Extra Vita
                break;
            case 2: 
                simbolo = 'B'; // Esempio: Bomba Extra
                break;
            case 3: 
                simbolo = 'F'; // Esempio: Flare Boost (Fiamma più lunga)
                break;
            default: 
                simbolo = '?'; // Oggetto sconosciuto
                break;
        }

        // Usiamo l'attributo BOLD per farli risaltare rispetto ai muri
        wattron(win, A_BOLD);
        mvwaddch(win, y, x, simbolo);
        wattroff(win, A_BOLD);
    }
}

int Item::getX()  { 
    return x; 
}

int Item::getY()  { 
    return y; 
}

int Item::getType()  {
     return type; 
}

bool Item::isActive()  {
     return active; 
}