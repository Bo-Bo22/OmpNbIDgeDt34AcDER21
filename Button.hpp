#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <ncurses.h>
#include <cstring> 

// Migliora la leggibilità, si potrebbe implementare con una variabile "state" di tipo int
// a cui vengono associati dei valori per quando un bottone è normale, selezionato o premuto
enum class ButtonState { NORMAL, SELECTED, PRESSING };

class Button {

protected:

    int y, x;
    int y_str, x_str;
    int width;
    char label[32];
    ButtonState state;
    WINDOW* win;
    WINDOW* btnWin; 
    bool blinkOn;
    int blinkTick;

public:
    
    Button(WINDOW* win, int y, int x, int y_str, int x_str, int width, const char* label);
    void draw();
    void tick();
    void setState(ButtonState s);

    
    // Ritorna un puntatore a const char compatibile con cstring
    const char* getLabel() const;
    ButtonState getState() const;
};

#endif