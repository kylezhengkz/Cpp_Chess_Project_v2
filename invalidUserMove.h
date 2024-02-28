#ifndef INVALIDUSERMOVE_H
#define INVALIDUSERMOVE_H
#include <stdexcept>
#include <string>
using namespace std;
class InvalidUserMove : public exception {
    string errorMessage;

   public:
    InvalidUserMove();
    const char *what() const noexcept override;
};
#endif
