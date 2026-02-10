#include "comfy_utils.hpp"
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

/*
Helper function to get prompts
*/
void ComfyMetadataExtractor::crawl_prompts(const std::string& k, const std::string& val) {
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

/*
Extracting ComfyUI's JSON DAG metadata
*/
void ComfyMetadataExtractor::extract_metadata(const std::string& raw_json) {
    try {
        json data = json::parse(raw_json);
        
        // clear previous state if reused
        prompts.clear();
        sampling_settings.clear();
        assets.clear();

        for (auto& [id, node] : data.items()) {
            auto& inputs = node["inputs"];

            for (auto& [k, v] : inputs.items()) {
                // prompt crawling
                if (v.is_string()) {
                    crawl_prompts(k, v.get<std::string>());
                }

                // crawl for checkpoints and loras
                if (k.find("ckpt_name") != std::string::npos || k.find("lora") != std::string::npos || 
                (v.is_string() && v.get<std::string>().find(".safetensors") != std::string::npos)) {
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