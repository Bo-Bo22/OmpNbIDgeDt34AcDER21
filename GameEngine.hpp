#ifndef GAMEENGINE_HPP
#define GAMEENGINE_HPP

#include <ncurses.h>
#include <chrono>
#include <fstream>
#include <cstring>
#include "Player.hpp"
#include "Map.hpp"
#include "LevelManager.hpp"
#include "Bomb.hpp"
#include "Enemy.hpp"
#include "MainMenu.hpp"
#include "Item.hpp"
#include "BasicEnemy.hpp"
#include "EasyChaserEnemy.hpp"
#include "ChaserEnemy.hpp"
#include "RandomEnemy.hpp"

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

    Enemy* nemiciPerLivello[50][30]; 
    int numNemiciPerLivello[50];
    bool livelloVisitato[50];

    // Metodi privati ausiliari
    void setupGameScreen();
    void generateEnemies();
    void drawHUD();
    void handleBombPlacement(int tasto);
    void updateAndDrawBombs();
    void resetGameVariables();
    bool checkEnemyCollisions();

    void checkItemCollisions();

    //Variabili per la gestione del tempo di gioco
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    bool timerRunning;

    int maxLevelReached; // Variabile per tenere traccia del livello massimo raggiunto

public:
    GameEngine(int y, int x);
    
    void run(); // Il ciclo principale

    int getNumeroNemici();

    void showGameOverScreen();

    void saveScore();

    void showLeaderboard();

    bool checkBombCollisions(); // Controlla se il giocatore è stato colpito da un'esplosione
    
    Enemy* getNemico(int indice);
};

#endif