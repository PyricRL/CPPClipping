#include <iostream>
#include <fstream>
#include "json.hpp"

static const std::string SETTINGS_FILE = "../json_storage/settings.json";

nlohmann::json load_json() {
    std::ifstream file(SETTINGS_FILE);

    if (!file.is_open()) {
        std::cerr << "Failed to open settings file" << std::endl;
        return nlohmann::json::object();
    }

    nlohmann::json j;
    file >> j;
    return j;
}

void save_json(const nlohmann::json& j) {
    std::ofstream file(SETTINGS_FILE);
    file << j.dump(4);
}