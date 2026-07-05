//#include "LevelManager.hpp"
#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include <ncurses.h>
#include "Button.hpp"
#include <cassert>

enum class MenuPage { MAIN, LEVEL_LIST, LEADERBOARD };

struct lista_cmd{
    Button* btn;
    lista_cmd* next;
    lista_cmd* prev;
};

typedef lista_cmd* plista_cmd;

class MainMenu{
    MenuPage currentPage;
    int selectedOption;
    int selectedLevel;
    
    Button play;
    Button lvlList;
    Button ldrBoard;   
    Button exitBtn; 
    plista_cmd n1; 
    plista_cmd n2; 
    plista_cmd n3; 
    plista_cmd n4;
    plista_cmd curr; 
    
    //LevelManager& lm;
    void drawMain(int MaxY, int MaxX);
    void drawLevelList();
    void drawLeaderboard();
    void drawLevelPreview(int Livello, int winY, int WinX);

    

public:
    
    MainMenu(int MaxX, int MaxY);//LevelManager& lm
    int run(int MaxY, int MaxX); //return il livello da caricare. 

};
#endif