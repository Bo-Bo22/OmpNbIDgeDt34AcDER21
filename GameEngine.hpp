#ifndef GAMEENGINE_HPP
#define GAMEENGINE_HPP

#include <ncurses.h>
#include <chrono>
#include "Player.hpp"
#include "Map.hpp"
#include "LevelManager.hpp"
#include "Bomb.hpp"
#include "Enemy.hpp"
#include "MainMenu.hpp"

class GameEngine {
protected:
    int yMax, xMax;
    bool inGame;
    
    LevelManager manager;
    Map* currentMap;
    Player* p;
    
    // Gestione Bombe
    static const int MAX_BOMBE = 3;
    Bomb* bombeAttive[MAX_BOMBE];
    
    // Gestione Nemici (Predisposizione)
    static const int MAX_NEMICI = 10;
    Enemy* arrayNemici[MAX_NEMICI];
    int numeroNemici;

    // Metodi privati ausiliari
    void setupGameScreen();
    void generateEnemies();
    void drawHUD();
    void handleBombPlacement(int tasto);
    void updateAndDrawBombs();
    void resetGameVariables();
    bool checkEnemyCollisions();

    //Variabili per la gestione del tempo di gioco
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    bool timerRunning;

    int maxLevelReached; // Variabile per tenere traccia del livello massimo raggiunto

public:
    GameEngine(int y, int x);
    
    void run(); // Il ciclo principale

    int getNumeroNemici() { return numeroNemici; }

    void showGameOverScreen();
    
    // Invece di restituire tutto l'array, creiamo un metodo che restituisce il singolo nemico
    Enemy* getNemico(int indice) { 
        if (indice >= 0 && indice < numeroNemici) {
            return arrayNemici[indice]; 
        }
        return NULL;
    }
};

#endif