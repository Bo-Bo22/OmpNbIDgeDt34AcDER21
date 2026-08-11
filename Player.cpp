#include "Player.hpp"
#include "Map.hpp"

// Costruttore
Player::Player(WINDOW* win, int y, int x, char c) {
    curwin = win;
    yLoc = y;
    xLoc = x;
    getmaxyx(curwin, yMax, xMax);
    keypad(curwin, true);  
    character = c;
    NextLevel = false;      
    PrevLevel = false;      
    ReturnToMenu = false;   
    
    Life = 3; // Inizializziamo le vite del giocatore
    Score = 0; // Inizializziamo lo score del giocatore

    hitByExplosion = false; // Inizializziamo lo stato di colpito dall'esplosione
}

// Aggiorna la finestra del giocatore e i suoi limiti massimi
void Player::setWindow(WINDOW* win) {
    this->curwin = win;
    getmaxyx(curwin, yMax, xMax);  
    keypad(curwin, true);          
    nodelay(curwin, true);         
    wtimeout(curwin, 0); // FIX: Garantisce in modo assoluto che l'input non blocchi il ciclo
}

// Muove il giocatore verso l'alto se la cella successiva è libera (0 = spazio vuoto)
void Player::mvup(Map &Mappa) {
    int nextY = yLoc - 1;
    if (nextY < 1) nextY = 1;  // Limita il movimento al bordo

    // Salviamo il valore della cella in una variabile per avere un codice più pulito
    int cella = Mappa.GetPos(nextY, xLoc);

    // Controlla se la cella successiva è traversabile
    // (cella == 2 && hasWallPass()) per attraversare i muri distruttibili!
    if (cella == 0 || cella == 3 || cella == 4 || cella == 5 || (cella == 2 && hasWallPass())) { 
        redrawPreviousCell(Mappa);
        yLoc = nextY;
        display();
    }

    // Controlla se il giocatore ha raggiunto una porta
    if (cella == 3) NextLevel = true;   // Porta livello successivo (>)
    if (cella == 4) PrevLevel = true;   // Porta livello precedente (<)
}

// Muove il giocatore verso il basso se la cella successiva è libera o è una porta
void Player::mvdown(Map &Mappa) {
    int nextY = yLoc + 1;
    if (nextY > yMax - 2) nextY = yMax - 2;  // Limita il movimento al bordo

    int cella = Mappa.GetPos(nextY, xLoc);

    // Controlla se la cella successiva è traversabile (spazio vuoto o porta)
    if (cella == 0 || cella == 3 || cella == 4 || cella == 5 || (cella == 2 && hasWallPass())) {
        redrawPreviousCell(Mappa);
        yLoc = nextY;
        display();
    }

    // Controlla se il giocatore ha raggiunto una porta
    if (cella == 3) NextLevel = true;   // Porta livello successivo (>)
    if (cella == 4) PrevLevel = true;   // Porta livello precedente (<)
}

// Muove il giocatore verso sinistra se la cella successiva è libera o è una porta
void Player::mvleft(Map &Mappa) {
    int nextX = xLoc - 1;
    if (nextX < 1) nextX = 1;  // Limita il movimento al bordo

    int cella = Mappa.GetPos(yLoc, nextX);

    // Controlla se la cella successiva è traversabile (spazio vuoto o porta)
    if (cella == 0 || cella == 3 || cella == 4 || cella == 5 || (cella == 2 && hasWallPass())) {
        redrawPreviousCell(Mappa);
        xLoc = nextX;
        display();
    }

    // Controlla se il giocatore ha raggiunto una porta
    if (cella == 3) NextLevel = true;   // Porta livello successivo (>)
    if (cella == 4) PrevLevel = true;   // Porta livello precedente (<)
}

// Muove il giocatore verso destra se la cella successiva è libera o è una porta
void Player::mvright(Map &Mappa) {
    int nextX = xLoc + 1;
    if (nextX > xMax - 2) nextX = xMax - 2;  // Limita il movimento al bordo

    int cella = Mappa.GetPos(yLoc, nextX);

    // Controlla se la cella successiva è traversabile (spazio vuoto o porta)
    if (cella == 0 || cella == 3 || cella == 4 || cella == 5 || (cella == 2 && hasWallPass())) {
        redrawPreviousCell(Mappa);
        xLoc = nextX;
        display();
    }

    // Controlla se il giocatore ha raggiunto una porta
    if (cella == 3) NextLevel = true;   // Porta livello successivo (>)
    if (cella == 4) PrevLevel = true;   // Porta livello precedente (<)
}

// Gestisce l'input da tastiera e chiama il metodo di movimento appropriato
int Player::getmv(Map &Mappa) {
    int choice = wgetch(curwin);
    
    switch (choice) {
        case KEY_UP:
            mvup(Mappa);
            break;
        case KEY_DOWN:
            mvdown(Mappa);
            break;
        case KEY_LEFT:
            mvleft(Mappa);
            break;
        case KEY_RIGHT:
            mvright(Mappa);
            break;
        case 'q':
        case 'Q':
            ReturnToMenu = true;  // Ritorna al menu principale
            break;
    }
    return choice;
}

// Disegna il giocatore nella sua posizione corrente e applica il colore e l'attributo BOLD
void Player::display() {
    // Attiva il Colore 1 (Ciano)
    wattron(curwin, COLOR_PAIR(4)); 
    
    // Stampa il giocatore (es. la lettera 'P' o il carattere che usi tu)
    mvwaddch(curwin, yLoc, xLoc, '@'); 
    
    // Spegne il colore, altrimenti colorerà tutto il resto dello schermo!
    wattroff(curwin, COLOR_PAIR(4));
}

// Ciclo di gioco: visualizza e aspetta input fino a 'x' (non usato nel progetto attuale)
void Player::movement(WINDOW* PlayWin, Map &Mappa) {
    do {
        display();
        wrefresh(PlayWin);
    } while (getmv(Mappa) != 'x');  // Ferma il gioco quando si preme 'x'
}

// Ritorna true se il giocatore ha raggiunto il livello successivo
bool Player::ReturnNextLevel() {
    return NextLevel;
}

// Ritorna true se il giocatore ha raggiunto il livello precedente
bool Player::ReturnPrevLevel() {
    return PrevLevel;
}

// Ritorna true se il giocatore vuole tornare al menu principale
bool Player::getReturnToMenu() const {
    return ReturnToMenu;
}

// Resetta i flag dei livelli quando si cambia mappa
void Player::resetLevelFlags() {
    NextLevel = false;
    PrevLevel = false;
    ReturnToMenu = false;
}

// Ritorna il giocatore alla posizione iniziale (1, 1)
void Player::resetPosition(){
    yLoc = 1;
    xLoc = 1;
}

// Morte con animazione (Lampeggio temporizzato con <chrono>)
int Player::Death(bool lampeggiante) {
    
    // Applichiamo il danno
    if (Life > 0) {
        Life--;
    }
    
    // Resettiamo la posizione del giocatore sulla mappa
    resetPosition();
    
    // Gestione del lampeggio visivo
    if (lampeggiante) {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point last_tick = start;
        
        int freq = 400; // FIX: Dichiarata correttamente la variabile intera
        bool tick = false; 

        while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < 2000) {

            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

            // Controllo del tempo per far cambiare stato al flag (Tick)
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick).count() >= freq) {
                tick = !tick;
                last_tick = now;
                
                // Effetto accelerazione
                if (freq >= 50) {
                    freq = freq / 2;
                }
            }

            // Se tick è true nascondiamo il personaggio, altrimenti lo mostriamo
            if (tick) {
                mvwaddch(curwin, yLoc, xLoc, ' ');
            } else {
                display(); 
            }
            
            wrefresh(curwin);
            napms(16); 
        }
    }

    display();
    return Life;
}

// Cancella il giocatore dalla finestra (lo sostituisce con uno spazio)
void Player::erase(Map &Mappa) {
    if((Mappa.GetMapChar(yLoc, xLoc)) == ' ') {
        mvwaddch(curwin, yLoc, xLoc, ' ');  // Cancella il giocatore se la cella è vuota
    } else {
        redrawPreviousCell(Mappa);  // Ridisegna la cella precedente se non è vuota
    }
}

// Ridisegna il contenuto corretto della cella dopo lo spostamento del giocatore
void Player::redrawPreviousCell(Map &Mappa) {
    Mappa.RedrawCell(yLoc, xLoc);
}

int Player::getX(){
    return xLoc;
}

int Player::getY(){
    return yLoc;
}

int Player::LifeUp(){
    if (Life>0) Life++;
    return Life;
}

int Player::getLife(){
     return Life; }

void Player::addScore(int points) { 
    Score += points; 
}

int Player::getScore() { 
    return Score; 
}

void Player::resetStats() {
    Score = 0;
    Life = 3;
    maxBombs = 3;
    bombRange = 1;
    wallPass = false;
}

void Player::setHitByExplosion(bool state) {
    hitByExplosion = state; 
}

bool Player::getHitByExplosion() {
    return hitByExplosion;
}

// Metodi per applicare i potenziamenti
void Player::addLife() { 
    Life++; 
}
void Player::addMaxBombs() { 
    maxBombs++; 
}
void Player::addBombRange() { 
    bombRange++; 
}
void Player::setWallPass(bool status) { 
    wallPass = status; 
}

// Getters per leggere i valori nel GameEngine
int Player::getMaxBombs() { 
    return maxBombs; 
}

int Player::getBombRange() { 
    return bombRange; 
}

bool Player::hasWallPass() {
    return wallPass; 
}

void Player::resetPowerups() {
    maxBombs = 3;
    bombRange = 1;
    wallPass = false;
}