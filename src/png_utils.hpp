#pragma once

#include <cstdint>

namespace ByteConversions {
    uint32_t get_length(const unsigned char* b);
}

namespace ByteLengths{
    constexpr int LEN_PNG_SIGNATURE = 8;
    constexpr int LEN_CHUNK_PREFIX = 4;
    constexpr int LEN_CHUNK_TYPE = 5;
    constexpr int LEN_CRC = 4;
}
