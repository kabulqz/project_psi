#pragma once

#include "Settings.hpp"


enum class Card_type {
    Unit,
    Spell,
    Item
};


class Card {
    Card_type type;
    std::string background_img;    // Background image
    std::string cardface_1st_img;  // Card face (front)
    std::string cardface_2nd_img;  // Card face (unit/spell/item-specific)
public:
    Card();
};