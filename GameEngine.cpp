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

    timerRunning = false;
    int maxLevelReached = 1;
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
    
    // Pulisce l'intera riga prima di scrivere, per evitare artefatti grafici (glitch visivi)
    move(hudY, 0);
    clrtoeol();
            
    // Stampiamo la stringa formattata con l'aggiunta dello SCORE (formattato a 6 cifre con gli zeri)
    mvprintw(hudY, startX, "SCORE: %07d  | VITE: %d  | LIVELLO: %d", 
             p->getScore(),
             p->getLife(), 
             currentMap->GetLvlN());
             

    // 2. HUD INFERIORE (tempo)
    int bottomY = startY + Map::getHeight() + 1; 
    
    move(bottomY, 0);
    clrtoeol(); 
    
    int tempoRimanente = 2000; // Valore massimo di partenza
    
    if (timerRunning) {
        auto now = std::chrono::steady_clock::now();
        int tempoTrascorso = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        
        // Sottraiamo il tempo trascorso dal valore iniziale
        tempoRimanente -= tempoTrascorso;
        
        // Evitiamo che il tempo vada in negativo
        if (tempoRimanente < 0) {
            tempoRimanente = 0;
            // (Opzionale) Qui in futuro potresti far scattare il Game Over
        }
    }
    
    int timeX = startX + (Map::getWidth() / 2) - 5;
    
    mvprintw(bottomY, timeX, " TIME: %d ", tempoRimanente);
    
    refresh();
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
    
    // Testi da visualizzare usando const char* (C-strings)
    const char* msg = "G A M E   O V E R";
    const char* subMsg = "Premi un tasto per tornare al Menu...";
    
    // Stampa al centro esatto dello schermo terminale usando strlen()
    mvprintw(yMax / 2 - 1, (xMax - strlen(msg)) / 2, "%s", msg);
    mvprintw(yMax / 2 + 1, (xMax - strlen(subMsg)) / 2, "%s", subMsg);
    
    refresh();
    
    // Si assicura che getch() si blocchi in attesa di un tasto
    nodelay(stdscr, FALSE); 

    // Pulisce eventuali input residui nel buffer per evitare che il tasto premuto in precedenza venga catturato
    flushinp();

    getch(); 
    
    // Ripulisce lo schermo prima di ridare il controllo al MainMenu
    clear(); 
}

// Funzione per salvare il punteggio del giocatore in un file di testo
void GameEngine::saveScore() {
    int punteggioFinale = p->getScore();
    
    // Non salviamo i punteggi a zero
    if (punteggioFinale <= 0) return; 

    // Crea un oggetto ofstream in modalità 'app' (append) per non sovrascrivere
    std::ofstream file("leaderboard.txt", std::ios::app);
    
    // Controlla se il file è stato aperto con successo
    if (file.is_open()) {
        file << punteggioFinale << "\n"; // Scrive il numero e va a capo
        file.close();                    // Chiude il flusso
    }
}

// Lettura e stampa della leaderboard dal file leaderboard.txt
void GameEngine::showLeaderboard() {
    clear();
    
    const char* titolo = "--- T O P   1 0   S C O R E S ---";
    mvprintw(2, (xMax - strlen(titolo)) / 2, "%s", titolo);
    
    std::ifstream file("leaderboard.txt");
    
    int rigaY = 5;
    
    if (file.is_open()) {
        int scoreLetto;
        int scores[100]; // Array per salvare fino a 100 punteggi
        int count = 0;
        
        // 1. Leggiamo tutti i punteggi dal file e li mettiamo nell'array
        while (file >> scoreLetto && count < 100) {
            scores[count] = scoreLetto;
            count++;
        }
        file.close();
        
        // 2. Ordiniamo l'array dal più grande al più piccolo (Bubble Sort)
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                if (scores[j] < scores[j + 1]) {
                    // Scambio i valori
                    int temp = scores[j];
                    scores[j] = scores[j + 1];
                    scores[j + 1] = temp;
                }
            }
        }
        
        // 3. Stampiamo i risultati
        if (count > 0) {
            // Vogliamo stampare al massimo i primi 10 risultati
            int maxDaStampare = (count < 10) ? count : 10;
            
            for (int i = 0; i < maxDaStampare; i++) {
                // Formattiamo la stringa con la posizione (es. "1. PUNTEGGIO: 005500")
                mvprintw(rigaY, (xMax - 20) / 2, "%d. PUNTEGGIO: %06d", i + 1, scores[i]);
                rigaY++;
            }
        } else {
            // Il file esiste ma è vuoto
            const char* errore = "Nessun punteggio salvato!";
            mvprintw(rigaY, (xMax - strlen(errore)) / 2, "%s", errore);
        }
        
    } else {
        // Il file non esiste ancora
        const char* errore = "Nessun punteggio salvato!";
        mvprintw(rigaY, (xMax - strlen(errore)) / 2, "%s", errore);
    }
    
    const char* subMsg = "Premi un tasto per tornare al Menu...";
    mvprintw(rigaY + 3, (xMax - strlen(subMsg)) / 2, "%s", subMsg);
    
    refresh();
    
    nodelay(stdscr, FALSE);
    flushinp();
    getch();
    
    clear();
}

// Il Game Loop Principale
void GameEngine::run() {
    while (true) {
        if (!inGame) {
            MainMenu menu(yMax, xMax);
            int menuChoice = menu.run(yMax, xMax);
            
            if (menuChoice == 1) { // L'utente sceglie GIOCA
                maxLevelReached = 1; //resetta il record del livello massimo raggiunto
                p->resetStats(); //resetta le vite e lo score del giocatore
                currentMap = &manager.AddLevel(1, yMax);
                p->resetPosition();
                p->resetLevelFlags();
                setupGameScreen();
                generateEnemies();
                inGame = true;
                // AVVIO DEL TIMER
                startTime = std::chrono::steady_clock::now();
                timerRunning = true;
                nodelay(stdscr, TRUE); // Imposta la modalità non bloccante per l'input
            } else if (menuChoice == 2) { // L'utente sceglie CLASSIFICA
                showLeaderboard();
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

                    int livelloCorrente = currentMap->GetLvlN();
                    
                    // ======================================================
                    // CHECK EXPLOIT: Diamo i punti solo se è la prima volta
                    // ======================================================
                    if (livelloCorrente >= maxLevelReached) {
                        
                        // --- CALCOLO BONUS TEMPO ---
                        auto now = std::chrono::steady_clock::now();
                        int tempoTrascorso = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
                        int tempoRimanente = 2000 - tempoTrascorso;
                        
                        if (tempoRimanente > 0) {
                            p->addScore(tempoRimanente * 10); 
                        }
                        
                        // Aggiorniamo il record: il prossimo livello da battere sarà quello successivo!
                        maxLevelReached = livelloCorrente + 1;
                    }
                    // ======================================================

                    p->erase(*currentMap);
                    resetGameVariables(); 
                    currentMap = &manager.nextLevel(yMax);
                    p->resetPosition();
                    p->resetLevelFlags();
                    setupGameScreen();    
                    generateEnemies(); 
                    
                    // --- RESET DEL TIMER PER IL NUOVO LIVELLO ---
                    startTime = std::chrono::steady_clock::now();
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
                    saveScore(); // Salva il punteggio su file

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