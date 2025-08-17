#pragma once
#include <iostream>
#include <fstream>
#include "json.hpp"

static const std::string SETTINGS_FILE = "../json_storage/settings.json";

nlohmann::json load_json();

void save_json(const nlohmann::json& j);

template<typename T>
T get_setting(const std::string& key) {
    std::ifstream file(SETTINGS_FILE);

    if (!file.is_open()) {
        std::cerr << "Failed to open settings file" << std::endl;
        return T();
    }

    nlohmann::json j;
    file >> j;

    if (j.contains(key)) {
        return j[key].get<T>();
    }
    return T();
}

template<typename T>
void set_settings(const std::string& key, const T& value) {
    nlohmann::json j = load_json();
    j[key] = value;
    save_json(j);
}