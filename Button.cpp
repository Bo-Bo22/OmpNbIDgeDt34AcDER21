#include "Button.hpp"

//tick controlla lo stato del bottone e chiama la funzione di disegno.
//gestisce anche i parametri interni del bottone.

void Button::tick(){
    switch(state){
        
        case ButtonState::NORMAL:
        blinkOn = false;
        draw();
        break;

        case ButtonState::SELECTED:
        blinkTick++;
        blinkOn = (blinkTick % 10) < 5;
        draw();
        break;

        case ButtonState::PRESSING:
        blinkOn  = false;
        blinkTick = 0;
        for(int a = 0; a < 10 ; a++){
            blinkTick++;
            width = 20 + blinkTick++;
            draw();
            break;
        }
    }
}
void Button::draw(){
    
    switch(state){
    
        case ButtonState::NORMAL:
            
            box(btnWin, 0, 0);                     // disegna il bordo del pulsante
            mvwprintw(btnWin, y_str , x_str, label.c_str()); // testo centrato dentro
            wrefresh(btnWin);
            break;            
        
        case ButtonState::SELECTED:

            if (blinkOn){
                box(btnWin, 0, 0);                     // disegna il bordo del pulsante
                mvwprintw(btnWin, y_str , x_str, label.c_str()); // testo centrato dentro
                wrefresh(btnWin);
            }
            else {
                wattron(btnWin, COLOR_PAIR(2));    
                box(btnWin, 0, 0);                     // disegna il bordo del pulsante
                mvwprintw(btnWin, y_str , x_str, label.c_str()); // testo centrato dentro
                wrefresh(btnWin);
                wattroff(btnWin, COLOR_PAIR(2));
            }
            break;

        case ButtonState::PRESSING:
            btnWin = newwin(3, width, y,x);
            box(btnWin, 0, 0);                     // disegna il bordo del pulsante
            mvwprintw(btnWin, y_str , x_str, label.c_str()); // testo centrato dentro
            wrefresh(btnWin);
            break;   
            
    }
    

}


void Button::setState(ButtonState s){
    state = s;
    return;
}

ButtonState Button::getState() const {
    return state;
}

int Button::getWidth(){
    return width;
}

const std::string& Button::getLabel() const {
    return label;
}



Button::Button(WINDOW* win, int y, int x, int y_str, int x_str, int width, const std::string & label){
    this->win = win;
    this->btnWin = newwin(3, width, y, x);
    this->y = y;
    this->x = x;
    this->y_str = y_str;
    this->x_str = x_str;
    this->width = width;
    this->label = label;
    this->state = ButtonState::NORMAL;
    this->blinkOn = false;
    this->blinkTick = 0;
}