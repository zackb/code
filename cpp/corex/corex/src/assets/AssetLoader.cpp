#include "corex/assets/AssetLoader.h"

#include <iostream>
#include <sstream>

std::vector<std::vector<int>> AssetLoader::loadMapCSV(std::string filePath) {
    std::vector<std::vector<int>> map;
    std::string realPath = resolveAssetPath(filePath);

    if (!std::filesystem::exists(realPath)) {
        std::cerr << "Failed to load map from file: " + realPath << std::endl;
    }

    std::ifstream file(realPath);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string val;
        std::vector<int> rowVec;

        while (std::getline(ss, val, ',')) {
            rowVec.push_back(std::stoi(val));
        }

        map.push_back(std::move(rowVec));
    }
    return map;
}
