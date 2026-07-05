#include "Player.hpp"
#include "Map.hpp"

// Costruttore: inizializza il giocatore con la finestra, posizione iniziale e carattere
Player::Player(WINDOW* win, int y, int x, char c) {
    curwin = win;
    yLoc = y;
    xLoc = x;
    getmaxyx(curwin, yMax, xMax);
    keypad(curwin, true);  // Abilita tasti speciali (frecce)
    character = c;

    NextLevel = false;      // Flag per passare al livello successivo
    PrevLevel = false;      // Flag per tornare al livello precedente
    ReturnToMenu = false;   // Flag per ritornare al menu principale
}

// Muove il giocatore verso l'alto se la cella successiva è libera (0 = spazio vuoto)
void Player::mvup(Map &Mappa) {
    int nextY = yLoc - 1;
    if (nextY < 1) nextY = 1;  // Limita il movimento al bordo

    // Controlla se la cella successiva è traversabile (spazio vuoto)
    if (Mappa.GetPos(nextY, xLoc) == 0) {
        redrawPreviousCell(Mappa);
        yLoc = nextY;
        display();
    }

    // Controlla se il giocatore ha raggiunto una porta
    if (Mappa.GetPos(nextY, xLoc) == 3) NextLevel = true;   // Porta livello successivo (>)
    if (Mappa.GetPos(nextY, xLoc) == 4) PrevLevel = true;   // Porta livello precedente (<)
}

// Muove il giocatore verso il basso se la cella successiva è libera o è una porta
void Player::mvdown(Map &Mappa) {
    int nextY = yLoc + 1;
    if (nextY > yMax - 2) nextY = yMax - 2;  // Limita il movimento al bordo

    // Controlla se la cella successiva è traversabile (spazio vuoto o porta)
    if (Mappa.GetPos(nextY, xLoc) == 0 || Mappa.GetPos(nextY, xLoc) == 3 || Mappa.GetPos(nextY, xLoc) == 4) {
        redrawPreviousCell(Mappa);
        yLoc = nextY;
        display();
    }

    // Controlla se il giocatore ha raggiunto una porta
    if (Mappa.GetPos(nextY, xLoc) == 3) NextLevel = true;   // Porta livello successivo (>)
    if (Mappa.GetPos(nextY, xLoc) == 4) PrevLevel = true;   // Porta livello precedente (<)
}

// Muove il giocatore verso sinistra se la cella successiva è libera o è una porta
void Player::mvleft(Map &Mappa) {
    int nextX = xLoc - 1;
    if (nextX < 1) nextX = 1;  // Limita il movimento al bordo

    // Controlla se la cella successiva è traversabile (spazio vuoto o porta)
    if (Mappa.GetPos(yLoc, nextX) == 0 || Mappa.GetPos(yLoc, nextX) == 3 || Mappa.GetPos(yLoc, nextX) == 4) {
        redrawPreviousCell(Mappa);
        xLoc = nextX;
        display();
    }

    // Controlla se il giocatore ha raggiunto una porta
    if (Mappa.GetPos(yLoc, nextX) == 3) NextLevel = true;   // Porta livello successivo (>)
    if (Mappa.GetPos(yLoc, nextX) == 4) PrevLevel = true;   // Porta livello precedente (<)
}

// Muove il giocatore verso destra se la cella successiva è libera o è una porta
void Player::mvright(Map &Mappa) {
    int nextX = xLoc + 1;
    if (nextX > xMax - 2) nextX = xMax - 2;  // Limita il movimento al bordo

    // Controlla se la cella successiva è traversabile (spazio vuoto o porta)
    if (Mappa.GetPos(yLoc, nextX) == 0 || Mappa.GetPos(yLoc, nextX) == 3 || Mappa.GetPos(yLoc, nextX) == 4) {
        redrawPreviousCell(Mappa);
        xLoc = nextX;
        display();
    }

    // Controlla se il giocatore ha raggiunto una porta
    if (Mappa.GetPos(yLoc, nextX) == 3) NextLevel = true;   // Porta livello successivo (>)
    if (Mappa.GetPos(yLoc, nextX) == 4) PrevLevel = true;   // Porta livello precedente (<)
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
    }
    return choice;
}

// Disegna il giocatore nella sua posizione corrente
void Player::display() {
    mvwaddch(curwin, yLoc, xLoc, character);
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
void Player::resetPosition() {
    yLoc = 1;
    xLoc = 1;
}

// Aggiorna la finestra del giocatore e i suoi limiti massimi
void Player::setWindow(WINDOW* win) {
    this->curwin = win;
    getmaxyx(curwin, yMax, xMax);  // Aggiorna i limiti della finestra
    keypad(curwin, true);          // Abilita i tasti speciali
}

// Cancella il giocatore dalla finestra (lo sostituisce con uno spazio)
void Player::erase() {
    mvwaddch(curwin, yLoc, xLoc, ' ');
}

// Ridisegna il contenuto corretto della cella dopo lo spostamento del giocatore
void Player::redrawPreviousCell(Map &Mappa) {
    Mappa.RedrawCell(yLoc, xLoc);
}


