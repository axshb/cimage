#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>

/* 
Helper from @mrousavy
https://gist.github.com/mrousavy/584d1b72cc381f01d6f1af0bbec7343c
flips big endian bytes from the png to little endian ints for c++
aka, turn 4 bytes into a number
*/
namespace png {
    uint32_t get_length(const unsigned char* b) {
        return (uint32_t)b[0] << 24 | (uint32_t)b[1] << 16 | (uint32_t)b[2] << 8 | (uint32_t)b[3];
    }
}

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
        uint32_t length = png::get_length(len_buff);

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
                std::cout << "Found Metadata: " << content.substr(null_pos + 1) << std::endl;
            }
            
        } else {
            // not the right chunk, go next (data length + 4 bytes of CRC jump)
            file.seekg(length + 4, std::ios::cur);
        }
    }
    return 0;
}