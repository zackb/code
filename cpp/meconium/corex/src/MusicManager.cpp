#include "corex/MusicManager.h"
#include "corex/FileUtils.h"
#include <SDL_mixer.h>
#include <iostream>

MusicManager::MusicManager() : music(nullptr) {}

MusicManager::~MusicManager() {
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
}

bool MusicManager::load(const std::string& path) {
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }

    music = Mix_LoadMUS(resolveAssetPath(path).c_str());
    if (!music) {
        std::cerr << "Failed to load music " << path.c_str() << " : " << Mix_GetError() << std::endl;
        return false;
    }
    return true;
}

void MusicManager::play(int loops) {
    if (music) {
        Mix_PlayMusic(music, loops);
    }
}

void MusicManager::stop() { Mix_HaltMusic(); }

void MusicManager::pause() {
    if (Mix_PlayingMusic() && !Mix_PausedMusic()) {
        Mix_PauseMusic();
    }
}

void MusicManager::resume() {
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}

void MusicManager::fadeIn(int ms, int loops) {
    if (music) {
        Mix_FadeInMusic(music, loops, ms);
    }
}

void MusicManager::fadeOut(int ms) { Mix_FadeOutMusic(ms); }

void MusicManager::setVolume(int volume) {
    Mix_VolumeMusic(volume); // MIX_MAX_VOLUME = 128
}

bool MusicManager::isPlaying() const { return Mix_PlayingMusic() != 0; }

bool MusicManager::isPaused() const { return Mix_PausedMusic() != 0; }
