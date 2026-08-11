#include "UIManager.hpp"
#include <ncurses.h>

UIManager::UIManager() {}

// Inizializza ncurses con colori e background nero
void UIManager::initializeUI() {

    //classica inizializzazione di ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
         

    if (has_colors()) { //controlla se il terminale supporta i colori
        start_color();  //avvia il motore dei colori
                 
        // Coppia 1: Testo Bianco su Sfondo Nero (Standard)
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        
        // Coppia 2: Testo Rosso su Sfondo Nero (Per il lampeggiamento)
        init_pair(2, COLOR_RED, COLOR_BLACK);

        init_pair(3, COLOR_GREEN, COLOR_BLACK);      // Coppia 3: Verde su Nero, per le porte
        init_pair(4, COLOR_CYAN, COLOR_BLACK);       // Coppia 4: Ciano su Nero, per il giocatore
        init_pair(5, COLOR_YELLOW, COLOR_BLACK);     // Coppia 5: Giallo su Nero, per gli oggetti ($)

        init_pair(6, COLOR_RED, COLOR_BLACK);        // Coppia 6: Rosso su Nero (es. per il Chaser)
        init_pair(7, COLOR_YELLOW, COLOR_BLACK);    // Coppia 7: Giallo su Nero per il Random)
        init_pair(8, COLOR_CYAN, COLOR_BLACK);      // Coppia 8: Cyan su Nero per il Basic)
        init_pair(9, COLOR_MAGENTA, COLOR_BLACK);   // Coppia 9: Magenta su Nero per il EasyChaser)

        // Definiamo i colori grigio chiaro e grigio scuro
        init_color(8, 500, 500, 500); 
        init_color(9, 250, 250, 250); 

        // Creiamo la coppia usando il nostro nuovo colore 8 sia per testo che per sfondo
        init_pair(11, 8, 8);
        init_pair(10, 9, 9);
                 
                 
        bkgd(COLOR_PAIR(1) | ' ');
        attron(COLOR_PAIR(1));
    }
         
    clear();
    refresh();
}

// Mette lo sfondo nero
void UIManager::setBlackBackground(WINDOW* win) {
    if (has_colors()) {
        wbkgd(win, COLOR_PAIR(1) | ' ');
        wattron(win, COLOR_PAIR(1));
    }
}

// Calcola le coordinate per centrare una finestra di dimensioni width x height
void UIManager::getCenterCoordinates(int width, int height, int& startY, int& startX) {
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    
    //in modo tale che una scatola di larghezza "width" e altezza "height" sia centrata nello schermo
    startY = (yMax - height) / 2;
    startX = (xMax - width) / 2;
    
    // Mi assicuro che le coordinate non siano negative
    if (startY < 0) startY = 0;
    if (startX < 0) startX = 0;
}


// Disegna una cornice ASCII intorno alla mappa
void UIManager::drawBorder(int width, int height, int startY, int startX) {
    // Definiamo un offset verticale di 2 spazi sopra e sotto
    int offsetY = 2;
    int offsetX = 1;
    
    // Allarghiamo i confini della cornice per includere lo spazio extra
    int borderY = startY - 1 - offsetY;       // Si alza di 2 righe in più sopra
    int borderX = startX - 1 - offsetX;       // Si sposta di 1 colonna a sinistra
    int borderWidth = width + 2 + (offsetX * 2); 
    int borderHeight = height + 2 + (offsetY * 2); // Aggiunge 2 spazi sopra e 2 sotto
    
    // Disegna gli Angoli
    mvaddch(borderY, borderX, 'o');
    mvaddch(borderY, borderX + borderWidth - 1, 'o');
    mvaddch(borderY + borderHeight - 1, borderX, 'o');
    mvaddch(borderY + borderHeight - 1, borderX + borderWidth - 1, 'o');
    
    // Disegna le Linee Orizzontali (Superiore e Inferiore)
    for (int i = 1; i < borderWidth - 1; i++) {
        mvaddch(borderY, borderX + i, '-');
        mvaddch(borderY + borderHeight - 1, borderX + i, '-');
    }
    
    // Disegna le Linee Verticali (Laterali)
    for (int i = 1; i < borderHeight - 1; i++) {
        mvaddch(borderY + i, borderX, '|');
        mvaddch(borderY + i, borderX + borderWidth - 1, '|');
    }
    
    refresh();
}
