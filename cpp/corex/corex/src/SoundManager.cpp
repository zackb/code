#include "corex/SoundManager.h"
#include "corex/FileUtils.h"

#include "corex/json.hpp"
#include <SDL_mixer.h>
#include <fstream>
#include <iostream>

SoundManager::SoundManager() {}

SoundManager::~SoundManager() {
    for (auto& pair : soundMap) {
        Mix_FreeChunk(pair.second);
    }
    soundMap.clear();
}

bool SoundManager::load(const std::string& id, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(resolveAssetPath(path).c_str());
    if (!chunk) {
        std::cerr << "Failed to load sound effect" << path << ": " << Mix_GetError() << std::endl;
        return false;
    }
    soundMap[id] = chunk;
    return true;
}

bool SoundManager::loadFromFile(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open())
        return false;

    nlohmann::json j;
    file >> j;

    for (auto it = j.begin(); it != j.end(); ++it) {
        const std::string& id = it.key();
        const std::string& path = it.value();

        Mix_Chunk* chunk = Mix_LoadWAV(resolveAssetPath(path).c_str());
        if (!chunk) {
            std::cerr << "Failed to load sound: " << path << ": " << Mix_GetError() << std::endl;
            continue;
        }

        soundMap[id] = chunk;
    }

    return true;
}

void SoundManager::play(const std::string& id, int loops) {
    auto it = soundMap.find(id);
    if (it != soundMap.end()) {
        Mix_PlayChannel(-1, it->second, loops);
    }
}

void SoundManager::stopAll() { Mix_HaltChannel(-1); }

void SoundManager::setVolume(int volume) {
    Mix_Volume(-1, volume); // applies to all channels
}
