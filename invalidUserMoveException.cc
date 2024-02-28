#include "invalidUserMoveException.h"
InvalidUserMoveException::InvalidUserMoveException() : errorMessage{"Invalid move"} {}
const char *InvalidUserMoveException::what() const noexcept {
    return errorMessage.c_str();
}
