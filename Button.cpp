#include "Button.hpp"

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

        //Stato "premuto", il bottone si allarga e lampeggia
        case ButtonState::PRESSING:
            blinkOn = false;
            blinkTick = 0;
            for(int a = 0; a < 10 ; a++) {
                blinkTick++;
                width = 20 + blinkTick % 3; // Aumenta la larghezza del bottone

                // Distruggo la vecchia finestra per evitare sovrapposizioni
                delwin(btnWin);
                
                // Creo la nuova finestra allargata e ridisegnala
                btnWin = newwin(3, width, y, x);
                draw();
                
                // Rallento il ciclo di 15 millisecondi per rendere l'animazione visibile
                napms(15);
            }
            break;
    }
}

// Disegna il bottone in base al suo stato
void Button::draw() {
    switch(state) {
        case ButtonState::NORMAL:
            box(btnWin, 0, 0);
            mvwprintw(btnWin, y_str, x_str, "%s", label);
            wrefresh(btnWin);
            break;
            
        // Effetto di lampeggio
        case ButtonState::SELECTED:
            if (blinkOn) {
                box(btnWin, 0, 0);
                mvwprintw(btnWin, y_str, x_str, "%s", label);
                wrefresh(btnWin);
            }
            else {
                wattron(btnWin, COLOR_PAIR(2));
                box(btnWin, 0, 0);
                mvwprintw(btnWin, y_str, x_str, "%s", label);
                wrefresh(btnWin);
                wattroff(btnWin, COLOR_PAIR(2));
            }
            break;
            
        case ButtonState::PRESSING:
            box(btnWin, 0, 0);
            mvwprintw(btnWin, y_str, x_str, "%s", label);
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