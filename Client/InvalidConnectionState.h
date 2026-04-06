
#ifndef INVALIDCONNECTIONSTATE_H
#define INVALIDCONNECTIONSTATE_H

#include <exception>
#include <string>

class InvalidConnectionStateException : public std::exception {
private:
    std::string message;
public:
    explicit InvalidConnectionStateException(std::string msg) : message(std::move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif //INVALIDCONNECTIONSTATE_H
