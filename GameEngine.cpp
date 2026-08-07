#include "GameEngine.hpp"
#include "UIManager.hpp"

// Costruttore
GameEngine::GameEngine(int y, int x) {
    yMax = y;
    xMax = x;
    inGame = false;
    currentMap = NULL;
    numeroNemici = 0;
    
    // Inizializza il giocatore
    p = new Player(stdscr, 1, 1, '@');
    
    // Assicuriamoci che ogni singolo slot degli array sia NULL all'avvio
    for (int i = 0; i < MAX_BOMBE; i++) {
        bombeAttive[i] = NULL;
    }
    for (int i = 0; i < MAX_NEMICI; i++) {
        arrayNemici[i] = NULL; 
    }
}

// Funzione grafica per disegnare la cornice
void GameEngine::setupGameScreen() {
    int startY, startX;
    clear();
    refresh();
    
    p->setWindow(currentMap->getWin());
    UIManager::getCenterCoordinates(Map::getWidth(), Map::getHeight(), startY, startX);
    UIManager::drawBorder(Map::getWidth(), Map::getHeight(), startY, startX);
    
    // Ripulisce la grafica da scie di E e *
    currentMap->renderLevel(); 

    touchwin(currentMap->getWin());
    currentMap->refresh();
    p->display();
    wrefresh(currentMap->getWin());
}

// Gestione dell'input per piazzare la bomba
void GameEngine::handleBombPlacement(int tasto) {
    if (tasto == ' ') {
        for (int i = 0; i < MAX_BOMBE; i++) {
            if (bombeAttive[i] == NULL) {
                bombeAttive[i] = new Bomb(p->getX(), p->getY(), currentMap->getWin(), 2, false);
                break;
            }
        }
    }
}

// Aggiornamento logico e grafico delle bombe
void GameEngine::updateAndDrawBombs() {
    for (int i = 0; i < MAX_BOMBE; i++) {
        if (bombeAttive[i] != NULL) {
            bool hit = bombeAttive[i]->update(*currentMap, *p, arrayNemici, numeroNemici);
            
            if (!bombeAttive[i]->isActive()) {
                delete bombeAttive[i];
                bombeAttive[i] = NULL;
            } else {
                bombeAttive[i]->display();
            }
        }
    }
}

// Gestione procedurale della generazione dei nemici
void GameEngine::generateEnemies() {
    // 1. PULIZIA TOTALE SICURA: Cicliamo su TUTTA la capienza dell'array
    for (int i = 0; i < MAX_NEMICI; i++) {
        if (arrayNemici[i] != NULL) {
            delete arrayNemici[i];
            arrayNemici[i] = NULL;
        }
    }
    
    // 2. CALCOLO NUMERO NEMICI
    numeroNemici = 3 + ((currentMap->GetLvlN()) / 2);
    if (numeroNemici > MAX_NEMICI) {
        numeroNemici = MAX_NEMICI;
    }
    
    // 3. GENERAZIONE
    for (int i = 0; i < numeroNemici; i++) {
        
        Enemy* nuovoNemico = NULL;
        
        nuovoNemico = new RandomEnemy(0, 0, 'E', currentMap->getWin(), Direction::RIGHT, 7); 
        
        // Ora la chiamata è sicura al 100%
        nuovoNemico->spawna_casuale(*currentMap, arrayNemici, i);
        arrayNemici[i] = nuovoNemico;
    }
}

// Stampa le statistiche sopra la finestra di gioco
void GameEngine::drawHUD() {
    int startY, startX;
    UIManager::getCenterCoordinates(Map::getWidth(), Map::getHeight(), startY, startX);
    
    // Ci posizioniamo 2 righe sopra la mappa
    int hudY = startY - 2;
    
    // Stampiamo la stringa formattata
    mvprintw(hudY, startX, " VITE: %d   |   LIVELLO: %d   |   BOMBE: %d ", 
             p->getLife(), 
             currentMap->GetLvlN(), 
             MAX_BOMBE);
             
    refresh(); // Aggiorna lo schermo standard (stdscr) su cui è disegnato l'HUD
}

// Controlla se il giocatore tocca un nemico vivo e ne gestisce il danno
bool GameEngine::checkEnemyCollisions() {
    for (int i = 0; i < numeroNemici; i++) {
        if (arrayNemici[i] != NULL && arrayNemici[i]->isAlive()) {
            
            // Se le coordinate coincidono
            if (p->getX() == arrayNemici[i]->getX() && p->getY() == arrayNemici[i]->getY()) {
                
                // Applica il danno e fa lampeggiare il giocatore
                p->Death(true); 
                
                // Controlla se il giocatore è definitivamente morto
                if (p->getLife() <= 0) {
                    return true; // Ritorna true segnalando il GAME OVER
                }
            }
        }
    }
    return false; // Il giocatore è sopravvissuto al frame attuale
}

// Pulisce le variabili prima di un cambio livello
void GameEngine::resetGameVariables() {
    // Distrugge tutte le bombe in corso per evitare che esplodano nella memoria
    for (int i = 0; i < MAX_BOMBE; i++) {
        if (bombeAttive[i] != NULL) {
            delete bombeAttive[i];
            bombeAttive[i] = NULL;
        }
    }
}

void GameEngine::showGameOverScreen() {
    // Pulisce brutalmente tutto lo schermo
    clear(); 
    
    // Testi da visualizzare
    std::string msg = "G A M E   O V E R";
    std::string subMsg = "Premi un tasto per tornare al Menu...";
    
    // Stampa al centro esatto dello schermo terminale
    mvprintw(yMax / 2 - 1, (xMax - msg.length()) / 2, "%s", msg.c_str());
    mvprintw(yMax / 2 + 1, (xMax - subMsg.length()) / 2, "%s", subMsg.c_str());
    
    refresh();
    
    // Si assicura che getch() si blocchi in attesa di un tasto
    nodelay(stdscr, FALSE); 
    getch(); 
    
    // Ripulisce lo schermo prima di ridare il controllo al MainMenu
    clear(); 
}

// Il Game Loop Principale
void GameEngine::run() {
    while (true) {
        if (!inGame) {
            MainMenu menu(yMax, xMax);
            int menuChoice = menu.run(yMax, xMax);
            
            if (menuChoice == 1) { // L'utente sceglie GIOCA
                currentMap = &manager.AddLevel(1, yMax);
                p->resetPosition();
                p->resetLevelFlags();
                setupGameScreen();
                generateEnemies();
                inGame = true;
                nodelay(stdscr, TRUE); // Imposta la modalità non bloccante per l'input
            } else {
                break; // Esce dal gioco
            }
        } else {
            // Logica in-game
            int tasto = p->getmv(*currentMap);
            handleBombPlacement(tasto);
            
            if (p->getReturnToMenu()) {
                p->erase(*currentMap);
                inGame = false;
                resetGameVariables(); // Pulisce le vecchie bombe
            }
            else {
            
                if (p->ReturnNextLevel()) {
                    p->erase(*currentMap);
                    resetGameVariables(); 
                    currentMap = &manager.nextLevel(yMax);
                    p->resetPosition();
                    p->resetLevelFlags();
                    setupGameScreen();    
                    generateEnemies();    
                }
                
                if (p->ReturnPrevLevel()) {
                    p->erase(*currentMap);
                    resetGameVariables(); 
                    currentMap = &manager.prevLevel();
                    p->resetPosition();
                    p->resetLevelFlags();
                    setupGameScreen();    
                    generateEnemies();    
                }
                
                // Rendering HUD e Mappa
                drawHUD();
                currentMap->refresh();
                p->display();
                
                // Aggiorna e disegna tutti i nemici in vita
                for (int i = 0; i < numeroNemici; i++) {
                    if (arrayNemici[i] != NULL && arrayNemici[i]->isAlive()) {
                        arrayNemici[i]->update(*currentMap, *p); 
                        arrayNemici[i]->display();               
                    }
                }
                
                // --- CONTROLLO COLLISIONI PULITO ---
                bool isGameOver = checkEnemyCollisions();
                if (isGameOver) {
                    // IL GIOCATORE HA PERSO
                    p->erase(*currentMap);
                    inGame = false;
                    
                    // Ripristina l'input bloccante per permettere ai menu di funzionare
                    nodelay(stdscr, FALSE); 
                    
                    showGameOverScreen();
                    resetGameVariables(); // Pulisce le vecchie bombe

                }
                else{

                updateAndDrawBombs();
                wrefresh(currentMap->getWin());
                napms(16); // Rallenta il loop per mantenere circa 60 FPS
            
                // ==========================================================
                // CHECK PROGRESSIONE: NEMICI MORTI E PORTA SEGRETA
                // ==========================================================

                bool tuttiMorti = false;

                // 1. Usa il getter per il numero dei nemici
                for (int i = 0; i < getNumeroNemici(); i++) {
                    Enemy* nemicoAttuale = getNemico(i);
                    if (nemicoAttuale != NULL && !nemicoAttuale->isAlive()) {
                        tuttiMorti = true; // C'è ancora qualcuno da sconfiggere!
                        break; 
                    }
                }

                // 2. Usa i getter per le coordinate della porta
                int pY = currentMap->getPortaY();
                int pX = currentMap->getPortaX();

                int playerY = p->getY(); 
                int playerX = p->getX();
                int valoreCellaPorta = currentMap->GetPos(pY, pX);

                mvprintw(0, 0, "DEBUG | TuttiMorti: %d | PortaNascosta(Y:%d X:%d Valore:%d) | Player(Y:%d X:%d)       ", 
                        tuttiMorti, pY, pX, valoreCellaPorta, playerY, playerX);
                        
                refresh();

                // 3. Fai apparire la porta
                if (tuttiMorti == true && currentMap->GetPos(pY, pX) == 0) {
                    currentMap->setPos(pY, pX, 3);       // Piazza la porta
                    currentMap->RedrawCell(pY, pX);      // Disegnala a schermo
                }
                // ==========================================================
                }
            } // else (Fine logica in-game se non hai premuto ESC)
        } // else(!inGame)
    } // while(true) 
} //run()