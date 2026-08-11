#include "Item.hpp"

Item::Item(int itemType) {
    type = itemType;
}

#include "Item.hpp"

void Item::applyEffect(Player* p) {
    switch(type) {
        case 1: 
            p->addScore(500);       // 1. Bonus 500 Punti
            break;
        case 2: 
            p->addLife();           // 2. 1-UP (Vita extra)
            break;
        case 3: 
            p->addBombRange();      // 3. Raggio Fiamma aumentato
            break;
        case 4: 
            p->addMaxBombs();       // 4. Bombe Extra (Piazzane di più)
            break;
        case 5: 
            p->setWallPass(true);   // 5. Passamuri!
            break;
    }
}