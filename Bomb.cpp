#include "Bomb.hpp"

// Aggiorna lo stato temporale della bomba (fase innesco, fiammata, distruzione blocchi).
// Ritorna true se un'entita' e' stata colpita durante questo ciclo di tick.
bool Bomb::update(Map &Mappa, Player &Pl, Enemy** En, int n_nemici) {

    std::chrono::steady_clock::time_point now =  std::chrono::steady_clock::now();
    int blink_time = 500; // millisecondi
    int exploding_time = 2; // secondi

    bool isHit = false;

    // =======================================================
    // FASE 1: LA BOMBA È A TERRA E LAMPEGGIA (NON ESPLOSA)
    // =======================================================
    if (!exploding){
        // Controlla se è passato abbastanza tempo per innescare l'esplosione
        if (std::chrono::duration_cast<std::chrono::seconds>(now - explode_time).count() >= exploding_time){
            exploding = true;
            explode_time = now;
        }
        // Fa lampeggiare l'icona della bomba (animazione)
        else if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time_point).count() > blink_time){
            last_time_point = now;
            visible = !visible;
        }
    
        // Disegna la bomba lampeggiante (rossa se hai impostato il COLOR_PAIR)
        if (visible){
            wattron(curwin, COLOR_PAIR(2));
            mvwaddch(curwin, yLoc, xLoc, 'o');
            wattroff(curwin,COLOR_PAIR(2));
        } else {
            mvwaddch(curwin, yLoc, xLoc, 'o');
        }

        return false;
    }

    // =======================================================
    // FASE 2: IL MOMENTO DELL'ESPLOSIONE E I DANNI
    // =======================================================
    else {

        // Vettori per espandere le fiamme: Su, Giù, Sinistra, Destra
        int dy[] = {-1, 1, 0, 0};
        int dx[] = {0, 0, -1, 1};

        int elapsed = 1; // Tempo per cui la fiamma rimane a schermo prima di sparire

        // Disegna il centro dell'esplosione
        mvwaddch(curwin, yLoc, xLoc, '*');

        // Controlla se il giocatore è fermo proprio sopra la bomba
        if (Pl.getX() == xLoc && Pl.getY()== yLoc){
            Pl.setHitByExplosion(true); // Segnala che il giocatore è stato colpito
        }

        // Controlla se ci sono nemici proprio sopra la bomba
        for (int j = 0; j < n_nemici; j++){
            if(En[j]->getX() == xLoc && En[j]->getY() == yLoc){
                En[j]->kill(Mappa);
            }
        }

        // Calcola l'espansione dei raggi di fuoco
        for (int i = 0; i <= est_bombe; i++){
            for (int k = 0; k < 4; k++) {

                int tY = yLoc + dy[k] * (i + 1);
                int tX = xLoc + dx[k] * (i + 1);

                // Se in questa direzione l'onda d'urto è stata fermata, passa oltre
                if (direzione[k] == true){ 

                // Blocca la fiamma se incontra un muro indistruttibile (1) 
                // o un muro distruttibile (2)
                if (Mappa.GetPos(tY, tX) == 1 || (Mappa.GetPos(tY,tX) == 2)) { 
                    direzione[k] = false;
                }

                // Disegna la fiammata se siamo entro i bordi e non c'è un muro indistruttibile
                if (tY >= 1 && tY <= YMax-2 && tX >= 1 && tX <= XMax-2 && Mappa.GetPos(tY, tX) != 1){
                    mvwaddch(curwin, tY, tX, '*');
                }

                // Danni della fiamma sul Giocatore (con controllo area di sicurezza iniziale)
                if (Pl.getX() == tX && Pl.getY() == tY) {
                    Pl.setHitByExplosion(true);
                }

                    // Danni della fiamma sui Nemici
                    for (int j = 0; j < n_nemici; j++){
                        if(En[j]->getX() == tX && En[j]->getY() == tY){
                            // Controlliamo che sia vivo per non fare punti infiniti!
                            if (En[j]->isAlive()) { 
                                En[j]->kill(Mappa);
                                Pl.addScore(500); // <-- 500 PUNTI PER IL NEMICO
                            }
                        }
                    }
                }
            }
        }

        // =======================================================
        // FASE 3: RIMOZIONE MURI E SPAWN DEGLI OGGETTI
        // =======================================================
        
        // Se la fiamma è rimasta a schermo abbastanza tempo, ripuliamo
        if (std::chrono::duration_cast<std::chrono::seconds>(now - explode_time).count() > elapsed){

             for (int i = 0; i <= est_bombe; i++){
                for (int k = 0; k < 4; k++) {

                    int tY = yLoc + dy[k] * (i + 1);
                    int tX = xLoc + dx[k] * (i + 1);
                
                    // Se in questa coordinata era stata disegnata una fiamma '*'
                    if ((mvwinch(Mappa.getWin(), tY, tX ) & A_CHARTEXT) == '*'){

                        // ----------------------------------------------------
                        // IL NUOVO SISTEMA DI DISTRUZIONE MURI
                        // ----------------------------------------------------
                        // Se la cella è un muro distruttibile (2)
                        if (Mappa.GetPos(tY, tX) == 2) {
                            
                            // Chiediamo alla mappa cosa nascondeva sotto!
                            // Se non c'era niente, 'sorpresa' sarà 0.
                            // Se c'era un powerup, 'sorpresa' sarà il suo ID (es. 5, 6...).
                            int sorpresa = Mappa.getOggettoNascosto(tY, tX);
                            
                            // Trasformiamo il muro esattamente in quello che nascondeva
                            Mappa.setPos(tY, tX, sorpresa);

                            Pl.addScore(50); // Aggiungiamo 50 punti per ogni muro distrutto
                        }
                        // ----------------------------------------------------
                        
                    }

                    // Ridisegna la cella (che ora conterrà il vuoto (0) o l'oggetto (5, 6..))
                    Mappa.renderPos(tY, tX);
                }
            }

            // Pulisci il centro dell'esplosione, disattiva la bomba e salva il tempo
            Mappa.renderPos(yLoc, xLoc);
            active = false;
            last_time_point = now;
        }
        
        return isHit;
    }
}

// Segnala se l'ordigno si trova nello stato transitorio di detonazione attiva.
bool Bomb::isExploding(){return exploding;}

// Indica se la bomba e' ancora attiva sulla scacchiera o pronta per la rimozione da memoria.
bool Bomb::isActive(){
    return active;
}

void Bomb::display(){
    wrefresh(curwin);
}

int Bomb::getX(){return xLoc;}

int Bomb::getY(){return yLoc;}

// Costruttore: inizializza le coordinate, il raggio di propagazione delle fiamme
// e azzera i timer cronometrici relativi al lampeggio e all'innesco della detonazione.
Bomb::Bomb(int x, int y, WINDOW* win, int estensione){
    yLoc = y;
    xLoc = x;
    curwin = win;
    visible = true;
    active = true;
    exploding = false;
    last_time_point = std::chrono::steady_clock::now();
    explode_time = std::chrono::steady_clock::now();

    // Inizializzazione dell'array di direzione
    for (int i = 0; i < 4; i++) {
        direzione[i] = true;
    }

    est_bombe = estensione;
    
    getmaxyx(curwin, YMax, XMax);
}
