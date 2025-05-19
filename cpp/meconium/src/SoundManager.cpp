#include "SoundManager.h"
#include "FileUtils.h"

#include <SDL_mixer.h>
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
