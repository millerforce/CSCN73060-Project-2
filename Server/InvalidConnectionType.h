
#ifndef INVALIDCONNECTIONTYPE_H
#define INVALIDCONNECTIONTYPE_H

#include <exception>
#include <string>

class InvalidConnectionTypeException : public std::exception {
private:
    std::string message;
public:
    explicit InvalidConnectionTypeException(std::string msg) : message(std::move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif //INVALIDCONNECTIONTYPE_H
