#include <ncurses.h>
#include <chrono>

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
    int Life;

public:
    Player(WINDOW* win, int y, int x, char c);
    
    void mvup(Map &Mappa);
    void mvdown(Map &Mappa);
    void mvleft(Map &Mappa);
    void mvright(Map &Mappa);
    int getmv(Map &Mappa);
    void display();
    void movement(WINDOW * win, Map &Mappa);
    
    int getX();
    int getY();
    int getLife();
    int LifeUp();
    

    int Death(bool lampeggiante);
    
    bool ReturnNextLevel();
    bool ReturnPrevLevel();
    bool getReturnToMenu() const;
    
    void resetLevelFlags();
    void resetPosition();
    void setWindow(WINDOW* win); 
    void erase(Map &Mappa); 
    void redrawPreviousCell(Map &Mappa); 
};
#endif