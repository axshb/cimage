#pragma once

#include <string>
#include <set>
#include <map>

#include <nlohmann/json.hpp>

namespace Comfy {

namespace Utils {

class ComfyMetadataExtractor {
public:
    void extract_metadata(const std::string& raw_json);
    void crawl_prompts(const std::string& k, const std::string& val);
    
    const std::set<std::string>& get_prompts() {
        return prompts;
    };

private:
    std::set<std::string> prompts;
    std::map<std::string, std::string> sampling_settings;
    std::set<std::string> assets;
    void process_node(nlohmann::json& node_inputs);
};

} // namespace Utils
} // namespace Comfy
