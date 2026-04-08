
// Credits:
// - https://github.com/rkg82/uuid-v4
// - https://github.com/mariusbancila/stduuid

#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <random>
#include <sstream>

#include "UUID.h"

const std::array<uint8_t, 16>& UUID::bytes() const {
    return bytes_arr;
}

UUID UUID::fromBytes(const std::array<uint8_t, 16>& bytes) {
    return UUID(bytes);
}

UUID UUID::fromStringStatic(const std::string& str) {
    return UUID(str);
}

std::string UUID::toString() const {
    std::ostringstream oss;
    for (size_t i = 0; i < bytes_arr.size(); ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            oss << '-';

        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(bytes_arr[i]);
    }
    return oss.str();
}

void UUID::generate() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);

    for (size_t i = 0; i < 16; i += 4) {
        uint32_t r = dist(gen);
        bytes_arr[i] = (r >> 24) & 0xFF;
        bytes_arr[i + 1] = (r >> 16) & 0xFF;
        bytes_arr[i + 2] = (r >> 8) & 0xFF;
        bytes_arr[i + 3] = r & 0xFF;
    }

    // Set version (4)
    bytes_arr[6] = (bytes_arr[6] & 0x0F) | 0x40;

    // Set variant (RFC 4122)
    bytes_arr[8] = (bytes_arr[8] & 0x3F) | 0x80;
}

static uint8_t hexCharToInt(char c) {
    if (std::isdigit(c)) return c - '0';
    c = std::tolower(c);
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    throw std::invalid_argument("Invalid hex character in UUID");
}

void UUID::fromString(const std::string& str) {
    std::string hex;
    for (char c : str) {
        if (c != '-') hex += c;
    }

    if (hex.size() != 32) {
        throw std::invalid_argument("Invalid UUID string");
    }

    for (size_t i = 0; i < 16; ++i) {
        bytes_arr[i] = (hexCharToInt(hex[2 * i]) << 4)
            | hexCharToInt(hex[2 * i + 1]);
    }
}
