#pragma once

// Credits:
// - https://github.com/rkg82/uuid-v4
// - https://github.com/mariusbancila/stduuid

#include <array>
#include <string>

class UUID {
public:
    // Default: generate random UUID (v4)
    UUID() {
        generate();
    }

    // Construct from raw bytes
    explicit UUID(const std::array<uint8_t, 16>& bytes) : bytes_arr(bytes) {}

    // Construct from string
    explicit UUID(const std::string& str) {
        fromString(str);
    }

    // Get raw bytes
    const std::array<uint8_t, 16>& bytes() const;

    // Convert to string
    std::string toString() const;

    // Static factory from string
    static UUID fromStringStatic(const std::string& str);

    // Static factory from bytes
    static UUID fromBytes(const std::array<uint8_t, 16>& bytes);

private:
    std::array<uint8_t, 16> bytes_arr;

    void generate();

    void fromString(const std::string& str);
};
