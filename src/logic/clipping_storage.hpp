#pragma once
#include <vector>
#include "clip.hpp"

int writeToFile(const std::string& gameName);

std::vector<Clip> readFromFile();

int sortByChoice(int ascending, int sortType, std::vector<Clip>& clipVec);