#include "png_utils.hpp"
#include <cassert>
#include <iostream>

void test_png_length() {
    unsigned char buffer[] = {0x00, 0x00, 0x00, 0x0D}; // 13 in big-endian
    uint32_t result = png_utils::get_length(buffer);
    assert(result == 13);
    std::cout << "test_png_length passed!" << std::endl;
}

int main() {
    test_png_length();
    // Add more test functions here
    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}