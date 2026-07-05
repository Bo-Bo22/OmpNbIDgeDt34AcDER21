#include <ncurses.h>
#ifndef PLAYER_HPP
#define PLAYER_HPP

class Map;

class Player {
protected:
    int xLoc, yLoc, xMax, yMax;
    char character;
    WINDOW * curwin;
    bool NextLevel;
    bool PrevLevel;
    bool ReturnToMenu;

public:
    Player (WINDOW* win, int y, int x, char c);
    
    void mvup(Map &Mappa);
    void mvdown(Map &Mappa);
    void mvleft(Map &Mappa);
    void mvright(Map &Mappa);

    int getmv(Map &Mappa);
    void display();
    void movement(WINDOW * win, Map &Mappa);

    int getX() const;
    int getY() const;

    
    bool ReturnNextLevel();
    bool ReturnPrevLevel();
    bool getReturnToMenu() const;
    void resetLevelFlags();

    void resetPosition();
    void setWindow(WINDOW* win); //aggiorna la finestra corrente
    void erase();
    void redrawPreviousCell(Map &Mappa); // Ridisegna la cella precedente

};

#endif