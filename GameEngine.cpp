#include "GameEngine.hpp"

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
    for (int i = 0; i < 6; i++) {
        bombeAttive[i] = NULL;
    }
    for (int i = 0; i < 10; i++) {
        arrayNemici[i] = NULL; 
    }

    for (int i = 0; i < 11; i++) {
        livelloVisitato[i] = false;
        numNemiciPerLivello[i] = 0;
        for (int j = 0; j < 10; j++) {
            nemiciPerLivello[i][j] = NULL;
        }
    }


    timerRunning = false;
    maxLevelReached = 1;
}

// Funzione per ottenere il numero di nemici attivi
int GameEngine::getNumeroNemici() {
    return numeroNemici; 
}

// Funzione grafica per disegnare la cornice
void GameEngine::setupGameScreen() {
    int startY, startX;
    clear();
    refresh();
    
    p->setWindow(currentMap->getWin());
    ui.getCenterCoordinates(currentMap->getWidth(), currentMap->getHeight(), startY, startX);
    ui.drawBorder(currentMap->getWidth(), currentMap->getHeight(), startY, startX);
    
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
        
        // ==========================================================
        // CONTROLLO ANTI-EXPLOIT: Niente bombe dentro i muri!
        // ==========================================================
        // Controlliamo il valore della mappa alle coordinate attuali del player
        if (currentMap->GetPos(p->getY(), p->getX()) == 2) {
            return; // Il giocatore è in un muro (grazie al wallPass). Interrompi subito la funzione!
        }
        
        // Se siamo qui, significa che il giocatore è su un terreno valido.
        // Procediamo col normale conteggio e piazzamento.
        int bombePiazzate = 0;
        for (int i = 0; i < 6; i++) {
            if (bombeAttive[i] != NULL) {
                bombePiazzate++;
            }
        }
        
        if (bombePiazzate < p->getMaxBombs()) {
            for (int i = 0; i < 6; i++) {
                if (bombeAttive[i] == NULL) {
                    // Crea la bomba con il raggio potenziato
                    bombeAttive[i] = new Bomb(p->getX(), p->getY(), currentMap->getWin(), p->getBombRange());
                    break;
                }
            }
        }
    }
}

// Aggiornamento logico e grafico delle bombe
void GameEngine::updateAndDrawBombs() {
    for (int i = 0; i < 6; i++) {
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
    for (int i = 0; i < 10; i++) {
        if (arrayNemici[i] != NULL) {
            delete arrayNemici[i];
            arrayNemici[i] = NULL;
        }
    }
    
    // 2. CALCOLO NUMERO NEMICI
    numeroNemici = 3 + ((currentMap->GetLvlN()) / 2);
    if (numeroNemici > 10) {
        numeroNemici = 10; // Limite massimo di nemici
    }
    
    // 3. GENERAZIONE
    for (int i = 0; i < numeroNemici; i++) {
        
        Enemy* nuovoNemico = NULL;
        
        if (i % 4 == 0) {
            nuovoNemico = new BasicEnemy(0, 0, 'B', currentMap->getWin(), Direction::RIGHT, 250, 7); 
        } else if (i % 4 == 1) {
            nuovoNemico = new EasyChaserEnemy(0, 0, 'E', currentMap->getWin(), Direction::DOWN, 350, 7); 
        } else if (i % 4 == 2) {
            nuovoNemico = new ChaserEnemy(0, 0, 'C', currentMap->getWin(), Direction::RIGHT, 350, 7);
        } else {
            nuovoNemico = new RandomEnemy(0, 0, 'R', currentMap->getWin(), Direction::LEFT, 300, 7);
        }
        
        // Ora la chiamata è sicura al 100%
        nuovoNemico->spawna_casuale(*currentMap, arrayNemici, i);
        arrayNemici[i] = nuovoNemico;
    }
}

// Stampa le statistiche sopra la finestra di gioco
void GameEngine::drawHUD() {
    int startY, startX;
    ui.getCenterCoordinates(currentMap->getWidth(), currentMap->getHeight(), startY, startX);
    
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
    int bottomY = startY + currentMap->getHeight() + 1; 
    
    move(bottomY, 0);
    clrtoeol(); 
    
    int tempoRimanente = 2000; // Valore massimo di partenza
    
    if (timerRunning) {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        int tempoTrascorso = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        
        // Sottraiamo il tempo trascorso dal valore iniziale
        tempoRimanente -= tempoTrascorso;
        
        // Evitiamo che il tempo vada in negativo
        if (tempoRimanente < 0) {
            tempoRimanente = 0;
            // (Opzionale) Qui in futuro potresti far scattare il Game Over
        }
    }
    
    int timeX = startX + (currentMap->getWidth() / 2) - 5;
    
    mvprintw(bottomY, timeX, " TIME: %d ", tempoRimanente);
    
    refresh();
}

// Controlla se il giocatore tocca un nemico vivo e ne gestisce il danno
bool GameEngine::checkEnemyCollisions() {
    for (int i = 0; i < numeroNemici; i++) {
        if (arrayNemici[i] != NULL && arrayNemici[i]->isAlive()) {
            
            // Se le coordinate coincidono, il giocatore è stato toccato
            if (p->getX() == arrayNemici[i]->getX() && p->getY() == arrayNemici[i]->getY()) {
                return true; // Segnala semplicemente il "COLPO SUBITO"
            }
        }
    }
    return false; // Nessuna collisione in questo frame
}

// Controlla se il giocatore è stato colpito da un'esplosione di bomba
bool GameEngine::checkBombCollisions() {
    if (p->getHitByExplosion()) {
        p->setHitByExplosion(false); 
        return true; 
    }
    return false;
}

// Verifica se la posizione del giocatore coincide con quella di un oggetto speciale (ID 5),
// ne applica casualmente l'effetto e avvia l'animazione grafica di raccolta.
void GameEngine::checkItemCollisions() {
    int pY = p->getY();
    int pX = p->getX();
    
    // Se il giocatore si trova su una cella con il valore 5 ($)
    if (currentMap->GetPos(pY, pX) == 5) {
        
        // 1. Sceglie un potenziamento casuale e lo applica
        int tipoCasuale = rand() % 5 + 1; // Genera un numero casuale tra 1 e 5
        Item powerup(tipoCasuale);
        powerup.applyEffect(p);
        
        // 2. Rimuove fisicamente il $ dalla mappa
        currentMap->setPos(pY, pX, 0);
        currentMap->RedrawCell(pY, pX);
        
        // ======================================================
        // 3. ANIMAZIONE DI RACCOLTA (Lampeggio del Player)
        // ======================================================
        for (int b = 0; b < 3; b++) { // Ripete il lampeggio 3 volte
            
            // Cancella il player e aggiorna lo schermo
            p->erase(*currentMap);
            wrefresh(currentMap->getWin());
            napms(40); // Pausa di 40 millisecondi
            
            // Ridisegna il player e aggiorna lo schermo
            p->display();
            wrefresh(currentMap->getWin());
            napms(40); // Pausa di 40 millisecondi
        }
        // ======================================================
    }
}

// Pulisce le variabili prima di un cambio livello
void GameEngine::resetGameVariables() {
    // Distrugge tutte le bombe in corso per evitare che esplodano nella memoria
    for (int i = 0; i < 6; i++) {
        if (bombeAttive[i] != NULL) {
            delete bombeAttive[i];
            bombeAttive[i] = NULL;
        }
    }
}

// Funzione per mostrare la schermata di Game Over e attendere l'input dell'utente
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

// Funzione per mostrare la schermata di Vittoria dopo 10 livelli
void GameEngine::showVictoryScreen() {
    clear();

    const char* msg = "V I T T O R I A !";
    const char* subMsg1 = "HAI COMPLETATO CON SUCCESSO TUTTI I 10 LIVELLI!";
    const char* subMsg2 = "Premi un tasto per registrare il tuo punteggio...";

    // Centra le scritte sulla finestra del terminale
    mvprintw(yMax / 2 - 3, (xMax - strlen(msg)) / 2, "%s", msg);
    mvprintw(yMax / 2 - 1, (xMax - strlen(subMsg1)) / 2, "%s", subMsg1);
    mvprintw(yMax / 2 + 1, (xMax - 30) / 2, "PUNTEGGIO FINALE: %07d", p->getScore());
    mvprintw(yMax / 2 + 3, (xMax - strlen(subMsg2)) / 2, "%s", subMsg2);

    refresh();

    // Attesa bloccante del giocatore
    nodelay(stdscr, FALSE);
    flushinp();
    getch();
    clear();
}

// Richiede il nome all'utente in modo sicuro, normalizza gli spazi e salva
// la coppia nome-punteggio in modalita' append (std::ios::app) sul file leaderboard.txt.
void GameEngine::saveScore() {
    int punteggioFinale = p->getScore();
    // Non registra punteggi nulli o negativi
    if (punteggioFinale <= 0) return;

    clear();
    // Disattiva la modalita' non-bloccante: l'I/O deve attendere la digitazione dell'utente
    nodelay(stdscr, FALSE);
    // Abilita l'eco a video dei caratteri digitati e rende visibile il cursore
    echo();
    curs_set(1);

    // Buffer: 15 caratteri alfanumerici massimi + 1 cella riservata al terminatore '\0'
    char nome[16] = "";
    const char* subPrompt = "Inserisci il tuo nome (max 15 caratteri): ";
    
    // Centratura dei messaggi a video tramite le dimensioni del terminale
    mvprintw(yMax / 2 - 2, (xMax - 30) / 2, "PUNTEGGIO FINALE: %07d", punteggioFinale);
    mvprintw(yMax / 2, (xMax - strlen(subPrompt)) / 2, "%s", subPrompt);
    refresh();

    // getnstr() tronca la lettura a 15 caratteri evitando scritture fuori dai limiti dello stack
    getnstr(nome, 15);

    // Fallback: se l'utente preme invio a vuoto, assegna una stringa di default
    if (strlen(nome) == 0) {
        strncpy(nome, "Giocatore", 15);
        nome[15] = '\0'; // Garanzia esplicita di terminazione corretta
    } else {
        // Sanitizzazione: converte gli spazi in '_' per non spezzare il parsing sequenziale con '>>'
        for (int i = 0; nome[i] != '\0'; i++) {
            if (nome[i] == ' ') {
                nome[i] = '_';
            }
        }
    }

    // Ripristina l'input nascosto e disattiva il cursore grafico
    noecho();
    curs_set(0);

    // Scrittura in modalita' append (std::ios::app) per aggiungere il record in coda senza sovrascritture
    std::ofstream file("leaderboard.txt", std::ios::app);
    if (file.is_open()) {
        file << nome << " " << punteggioFinale << "\n";
        file.close();
    }
    clear();
}

// Struttura C pura per incapsulare ciascun record senza ricorrere a std::string
struct RecordClassifica {
    char nome[16]; // Buffer allineato esattamente a 15 caratteri utili + '\0'
    int score;     // Punteggio numerico associato
};

// Acquisisce i record persistenti da file, ordina i punteggi in memoria tramite
// algoritmo di Bubble Sort decrescente e stampa a video la classifica formattata.
void GameEngine::showLeaderboard() {
    clear();
    // Modalita' bloccante per la selezione dei record e la schermata di attesa
    nodelay(stdscr, FALSE);
    
    std::ifstream file("leaderboard.txt");
    RecordClassifica scores[100]; // Array limitato a un massimo di 100 salvataggi
    int count = 0;

    if (file.is_open()) {
        // Lettura formattata a token: estrae la coppia nome-punteggio separata da spazio
        while (count < 100 && (file >> scores[count].nome >> scores[count].score)) {
            count++;
        }
        file.close();

        // Ordinamento decrescente in memoria tramite Bubble Sort
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                if (scores[j].score < scores[j + 1].score) {
                    // Scambio per valore dei due record adiacenti
                    RecordClassifica temp = scores[j];
                    scores[j] = scores[j + 1];
                    scores[j + 1] = temp;
                }
            }
        }
    }

    // Gestione del file assente o privo di punteggi registrati
    if (count == 0) {
        const char* errore = "Nessun punteggio salvato!";
        mvprintw(yMax / 2, (xMax - strlen(errore)) / 2, "%s", errore);
        const char* subMsg = "Premi un tasto per tornare al Menu...";
        mvprintw(yMax / 2 + 2, (xMax - strlen(subMsg)) / 2, "%s", subMsg);
        refresh();
        flushinp();
        getch();
        clear();
        return;
    }

    // Acquisizione parametrica del numero di record desiderati dall'utente
    echo();
    curs_set(1);
    const char* richPrompt = "Quanti record vuoi visualizzare? (1-10): ";
    mvprintw(yMax / 2 - 1, (xMax - strlen(richPrompt)) / 2, "%s", richPrompt);
    refresh();

    char bufferN[8] = "";
    getnstr(bufferN, 7); // Lettura sicura del valore numerico sotto forma di stringa
    int nRichiesti = atoi(bufferN);

    // Validazione: fallback a 5 elementi se il dato e' errato, clamp al numero di record reali
    if (nRichiesti <= 0) nRichiesti = 5;
    if (nRichiesti > count) nRichiesti = count;

    noecho();
    curs_set(0);
    clear();

    const char* titolo = "--- C L A S S I F I C A ---";
    mvprintw(2, (xMax - strlen(titolo)) / 2, "%s", titolo);

    int rigaY = 5;
    for (int i = 0; i < nRichiesti; i++) {
        // Ripristino visivo: riconverte i caratteri '_' in spazi per la visualizzazione all'utente
        for (int c = 0; scores[i].nome[c] != '\0'; c++) {
            if (scores[i].nome[c] == '_') {
                scores[i].nome[c] = ' ';
            }
        }

        // Stampa tabellare: posizione numerica, nome allineato a sinistra su 15 colonne e score a 6 cifre
        mvprintw(rigaY, (xMax - 32) / 2, "%d. %-15s %06d", i + 1, scores[i].nome, scores[i].score);
        rigaY++;
    }

    const char* subMsg = "Premi un tasto per tornare al Menu...";
    mvprintw(rigaY + 2, (xMax - strlen(subMsg)) / 2, "%s", subMsg);

    refresh();
    // Svuota i tasti residui dal buffer prima del rientro al menu
    flushinp();
    getch();
    clear();
}

// Invece di restituire tutto l'array, creiamo un metodo che restituisce il singolo nemico
Enemy* GameEngine::getNemico(int indice) { 
    if (indice >= 0 && indice < numeroNemici) {
        return arrayNemici[indice]; 
    }
    return NULL;
}


// ============================================================================
// CICLO PRINCIPALE DI GIOCO (GAME LOOP)
// ============================================================================
void GameEngine::run() {
    while (true) {
        // GESTIONE DELLO STATO FUORI DALLA PARTITA (MENU PRINCIPALE)
        if (!inGame) {
            // Visualizza il menu interattivo e gestisce la selezione dell'utente
            MainMenu menu(yMax, xMax);
            int menuChoice = menu.run(yMax, xMax);
            
            if (menuChoice == 1) { 
                // Avvio di una nuova partita: reset completo di statistiche e punteggi
                maxLevelReached = 1;
                p->resetStats();
                
                // Reset della cronologia delle stanze visitate
                for (int i = 0; i < 11; i++) {
                    livelloVisitato[i] = false;
                }
                
                // Creazione del primo livello nella lista dinamica
                currentMap = &manager.AddLevel(1, yMax);
                p->resetPosition();
                p->resetLevelFlags();
                setupGameScreen();
                generateEnemies();
                
                livelloVisitato[1] = true;
                inGame = true;
                startTime = std::chrono::steady_clock::now();
                timerRunning = true;
                
                // Modalita' non-bloccante: la getch non arresta l'avanzamento dei frame
                nodelay(stdscr, TRUE); 
            } else if (menuChoice == 2) { 
                // Consultazione della classifica persistente salvata su file
                showLeaderboard();
            } else {
                // Selezione "ESCI" o tasto di uscita: arresto definitivo del ciclo
                break; 
            }
        } else {
            // ========================================================================
            // FASE 1: ACQUISIZIONE INPUT E AZIONI ATOMICHE
            // ========================================================================
            // Lettura asincrona della direzione di movimento o comandi speciali
            int tasto = p->getmv(*currentMap);
            handleBombPlacement(tasto);
            
            // Gestione dell'interruzione anticipata con rientro al menu
            if (p->getReturnToMenu()) {
                p->erase(*currentMap);
                inGame = false;
                resetGameVariables();
            } else {
                // ====================================================================
                // FASE 2: TRANSIZIONE AL LIVELLO SUCCESSIVO
                // ====================================================================
                if (p->ReturnNextLevel()) {
                    int livelloCorrente = currentMap->GetLvlN();
                    
                    // Assegnazione del bonus tempo solo al primo completamento del livello
                    if (livelloCorrente >= maxLevelReached) {
                        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                        int tempoTrascorso = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
                        int tempoRimanente = 2000 - tempoTrascorso;
                        if (tempoRimanente > 0) p->addScore(tempoRimanente * 10); 
                        maxLevelReached = livelloCorrente + 1;
                    }

                    // Condizione di vittoria: completamento del decimo livello
                    if (livelloCorrente >= 10) {
                        p->erase(*currentMap);
                        resetGameVariables();
                        showVictoryScreen();
                        saveScore();
                        inGame = false;
                        nodelay(stdscr, FALSE);
                    }

                    else{

                        // Salvataggio dello stato dei nemici per la persistenza della stanza
                        numNemiciPerLivello[livelloCorrente] = numeroNemici;
                        for (int i = 0; i < numeroNemici; i++) {
                            nemiciPerLivello[livelloCorrente][i] = arrayNemici[i];
                        }

                        p->erase(*currentMap);
                        resetGameVariables(); 
                        // Avanzamento al nodo successivo della lista bidirezionale dei livelli
                        currentMap = &manager.nextLevel(yMax);
                        p->resetPosition();
                        p->resetLevelFlags();
                        p->resetPowerups();
                        setupGameScreen();    
                        
                        // Ripristino nemici se la stanza e' gia' stata esplorata, altrimenti generazione
                        int nuovoLvl = currentMap->GetLvlN();
                        if (livelloVisitato[nuovoLvl]) {
                            numeroNemici = numNemiciPerLivello[nuovoLvl];
                            for (int i = 0; i < numeroNemici; i++) {
                                arrayNemici[i] = nemiciPerLivello[nuovoLvl][i];
                            }
                        } else {
                            generateEnemies();
                            livelloVisitato[nuovoLvl] = true;
                        }
                        
                        startTime = std::chrono::steady_clock::now();
                    }
                }
                
                // ====================================================================
                // FASE 3: RITORNO AL LIVELLO PRECEDENTE (BACKTRACKING)
                // ====================================================================
                if (p->ReturnPrevLevel()) {
                    int livelloCorrente = currentMap->GetLvlN();
                    
                    // Salvataggio dei nemici residui della stanza che si sta abbandonando
                    numNemiciPerLivello[livelloCorrente] = numeroNemici;
                    for (int i = 0; i < numeroNemici; i++) {
                        nemiciPerLivello[livelloCorrente][i] = arrayNemici[i];
                    }

                    p->erase(*currentMap);
                    resetGameVariables(); 
                    // Retrocessione nella lista concatenata dei livelli
                    currentMap = &manager.prevLevel();
                    p->resetPosition();
                    p->resetLevelFlags();
                    p->resetPowerups();
                    setupGameScreen();    
                    
                    // Ripristino fedele dello stato della stanza precedente
                    int vecchioLvl = currentMap->GetLvlN();
                    if (livelloVisitato[vecchioLvl]) {
                        numeroNemici = numNemiciPerLivello[vecchioLvl];
                        for (int i = 0; i < numeroNemici; i++) {
                            arrayNemici[i] = nemiciPerLivello[vecchioLvl][i];
                        }
                    }
                }
                
                // ====================================================================
                // FASE 4: RENDERING SCENARIO E GIOCATORE
                // ====================================================================
                drawHUD();
                currentMap->refresh();

                // Disegno condizionato: previene glitch grafici se il player e' colpito
                if (!p->getHitByExplosion()) {
                    p->display();
                }
                
                // ====================================================================
                // FASE 5: AGGIORNAMENTO LOGICO E RENDERING DEI NEMICI
                // ====================================================================
                for (int i = 0; i < numeroNemici; i++) {
                    if (arrayNemici[i] != NULL && arrayNemici[i]->isAlive()) {
                        char sym = arrayNemici[i]->getSymbol();

                        // Polimorfismo esplicito basato sul simbolo dell'entita'
                        if (sym == 'C') {
                            ((ChaserEnemy*)arrayNemici[i])->update(*currentMap, *p);
                        } else if (sym == 'R') {
                            ((RandomEnemy*)arrayNemici[i])->update(*currentMap, *p);
                        } else if (sym == 'E') {
                            ((EasyChaserEnemy*)arrayNemici[i])->update(*currentMap, *p);
                        } else if (sym == 'B') {
                            ((BasicEnemy*)arrayNemici[i])->update(*currentMap, *p);
                        }

                        arrayNemici[i]->display();               
                    }
                }

                // Controllo e raccolta di power-up nascosti sotto i muri distrutti
                checkItemCollisions();
                
                // ====================================================================
                // FASE 6: RISOLUZIONE DELLE COLLISIONI E MORTE DEL PLAYER
                // ====================================================================
                bool colpoDaBomba = p->getHitByExplosion();
                bool isHit = checkEnemyCollisions() || checkBombCollisions();

                if (isHit) {
                    int morteY = p->getY();
                    int morteX = p->getX();

                    // Disegna l'effetto visivo del colpo subito (fiamma o nemico)
                    if (colpoDaBomba) {
                        mvwaddch(currentMap->getWin(), morteY, morteX, '*');
                        wrefresh(currentMap->getWin());
                    } else {
                        for (int i = 0; i < numeroNemici; i++) {
                            if (arrayNemici[i] != NULL && arrayNemici[i]->isAlive()) {
                                arrayNemici[i]->display();
                            }
                        }
                        wrefresh(currentMap->getWin());
                    }

                    // Esegue l'animazione di morte temporizzata e decrementa le vite
                    p->Death(true); 

                    if (p->getLife() <= 0) {
                        saveScore(); 
                        inGame = false;
                        nodelay(stdscr, FALSE); 
                        showGameOverScreen();
                    } else {
                        // Ripristino del giocatore all'angolo di spawn con reset dei bonus
                        p->resetPosition();
                        p->resetPowerups();
                        setupGameScreen();

                        for (int i = 0; i < numeroNemici; i++) {
                            if (arrayNemici[i] != NULL && arrayNemici[i]->isAlive()) {
                                arrayNemici[i]->display();
                            }
                        }
                        wrefresh(currentMap->getWin());
                    }
                    
                    resetGameVariables(); 
                    // Bonifica del buffer di input: scarta comandi premuti durante l'animazione
                    flushinp(); 
                } else {
                    // ================================================================
                    // FASE 7: CADENZA FRAME-RATE E APERTURA PORTA DI LIVELLO
                    // ================================================================
                    updateAndDrawBombs();
                    wrefresh(currentMap->getWin());
                    
                    // Frame pacing: pausa calibrata a circa 60 FPS
                    napms(16); 

                    // Verifica se tutti i nemici presenti sono stati eliminati
                    bool tuttiMorti = true;
                    for (int i = 0; i < getNumeroNemici(); i++) {
                        Enemy* nemicoAttuale = getNemico(i);
                        if (nemicoAttuale != NULL && nemicoAttuale->isAlive()) {
                            tuttiMorti = false;
                            break; 
                        }
                    }

                    // Sblocco della porta del livello: compare solo se il corridoio e' libero
                    int pY = currentMap->getPortaY();
                    int pX = currentMap->getPortaX();
                    if (tuttiMorti && currentMap->GetPos(pY, pX) == 0) {
                        currentMap->setPos(pY, pX, 3);
                        wattron(currentMap->getWin(), COLOR_PAIR(3));
                        currentMap->RedrawCell(pY, pX);      
                        wattroff(currentMap->getWin(), COLOR_PAIR(3));
                    }
                }
            }
        }
    }
}
