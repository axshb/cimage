#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <set>
#include <map>

namespace comfy_utils {
    void extract_metadata(const std::string& raw_json);
}