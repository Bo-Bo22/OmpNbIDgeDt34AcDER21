#ifndef MAP_HPP
#define MAP_HPP

#include "UIManager.hpp"
#include "Enemy.hpp"
#include <ncurses.h>


const int MAP_RIGHE = 20;
const int MAP_COLONNE = 40;
const int MAX_MURI_DISTRUTTIBILI = 700;

class Map {

protected:
    int righe;
    int colonne;
    int startX;
    int startY;

    int map[MAP_RIGHE][MAP_COLONNE];
    int oggettiNascosti[MAP_RIGHE][MAP_COLONNE];
    int* MuriDistruttibili[MAX_MURI_DISTRUTTIBILI]; // Array di puntatori per i muri distruttibili, ottimizzato a 20x40-(~100) = 700

    int NLivello;
    int portaY;
    int portaX;

    char legenda[6];
    WINDOW* playwin;

public:
    Map(int yMax);

    //wrefresh su mappa. 
    void refresh();
    //inizializza la mappa.
    void initmap(int l);

    //ritorna il valore della matrice mappa, a seconda del tipo di muro o porta.
    int GetPos(int y, int x);

    int GetLvlN();
    
    // Restituisce il carattere da mostrare per una cella della mappa
    char GetMapChar(int y, int x);
    
    // Ridisegna una cella della mappa
    void RedrawCell(int y, int x);

    WINDOW* getWin();
    
    int getWidth() const;
    int getHeight() const;


    //funzione che disegna la mappa a partire dalla matrice map.
    void renderLevel();

    //funzione che permette di inserire un carattere a seconda del valore in legenda in una posizione. 
    void renderPos(int y, int x);


    //funzione che permette di modificare i valori della matrice di livello.
    void setPos(int y, int x, int val);

    //metodi getter
    int getPortaY();
    int getPortaX();
    int getOggettoNascosto(int y, int x);

};

#endif