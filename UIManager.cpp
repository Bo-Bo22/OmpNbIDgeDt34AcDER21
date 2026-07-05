#include "UIManager.hpp"
#include <ncurses.h>

UIManager::UIManager() {}

UIManager::~UIManager() {}

void UIManager::initializeUI() {
    // Inizializza ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    
    // Abilita i colori
    if (has_colors()) {
        start_color();
        
        // Definiamo una coppia di colori: testo bianco su sfondo nero
        init_pair(COLOR_PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
        
        // Applica il background nero a tutto lo schermo
        bkgd(COLOR_PAIR(COLOR_PAIR_DEFAULT) | ' ');
        attron(COLOR_PAIR(COLOR_PAIR_DEFAULT));
    }
    
    // Pulisci lo schermo
    clear();
    refresh();
}

void UIManager::setBlackBackground(WINDOW* win) {
    if (has_colors()) {
        wbkgd(win, COLOR_PAIR(COLOR_PAIR_DEFAULT) | ' ');
        wattron(win, COLOR_PAIR(COLOR_PAIR_DEFAULT));
    }
}

void UIManager::getCenterCoordinates(int width, int height, int& startY, int& startX) {
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    
    startY = (yMax - height) / 2;
    startX = (xMax - width) / 2;
    
    // Assicurati che le coordinate non siano negative
    if (startY < 0) startY = 0;
    if (startX < 0) startX = 0;
}

void UIManager::drawBorder(int width, int height, int startY, int startX) {
    // Disegna una cornice ASCII intorno alla mappa
    int borderY = startY - 1;
    int borderX = startX - 1;
    int borderWidth = width + 2;
    int borderHeight = height + 2;
    
    // Angoli
    mvaddch(borderY, borderX, '+');
    mvaddch(borderY, borderX + borderWidth - 1, '+');
    mvaddch(borderY + borderHeight - 1, borderX, '+');
    mvaddch(borderY + borderHeight - 1, borderX + borderWidth - 1, '+');
    
    // Linee orizzontali
    for (int i = 1; i < borderWidth - 1; i++) {
        mvaddch(borderY, borderX + i, '-');
        mvaddch(borderY + borderHeight - 1, borderX + i, '-');
    }
    
    // Linee verticali
    for (int i = 1; i < borderHeight - 1; i++) {
        mvaddch(borderY + i, borderX, '|');
        mvaddch(borderY + i, borderX + borderWidth - 1, '|');
    }
    
    refresh();
}
