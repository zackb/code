#pragma once

#include <SDL_mixer.h>
#include <string>
#include <unordered_map>

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    bool load(const std::string& id, const std::string& path);
    void play(const std::string& id, int loops = 0);
    void stopAll();
    void setVolume(int volume);

private:
    std::unordered_map<std::string, Mix_Chunk*> soundMap;
};
