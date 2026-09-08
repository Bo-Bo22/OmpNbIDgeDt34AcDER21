#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include <ncurses.h>
#include "Map.hpp"

struct LivelloNode {
    int numeroLivello;
    Map* map;
    LivelloNode* prev;
    LivelloNode* next;
};
typedef LivelloNode* ListaLivelli;

class LevelManager {
protected:
    ListaLivelli H; // Puntatore alla testa (Head)
    ListaLivelli T; // Puntatore alla coda (Tail)

public:
    LevelManager();

    Map& AddLevel(int livello, int Ymax);
    Map& nextLevel(int yMax);
    Map& prevLevel();
};

#endif
