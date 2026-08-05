#include <ncurses.h>
#ifndef MAP_HPP
#define MAP_HPP

#include "UIManager.hpp"
#include "Enemy.hpp"

class Map {
protected:
    static const int righe =20;  // Se metti static davanti a un membro non const, esiste una sola copia condivisa da tutti gli oggetti della classe.
    static const int colonne = 40; 
    int startX;
    int startY;
    int map[righe][colonne];
    int NLivello;
    static const int NMuriDistruttibili=50; //ho scelto arbitrariamente un numero di muri distruttibili che verranno aggiunti alla mappa (in questo momento sono 50 su 1225 celle totali)
    static const int MaxMuriDistruttibili=2000;
    int* MuriDistruttibili[MaxMuriDistruttibili];
    int portaY;
    int portaX;
    int oggettiNascosti[20][40];

    //legenda degli elementi della mappa.
    char legenda[16] = {' ', '#', '+', '>', '<', // elementi basilari
                      '$', 'U', 'F', 'S', 'H',
                      '+', '+', '+', '+', '+',
                      '+'};     // power-up

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
    
    static int getWidth() { return colonne; }
    static int getHeight() { return righe; }


    //funzione che disegna la mappa a partire dalla matrice map.
    void renderLevel();

    //funzione che permette di inserire un carattere a seconda del valore in legenda in una posizione. 
    void renderPos(int y, int x);


    //funzione che permette di modificare i valori della matrice di livello.
    void setPos(int y, int x, int val);

    //metodi getter
    int getPortaY() { return portaY; }
    int getPortaX() { return portaX; }
    int getOggettoNascosto(int y, int x) {return oggettiNascosti[y][x]; }

};

#endif