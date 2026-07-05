#include <ncurses.h>
#include <iostream>
#include <ctime>
#include "Player.hpp"
#include "Map.hpp"
#include "LevelManager.hpp"
#include "UIManager.hpp"
#include "MenuManager.hpp"
using namespace std;

int main() {

    // Inizializza l'UI con sfondo nero
    UIManager::initializeUI();

    int xMax, yMax;
    getmaxyx(stdscr, yMax, xMax);
    
    MenuManager menu;
    
    // Mostra il menu principale
    int menuChoice = menu.showMainMenu();
    
    if (menuChoice == 1) {
        // Gioca
        clear();
        refresh();
    } else if (menuChoice == 2) {
        // Lista Livelli - per ora non implementato, torna al menu
        endwin();
        return 0;
    } else if (menuChoice == 3) {
        // Classifica - per ora non implementato, torna al menu
        endwin();
        return 0;
    }

    LevelManager manager;
    // Usiamo un PUNTATORE alla mappa corrente
    Map* currentMap = &manager.AddLevel(1, yMax);
    
    Player p(currentMap->getWin(), 1, 1, '@');
    
    // Ottieni le coordinate della mappa per disegnare la cornice
    int startY, startX;
    UIManager::getCenterCoordinates(Map::getWidth(), Map::getHeight(), startY, startX);
    
    // Disegna la cornice una sola volta all'inizio
    UIManager::drawBorder(Map::getWidth(), Map::getHeight(), startY, startX);
    currentMap->refresh();
    p.display();
    wrefresh(currentMap->getWin());

while(true) {
    p.getmv(*currentMap);
    
    // Controlla se l'utente vuole tornare al menu
    if(p.getReturnToMenu()) {
        p.erase();
        clear();
        refresh();
        
        // Ritorna al menu
        int menuChoice = menu.showMainMenu();
        if (menuChoice == 1) {
            // Riavvia il gioco
            clear();
            refresh();
            currentMap = &manager.AddLevel(1, yMax);
            p.setWindow(currentMap->getWin());
            p.resetPosition();
            p.resetLevelFlags();
            UIManager::getCenterCoordinates(Map::getWidth(), Map::getHeight(), startY, startX);
            UIManager::drawBorder(Map::getWidth(), Map::getHeight(), startY, startX);
            currentMap->refresh();
            p.display();
            wrefresh(currentMap->getWin());
        } else {
            // Esci
            endwin();
            return 0;
        }
    }

    if(p.ReturnNextLevel()) {
        p.erase(); // <--- 1. CANCELLA IL GIOCATORE DALLA VECCHIA MAPPA

        currentMap = &manager.nextLevel(yMax);
        
        p.setWindow(currentMap->getWin()); 
        p.resetPosition(); // <--- 2. ORA PUOI RESETTARE LE COORDINATE (1,1)
        p.resetLevelFlags();

        clear(); 
        touchwin(currentMap->getWin());
        UIManager::getCenterCoordinates(Map::getWidth(), Map::getHeight(), startY, startX);
        UIManager::drawBorder(Map::getWidth(), Map::getHeight(), startY, startX);
        currentMap->refresh();
        p.display();
        wrefresh(currentMap->getWin());
    }

    if(p.ReturnPrevLevel()) {
        p.erase(); // <--- 1. CANCELLA IL GIOCATORE DALLA VECCHIA MAPPA

        currentMap = &manager.prevLevel();
        
        p.setWindow(currentMap->getWin());
        p.resetPosition();
        p.resetLevelFlags();

        clear(); 
        touchwin(currentMap->getWin());
        UIManager::getCenterCoordinates(Map::getWidth(), Map::getHeight(), startY, startX);
        UIManager::drawBorder(Map::getWidth(), Map::getHeight(), startY, startX);
        currentMap->refresh();
        p.display();
        wrefresh(currentMap->getWin());
    }

    // Rendering standard
    currentMap->refresh(); 
    p.display();
    wrefresh(currentMap->getWin());
}
    endwin();
}