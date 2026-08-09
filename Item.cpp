#include "Item.hpp"

Item::Item(int itemType) {
    type = itemType;
}

void Item::applyEffect(Player* p) {
    // Tutti gli oggetti sembrano uguali ($), ma qui decidiamo cosa fanno!
    switch(type) {
        case 1:
            p->addScore(2000); // Sostituisci con p->addLife() se preferisci
            break;
        case 2:
            p->addScore(1000); // Es: Aggiunge una bomba
            break;
        case 3:
            p->addScore(500);  // Es: Aumenta il raggio dell'esplosione
            break;
    }
}