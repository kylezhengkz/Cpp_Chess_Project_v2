#include "player.h"
Player::Player(Colour colour): colour{colour} {}

Colour Player::getColour() {
    return colour;
}
