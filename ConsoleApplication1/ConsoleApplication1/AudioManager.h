#pragma once
#include "raylib.h"
#include <string>

class AudioManager {
public:
    // Access the singleton instance
    static AudioManager& Get() {
        static AudioManager instance; // created on first use, destroyed automatically
        return instance;
    }

    // Delete copy/move constructors
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void LoadMusic(const std::string& path) { music = LoadMusicStream(path.c_str()); }
    void PlayMusic() { PlayMusicStream(music); }
    void StopMusic() { StopMusicStream(music); }

private:
    AudioManager() {}                // private constructor
    ~AudioManager() { UnloadMusicStream(music); }

    Music music;
};

