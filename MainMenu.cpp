#include "Map.hpp"
#include "MainMenu.hpp"
#include "Button.hpp"

#include <ncurses.h>
#include <cassert>

int MainMenu::run(int MaxY, int MaxX){

    while(true){
        drawMain(MaxY, MaxX);
        int ch = getch();
        switch(ch){
            
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
            
            case '\n':
            case '\r':
            case KEY_ENTER:
                curr->btn->setState(ButtonState::PRESSING);
                curr->btn->tick();
                if (curr->btn->getLabel() == "GIOCA"){
                    return 0;
                }
                else if (curr->btn->getLabel() == "LISTA LIVELLI"){
                    return 1;
                }
                else if(curr->btn->getLabel() == "LEADERBOARD"){
                    return 2;
                }
                else if (curr->btn->getLabel() =="ESCI"){
                    return -1;
                }
                break;
                
              
            case 'q':
                return -1;
            
        }
        napms(50);
    }
}


void MainMenu::drawMain(int MaxY, int MaxX){
    const char* label[5]={
        "###   ####  #     #  ###   ###  ###    #     #      #      #     #",
        "#  #  #  #  # # # #  #  #  #    #  #   # # # #     # #     # #   #",
        "###   #  #  #  #  #  ###   ###  ###    #  #  #    #   #    #  #  #",
        "#  #  #  #  #     #  #  #  #    #  #   #     #   #######   #   # #",
        "###   ####  #     #  ###   ###  #   #  #     #  #       #  #     #"
    };

    
    
    for (int i= 0; i < 5; i = i+1){
        mvprintw((MaxY/3) +i, MaxX/3, label[i]);
    }
    refresh();

    plista_cmd tmp = n1;

    for (int i = 0; i<4; i++){
        tmp->btn->tick();
        tmp=tmp->next;
    }

    curr->btn->setState(ButtonState::SELECTED);
    curr->btn->tick();
}


MainMenu::MainMenu(int MaxY, int MaxX):
    
    play(stdscr, MaxY/2 + 2, MaxX/3 + 24, 1, 7, 20, "GIOCA"),
    lvlList(stdscr, MaxY/2 + 6, MaxX/3 + 24,  1, 4, 20, "LISTA LIVELLI"),
    ldrBoard(stdscr, MaxY/2 + 10, MaxX/3 + 24, 1, 4, 20, "LEADERBOARD"),
    exitBtn(stdscr, MaxY/2 + 16, MaxX/3 + 24, 1, 7, 20, "ESCI")
    
{
    currentPage = MenuPage::MAIN;
    selectedOption = 0;

    //lista bidirezionale circolare per scorrere i tasti
    this-> n1 = new lista_cmd{&play, NULL, NULL};
    this-> n2 = new lista_cmd{&lvlList, NULL, n1};
    this-> n3 = new lista_cmd{&ldrBoard, NULL, n2};
    this-> n4 = new lista_cmd{&exitBtn, n1, n3};
    
    n1->next = n2;
    n1->prev = n4;

    n2->next = n3;

    n3->next = n4;

    this -> curr = n1;
}

