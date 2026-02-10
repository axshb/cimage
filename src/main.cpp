#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <set>
#include <map>

#include <nlohmann/json.hpp>
#include "png_utils.hpp"
#include "comfy_utils.hpp"

using json = nlohmann::json;

/*
Notes:
https://cplusplus.com/reference/istream/istream/read/ 
file.read() expects a pointer to a char in parameter 0
*/
int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Error parsing arguments. Did you include an image path?" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);

    if (!file) {
       std::cerr << "Could not open file." << std::endl;
       return 1;
    }

    // skip the png signature (8 bytes)
    file.seekg(8, std::ios::cur);

    while (file) {
        
        // get length of chunk
        unsigned char len_buff[4];
        if (!file.read((char*)len_buff, 4)) {
            break;
        }
        uint32_t length = png_utils::get_length(len_buff);

        // read type of chunk
        char type_buf[5] = {0}; // all null because 4 chars + null terminator
        file.read(type_buf, 4);
        std::string type = type_buf;

        if (type == "tEXt") {

            // slice, getting data based on length
            std::vector<char> buffer(length);
            file.read(buffer.data(), length);

            // finding null separator by converitng it to a string
            std::string content(buffer.begin(), buffer.end());
            size_t null_pos = content.find('\0');

            // not found makes it return npos (double negative)
            if (null_pos != std::string::npos) {
                std::string key = content.substr(0, null_pos);
                std::string value = content.substr(null_pos + 1);
                
                // comfyui uses prompt as the key for the json/graph
                if (key == "prompt") {
                    comfy_utils::extract_metadata(value);
                }
            }
            
        } else {
            // not the right chunk, go next (data length + 4 bytes of CRC jump)
            file.seekg(length + 4, std::ios::cur);
        }
    }
    return 0;
}