#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <sstream>
#include "json.hpp"

#include "clip.hpp"

int writeToFile(const std::string& gameName) {
    time_t timestamp;
    time(&timestamp);
    tm* now = localtime(&timestamp);

    std::stringstream titleStream;
    titleStream << (now->tm_year + 1900) << "_" << (now->tm_mon + 1) << "_" << (now->tm_mday) << "_" << (now->tm_hour) << "_" << (now->tm_min) << "_" << (now->tm_sec);
    std::string title = titleStream.str();

    nlohmann::ordered_json newClip;

    newClip["title"] = title;
    newClip["game"] = gameName;

    std::ifstream inFile("../json_storage/clip.json");

    if (!inFile.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    std::string content((std::istreambuf_iterator<char>(inFile)),std::istreambuf_iterator<char>());

    if (content.empty()) {
        content = "[]";
    }

    nlohmann::ordered_json clip = nlohmann::ordered_json::parse(content);

    // ensure clip is an array
    if (!clip.is_array()) {
        clip = nlohmann::ordered_json::array();
    }

    clip.emplace_back(newClip);

    std::ofstream outFile("../json_storage/clip.json");

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    outFile << clip.dump(4);

    inFile.close();
    outFile.close();

    return 0;
}

std::vector<Clip> readFromFile() {
    std::vector<Clip> clipsVec;
    std::ifstream inFile("../json_storage/clip.json");

    if (!inFile.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return {};
    }

    nlohmann::json clips = nlohmann::json::parse(inFile);

    for (int i = 0; i < clips.size(); i++) {
        Clip temp;
        temp.title = clips[i]["title"];
        temp.duration = clips[i]["duration"];
        temp.game = clips[i]["game"];

        clipsVec.push_back(temp);
    };

    return clipsVec;
}

int sortByChoice(int ascending, int sortType, std::vector<Clip>& clipVec) {
    switch (sortType) {
        case 0: // sort by title
            if (ascending == 0) {
                std::sort(clipVec.begin(), clipVec.end(), [](const Clip& a, const Clip& b) {
                    return a.title < b.title;
                });
            } else {
                std::sort(clipVec.begin(), clipVec.end(), [](const Clip& a, const Clip& b) {
                    return a.title > b.title;
                });
            }
            break;

        case 1: // sort by duration
            if (ascending == 0) {
                std::sort(clipVec.begin(), clipVec.end(), [](const Clip& a, const Clip& b) {
                    return a.duration < b.duration;
                });
            } else {
                std::sort(clipVec.begin(), clipVec.end(), [](const Clip& a, const Clip& b) {
                    return a.duration > b.duration;
                });
            }
            break;

        case 2: // sort by game
            if (ascending == 0) {
                std::sort(clipVec.begin(), clipVec.end(), [](const Clip& a, const Clip& b) {
                    return a.game < b.game;
                });
            } else {
                std::sort(clipVec.begin(), clipVec.end(), [](const Clip& a, const Clip& b) {
                    return a.game > b.game;
                });
            }
            break;

        default:
            std::cout << "Incorrect input" << std::endl;
            return 1;
    }

    return 0;
}