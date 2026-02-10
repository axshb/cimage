#include "png_utils.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <set>
#include <map>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace png_utils {

    /* 
    Helper from @mrousavy
    https://gist.github.com/mrousavy/584d1b72cc381f01d6f1af0bbec7343c
    flips big endian bytes from the png to little endian ints for c++
    aka, turn 4 bytes into a number
    */
    uint32_t get_length(const unsigned char* b) {
        return (uint32_t)b[0] << 24 | (uint32_t)b[1] << 16 | (uint32_t)b[2] << 8 | (uint32_t)b[3];
    }
}