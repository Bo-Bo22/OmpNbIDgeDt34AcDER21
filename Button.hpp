#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <ncurses.h>
#include <string>

enum class ButtonState{NORMAL, SELECTED, PRESSING};

class Button{
    int y,x;
    int y_str,x_str;
    int width;
    std::string label;
    ButtonState state;
    WINDOW* win;
    WINDOW* btnWin; // 3 righe, 20 colonne, posizione y,x


    bool blinkOn;
    int blinkTick;

public:
    Button(WINDOW* win, int y, int x, int y_str, int x_str, int width, const std::string & label);
    void draw();
    void tick();
    void setState(ButtonState s);
    int getWidth();
    const std::string& getLabel() const;

    ButtonState getState() const;
};


#endif