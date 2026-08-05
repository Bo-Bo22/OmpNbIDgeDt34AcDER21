#ifndef BOMB_HPP
#define BOMB_HPP


#include <chrono>
#include <ncurses.h>

#include "Map.hpp"
#include "Player.hpp"
#include "Enemy.hpp"


class Bomb{

protected:

    WINDOW *curwin;

    int xLoc, yLoc;
    int XMax, YMax;
    int score;

    bool active; //questi booleani gestiscono lo stato della bomba. serviranno tutti in update.
    bool visible;
    bool exploding;

    std::chrono::time_point<std::chrono::steady_clock> last_time_point;
    std::chrono::time_point<std::chrono::steady_clock> explod_time;
    std::chrono::time_point<std::chrono::steady_clock> display_tick;
    int delay;  

    //per gestire gli item
    int est_bombe;
    bool flare_boost;

public:

    Bomb(int x, int y, WINDOW* win, int estensione, bool fl_bst);

    int getX();
    int getY();

    bool update(Map &Mappa, Player &Pl, Enemy** En, int n_nemici);

    //mi permette di controllare il tipo di esplosione, perforante o meno
    bool direzione[4] = {true, true, true, true};


    void forceExplode();
    bool isExploding();

    int getScore() const;

    void display();

    bool isActive();

};


#endif