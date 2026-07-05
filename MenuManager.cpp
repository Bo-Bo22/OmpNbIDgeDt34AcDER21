#include "MenuManager.hpp"
#include <cstring>
#include <unistd.h>
#include <cstdlib>

MenuManager::MenuManager() {}

MenuManager::~MenuManager() {}

int MenuManager::showMainMenu() {
    int xMax, yMax;
    getmaxyx(stdscr, yMax, xMax);
    
    // Crea una finestra per il menu
    int startY = (yMax - MENU_HEIGHT) / 2;
    int startX = (xMax - MENU_WIDTH) / 2;
    
    if (startY < 0) startY = 0;
    if (startX < 0) startX = 0;
    
    WINDOW* menuwin = newwin(MENU_HEIGHT, MENU_WIDTH, startY, startX);
    
    // Abilita i tasti speciali (frecce)
    keypad(menuwin, true);
    
    // Applica il background nero e disegna la cornice
    wbkgd(menuwin, COLOR_PAIR(1) | ' ');
    box(menuwin, 0, 0);
    
    // Disegna il titolo BOMBERMAN
    drawTitle(menuwin, 1, 1);
    
    int selected = 0;
    int ch = 0;
    bool running = true;
    
    while (running) {
        wclear(menuwin);
        box(menuwin, 0, 0);
        drawTitle(menuwin, 1, 1);
        
        // Disegna le opzioni del menu
        int optionY = 16;
        
        // Opzione 1: Gioca
        drawOption(menuwin, optionY, 5, "1. Gioca", selected == 0);
        
        // Opzione 2: Lista Livelli
        drawOption(menuwin, optionY + 2, 5, "2. Lista Livelli", selected == 1);
        
        // Opzione 3: Classifica
        drawOption(menuwin, optionY + 4, 5, "3. Classifica", selected == 2);
        
        wrefresh(menuwin);
        
        ch = wgetch(menuwin);
        
        switch (ch) {
            case '1':
                delwin(menuwin);
                return 1;
            case '2':
                delwin(menuwin);
                return 2;
            case '3':
                delwin(menuwin);
                return 3;
            case KEY_UP:
                selected = (selected - 1 + 3) % 3;
                break;
            case KEY_DOWN:
                selected = (selected + 1) % 3;
                break;
            case '\n':
                delwin(menuwin);
                return selected + 1;
            case 'q':
            case 'Q':
                // Esci dal programma
                delwin(menuwin);
                endwin();
                exit(0);
        }
    }
    
    delwin(menuwin);
    return 1;
}

void MenuManager::drawTitle(WINDOW* win, int startY, int startX) {
    // BOMBERMAN in ASCII art - centrato
    const char* title[] = {
        " ____   __   _  _  ____  ____  ____  _  _   __   __ _ ",
        "(  _ \\ /  \\ ( \\/ )(  _ \\(  __)(  _ \\( \\/ ) / _\\ (  ( \\",
        " ) _ ((  O )/ \\/ \\ ) _ ( ) _)  )   // \\/ \\/    \\/    /",
        "(____/ \\__/ \\_)(_/(____/(____)(__\\_)\\_)(_/\\_/\\_/\\_)__)"
    };
    
    int titleLines = sizeof(title) / sizeof(title[0]);
    int titleWidth = 54; // Larghezza della scritta ASCII
    int centerX = (MENU_WIDTH - titleWidth) / 2;
    
    for (int i = 0; i < titleLines && startY + i < MENU_HEIGHT - 1; i++) {
        mvwprintw(win, startY + i, centerX, "%s", title[i]);
    }
}

void MenuManager::drawOption(WINDOW* win, int y, int x, const char* text, bool selected) {
    if (selected) {
        // Lampeggia l'opzione selezionata
        wattron(win, A_BOLD | A_REVERSE);
        mvwprintw(win, y, x, "%-30s", text);
        wattroff(win, A_BOLD | A_REVERSE);
    } else {
        mvwprintw(win, y, x, "%-30s", text);
    }
}
