#pragma once

#include <SDL_mixer.h>
#include <string>

class MusicManager {
public:
    MusicManager();
    ~MusicManager();

    bool load(const std::string& path);
    void play(int loops = -1);
    void stop();
    void pause();
    void resume();
    void fadeIn(int ms, int loops = -1);
    void fadeOut(int ms);
    void setVolume(int volume);
    bool isPlaying() const;
    bool isPaused() const;

private:
    Mix_Music* music;
};
