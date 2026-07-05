#include <ncurses.h>
#ifndef MAP_HPP
#define MAP_HPP

#include "UIManager.hpp"

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

    WINDOW* playwin;

public:
    Map(int yMax);
    void refresh();
    void initmap(int l);

    int GetPos(int y, int x);
    int GetLvlN();
    
    // Restituisce il carattere da mostrare per una cella della mappa
    char GetMapChar(int y, int x);
    
    // Ridisegna una cella della mappa
    void RedrawCell(int y, int x);

    WINDOW* getWin();
    
    static int getWidth() { return colonne; }
    static int getHeight() { return righe; }
    
};

#endif