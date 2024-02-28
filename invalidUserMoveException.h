#ifndef INVALIDUSERMOVEEXCEPTION_H
#define INVALIDUSERMOVEEXCEPTION_H
#include <stdexcept>
#include <string>
using namespace std;
class InvalidUserMoveException: public exception {
    string errorMessage;
  public:
    InvalidUserMoveException();
    const char* what() const noexcept override;
};
#endif
