#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <set>
#include <map>

namespace png_utils {
    uint32_t get_length(const unsigned char* b);
}