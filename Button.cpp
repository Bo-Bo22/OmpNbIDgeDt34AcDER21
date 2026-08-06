#include "Button.hpp"
#include <cstring>

using namespace std; //per non usare std:: prima delle funzioni della libreria cstring

Button::Button(WINDOW* win, int y, int x, int y_str, int x_str, int width, const char* label) {
    this->win = win;
    btnWin = newwin(3, width, y, x);
    this->y = y;
    this->x = x;
    this->y_str = y_str;
    this->x_str = x_str;
    this->width = width;
    
    // Copia al massimo 31 caratteri per non sforare l'array da 32
    strncpy(this->label, label, 31); 
    
    // Inserisco manualmente il terminatore null nell'ultima cella (l'indice 31)
    this->label[31] = '\0';
    
    state = ButtonState::NORMAL;
    blinkOn = false;
    blinkTick = 0;
}

//Disegna il bottone in base al suo stato
void Button::tick() {
    switch(state) { //Lo state cambia in base a cosa punta "curr", e il bottone lampeggia se è selezionato

        //Stato del bottone "inattivo", nessun lampeggio
        case ButtonState::NORMAL:
            blinkOn = false;
            draw();
            break;

        //Stato "selezionato", lampeggia 
        case ButtonState::SELECTED:
            blinkTick++;
            blinkOn = ((blinkTick % 10) < 5); //La variabile blinkOn cambia ogni 5 tick, creando un effetto di lampeggio
            draw();
            break;

        //Stato "premuto", si allarga e si riduce in un effetto di "pressione"
        case ButtonState::PRESSING:
            blinkOn = false;
            blinkTick = 0;
            
            // Riduciamo a 6 frame per un "click" più rapido e scattante
            for(int a = 0; a < 6 ; a++) {
                
                int espX = 0; 
                int espY = 0; // DISATTIVATO: Non tocchiamo l'altezza per evitare l'effetto "gigante"
                
                // =========================================================
                // LOGICA DELL'ANIMAZIONE (Solo orizzontale)
                // =========================================================
                if (a == 0 || a == 5) {
                    // Frame iniziale e finale: Dimensione originale
                    espX = 0; 
                } 
                else if (a == 1 || a == 4) {
                    // Transizione: +1 carattere a destra e +1 a sinistra
                    espX = 1; 
                } 
                else {
                    // Momento di massima pressione (frame 2 e 3): +2 caratteri per lato
                    espX = 2; 
                }

                // =========================================================
                // CALCOLO DIMENSIONI E COORDINATE
                // =========================================================
                int altezzaAttuale = 3 + (espY * 2); // Rimane sempre 3
                int larghezzaAttuale = 20 + (espX * 2); // Varia da 20 a 24 massimo

                int coordinataY = y - espY; // Rimane sempre y
                int coordinataX = x - espX; // Si adatta per mantenere il centro

                // Pulisce il frame precedente
                wclear(btnWin);
                wrefresh(btnWin);
                delwin(btnWin);
                
                // Disegna il frame attuale
                btnWin = newwin(altezzaAttuale, larghezzaAttuale, coordinataY, coordinataX);
                draw();
                
                // Delay tra un frame e l'altro durante l'animazione (25 millisecondi)
                napms(25); 
            }
            
            // =========================================================
            // IL DELAY FINALE (PAUSA DI RILASCIO)
            // =========================================================
            // L'animazione è finita, il bottone è tornato alla dimensione normale.
            // Fermiamo il gioco per 150 millisecondi (poco più di un decimo di secondo)
            // per far "vedere" il bottone rilasciato prima che il menu si chiuda!
            napms(150);
            
            break;
    }
}

// Disegna il bottone in base al suo stato
void Button::draw() {
    
    // 1. Chiediamo a ncurses le dimensioni ATTUALI della finestra in questo esatto frame
    int h, w;
    getmaxyx(btnWin, h, w);
    
    // 2. Calcoliamo il centro matematico
    int textY = h / 2;
    int textX = (w - strlen(label)) / 2; 

    switch(state) {
        case ButtonState::NORMAL:
            box(btnWin, 0, 0);
            // Sostituiamo y_str e x_str con le nostre coordinate dinamiche
            mvwprintw(btnWin, textY, textX, "%s", label);
            wrefresh(btnWin);
            break;
            
        // Effetto di lampeggio
        case ButtonState::SELECTED:
            if (blinkOn) {
                box(btnWin, 0, 0);
                mvwprintw(btnWin, textY, textX, "%s", label);
                wrefresh(btnWin);
            }
            else {
                wattron(btnWin, COLOR_PAIR(2));
                box(btnWin, 0, 0);
                mvwprintw(btnWin, textY, textX, "%s", label);
                // È buona prassi spegnere l'attributo colore PRIMA del refresh!
                wattroff(btnWin, COLOR_PAIR(2)); 
                wrefresh(btnWin);
            }
            break;
            
        case ButtonState::PRESSING:
            // Anche durante l'animazione, il testo ricalcolerà il centro ad ogni frame
            box(btnWin, 0, 0);
            mvwprintw(btnWin, textY, textX, "%s", label);
            wrefresh(btnWin);
            break;
    }
}

void Button::setState(ButtonState s) {
    state = s;
}

ButtonState Button::getState() const {
    return state;
}

const char* Button::getLabel() const {
    return label;
}