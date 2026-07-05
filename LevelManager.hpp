#include <ncurses.h>
#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

class Map;

struct LivelloNode {
    int numeroLivello;
    Map* map;
    LivelloNode* prev;
    LivelloNode* next;
};
typedef LivelloNode *ListaLivelli;

class LevelManager{

protected:
ListaLivelli H; //puntatore alla testa, Head
ListaLivelli T; //puntatore alla coda, Tail

public:

LevelManager();


Map& AddLevel(int livello, int Ymax);
Map& nextLevel(int yMax);
Map& prevLevel();

};

#endif