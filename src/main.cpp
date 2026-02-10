#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <set>
#include <map>

using json = nlohmann::json;

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
Helper to parse the JSON DAG
*/
void extract_metadata(const std::string& raw_json) {
    try {
        json data = json::parse(raw_json);
        
        // raw data bucks
        std::set<std::string> prompts;
        std::map<std::string, std::string> sampling_settings;
        std::set<std::string> assets;

        for (auto& [id, node] : data.items()) {
            std::string type = node.value("class_type", "");
            auto& inputs = node["inputs"];

            for (auto& [k, v] : inputs.items()) {
                // prompt crawling
                if (v.is_string()) {
                    std::string val = v.get<std::string>();
                    std::string key_lower = k;
                    std::transform(key_lower.begin(), key_lower.end(), key_lower.begin(), ::tolower);

                    // blacklist
                    bool is_file = (val.find(".pt") != std::string::npos || 
                                    val.find(".pth") != std::string::npos || 
                                    val.find(".jpeg") != std::string::npos ||
                                    val.find(".json") != std::string::npos ||
                                    val.find("Select") != std::string::npos);

                    // heuristic; these criteria likely make it a prompt
                    bool has_commas = (val.find(",") != std::string::npos);
                    bool is_prompt_key = (key_lower == "text" || key_lower.find("part") != std::string::npos || key_lower == "value");

                    if (!is_file && val.length() > 5) {
                        if (has_commas || is_prompt_key) {
                            prompts.insert(val);
                        }
                    }
                }

                // crawl for checkpoints and loras
                if (k.find("ckpt_name") != std::string::npos || k.find("lora") != std::string::npos || (v.is_string() && v.get<std::string>().find(".safetensors") != std::string::npos)) {
                    assets.insert(v.is_string() ? v.get<std::string>() : v.dump());
                }

                // crawl for sampling params
                if (k == "cfg" || k == "steps" || k == "steps_total" || k == "sampler_name" || k == "scheduler" || k == "denoise") {
                    if (!v.is_array()) { // Only take the raw value from the source config
                        sampling_settings[k] = v.dump();
                    }
                }
                
                // seeds
                if (k == "seed" || k == "noise_seed") {
                    if (!v.is_array()) sampling_settings["seed"] = v.dump();
                }
            }
        }

        std::cout << "\n>>> IMAGE GENERATION SUMMARY <<<\n";

        std::cout << "\n[MODELS & LORAS]\n";
        for (const auto& a : assets) std::cout << "  - " << a << "\n";

        std::cout << "\n[SAMPLING SETTINGS]\n";
        for (auto const& [key, val] : sampling_settings) {
            std::cout << "  " << key << ": " << val << "\n";
        }

        std::cout << "\n[PROMPTS]\n";
        for (const auto& p : prompts) {
            std::cout << "  > " << p << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
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
                std::string key = content.substr(0, null_pos);
                std::string value = content.substr(null_pos + 1);
                
                // comfyui uses prompt as the key for the json/graph
                if (key == "prompt") {
                    extract_metadata(value);
                }
            }
            
        } else {
            // not the right chunk, go next (data length + 4 bytes of CRC jump)
            file.seekg(length + 4, std::ios::cur);
        }
    }
    return 0;
}