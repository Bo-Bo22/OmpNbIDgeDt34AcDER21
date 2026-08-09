#ifndef ITEM_HPP
#define ITEM_HPP

#include "Player.hpp"

class Item {
private:
    int type; // 1 = Punti/Vita, 2 = Bomba Extra, 3 = Raggio, ecc.

public:
    Item(int itemType);
    void applyEffect(Player* p);
};

#endif