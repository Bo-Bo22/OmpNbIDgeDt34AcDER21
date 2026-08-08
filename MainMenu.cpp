#include "Map.hpp" 
#include "MainMenu.hpp" 
#include "Button.hpp" 
#include <ncurses.h> 
#include <cstring>

using namespace std;

MainMenu::MainMenu(int MaxY, int MaxX) : 

    // Inizializzazione dei bottoni con le coordinate calcolate in base alla dimensione dello schermo
    // Spazio orizzontale centrato: (MaxX - larghezza_bottone) / 2
    play(stdscr, MaxY/2 - 4, (MaxX - 20) / 2, 1, 7, 20, "GIOCA"),
    ldrBoard(stdscr, MaxY/2, (MaxX - 20) / 2, 1, 4, 20, "LEADERBOARD"),
    exitBtn(stdscr, MaxY/2 + 4, (MaxX - 20) / 2, 1, 8, 20, "ESCI") 
{
    // Inizializza lo stato del menu e dei bottoni usando una lista concatenata circolare
    currentPage = MenuPage::MAIN;
    selectedOption = 0;
    
    //sono variabili membri della classe definite nell'header e non variabili temporanee create sul momento.
    n1 = new lista_cmd{&play, NULL, NULL};
    n2 = new lista_cmd{&ldrBoard, NULL, n1};
    n3 = new lista_cmd{&exitBtn, n1, n2};
         
    n1->next = n2;
    n1->prev = n3;
    n2->next = n3;
    n3->next = n1;
    curr = n1;
}

void MainMenu::drawMain(int MaxY, int MaxX) {
    clear();
    
    // Nuova scritta ASCII art compatta (Larghezza: 63 caratteri), il doppio backslash serve per evitare warning
    // causati dalla sequenza "\/" che può essere interpretata come un carattere di escape in alcune configurazioni.
const char* label[5] = {
    "    ____                  __                                   ",
    "   / __ )____  ____ ___  / /_  ___  _________ ___  ____ _____  ",
    "  / __  / __ \\/ __ `__ \\/ __ \\/ _ \\/ ___/ __ `__ \\/ __ `/ __ \\ ",
    " / /_/ / /_/ / / / / / / /_/ /  __/ /  / / / / / / /_/ / / / / ",
    "/_____/\\____/_/ /_/ /_/_.___/\\___/_/  /_/ /_/ /_/\\__,_/_/ /_/  "
};
int testo_width = 63; // Ricordati di aggiornare testo_width a 63 nel drawMain!
    // Calcola il punto di partenza esatto per centrare la scritta orizzontalmente
    int start_x_scritta = (MaxX - testo_width) / 2;
    if (start_x_scritta < 0) start_x_scritta = 0;

    // Stampa il titolo centrato nella parte alta (riga MaxY / 6)
    for (int i = 0; i < 5; i++) {
        mvprintw((MaxY / 6) + i, start_x_scritta, "%s", label[i]);
    }
    
    refresh();
    
    // Disegna e aggiorna tutti i pulsanti della lista usando un puntatore per scorrere la lista
    plista_cmd tmp = n1;
    for (int i = 0; i < 3; i++) { // Aggiornato a 3 bottoni
        tmp->btn->tick();
        tmp = tmp->next;
    }
    
    curr->btn->setState(ButtonState::SELECTED);
    curr->btn->tick();
}


int MainMenu::run(int MaxY, int MaxX) {

    // Rende getch() NON bloccante: se l'utente non preme nulla, il ciclo continua a girare e non aspetta l'input da getch
    nodelay(stdscr, TRUE);
    
    while(true) {
        drawMain(MaxY, MaxX);
        
        int ch = getch();
        switch(ch) {

            case ERR:
                // Nessun tasto premuto: non fare nulla, lascia che il ciclo continui
                // in modo che drawMain() e i tick() dei bottoni aggiornino il lampeggio
                break;
                
            case KEY_UP:
                curr->btn->setState(ButtonState::NORMAL);
                curr->btn->tick();
                curr = curr->prev;
                curr->btn->setState(ButtonState::SELECTED);
                curr->btn->tick();
                break;
                
            case KEY_DOWN:
                curr->btn->setState(ButtonState::NORMAL);
                curr->btn->tick();
                curr = curr->next;
                curr->btn->setState(ButtonState::SELECTED);
                curr->btn->tick();
                break;
                
            //Casi "Invio"
            case '\r':
            case '\n':
            case KEY_ENTER:
                curr->btn->setState(ButtonState::PRESSING);
                curr->btn->tick();
                
                // Ripristina getch() bloccante prima di uscire dal menu
                nodelay(stdscr, FALSE);
                

                // Non è possibile usare uno switch su strcmp.
                // Viene presa l'etichetta del bottone selezionato e confrontata con le stringhe dei bottoni
                
                if (strcmp(curr->btn->getLabel(), "GIOCA") == 0) {
                    return 1;
                }
                else if (strcmp(curr->btn->getLabel(), "LEADERBOARD") == 0) {
                    return 2; // Aggiornato a 2
                }
                else if (strcmp(curr->btn->getLabel(), "ESCI") == 0) {
                    return -1;
                }
                break;
                
            //Caso q per uscire dal menu
            case 'q':
                // Ripristina getch() bloccante prima di uscire
                nodelay(stdscr, FALSE);
                return -1;
        }
        
        // Questo delay rallenta il ciclo principale (e regola la velocità del lampeggio)
        napms(100); 
    }
}