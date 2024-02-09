#include "invalidUserMove.h"
InvalidUserMove::InvalidUserMove(): errorMessage{"Invalid move"} {}
const char* InvalidUserMove::what() const noexcept {
    return errorMessage.c_str();
}
