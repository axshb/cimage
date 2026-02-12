#include "../src/png_utils.hpp"
#include "../src/comfy_utils.hpp"
#include <cassert>
#include <iostream>
#include <string>

using Comfy::Utils::ComfyMetadataExtractor;

void test_png_length() {
    unsigned char buffer[] = {0x00, 0x00, 0x00, 0x0D}; // 13 in big-endian
    uint32_t result = ByteConversions::get_length(buffer);
    assert(result == 13);
    std::cout << "test_png_length passed." << std::endl;
}

void test_crawl_prompts() {
    std::string k_1 = "text";
    std::string prompt_1 = "masterpiece, 8k, ultra highres, artist(0.5)";
    
    std::string k_2 = "part";
    std::string prompt_2 = "masterpiece, 8k, ultra highres, artist(0.5)";
    
    std::string k_3 = "value";
    std::string prompt_3 = "absurdres, 4k, 1girl, detailed background,";
    
    ComfyMetadataExtractor extractor;

    extractor.crawl_prompts(k_1, prompt_1);
    extractor.crawl_prompts(k_2, prompt_2);
    extractor.crawl_prompts(k_3, prompt_3);
    
    std::set<std::string> test_prompts { "masterpiece, 8k, ultra highres, artist(0.5)", "absurdres, 4k, 1girl, detailed background," };
    std::set<std::string> returned_prompts = extractor.get_prompts();     
    
    assert(test_prompts == returned_prompts);
    std::cout << "test_crawl_prompts passed." << std::endl;
}

int main() {
    std::cout << "Testing png_length..." << std::endl;
    test_png_length();

    std::cout << "Testing crawl_prompts..." << std::endl;
    test_crawl_prompts();
    
    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}