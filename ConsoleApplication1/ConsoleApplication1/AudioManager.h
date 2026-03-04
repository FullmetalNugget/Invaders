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

    // Initialize audio
    bool Init() {
        if (!initialized) {
            InitAudioDevice();
            initialized = IsAudioDeviceReady();
            if (!initialized) TraceLog(LOG_WARNING, "AudioManager: audio device not ready");
        }
        return initialized;
    }

    // Load a music stream from file path. Returns true on success.
    bool LoadMusic(const std::string& path) {
        if (!initialized && !Init()) return false;
        // Unload previous if loaded
        if (loaded) {
            StopMusicStream(music);
            UnloadMusicStream(music);
            loaded = false;
            playing = false;
        }
        music = LoadMusicStream(path.c_str());
        loaded = (music.ctxData != nullptr);
        if (!loaded) TraceLog(LOG_WARNING, "AudioManager: failed to load music '%s' (working dir: %s)", path.c_str(), GetWorkingDirectory());
        return loaded;
    }

    // Playback control
    void Play() {
        if (loaded) {
            PlayMusicStream(music);
            playing = true;
        }
    }
    void Pause() {
        if (loaded) {
            PauseMusicStream(music);
            playing = false;
        }
    }
    void Resume() {
        if (loaded) {
            ResumeMusicStream(music);
            playing = true;
        }
    }
    void Stop() {
        if (loaded) {
            StopMusicStream(music);
            playing = false;
        }
    }

    // Must be called each frame to stream music
    void Update() {
        if (!loaded) return;
        UpdateMusicStream(music);

        // Manual looping to ensure continuous playback across builds
        float played = GetMusicTimePlayed(music);
        float length = GetMusicTimeLength(music);
        if (length > 0.0f && played >= length) {
            PlayMusicStream(music);
            playing = true;
        }
    }

    void SetVolume(float v) {
        if (loaded) SetMusicVolume(music, v);
        volume = v;
    }

    bool IsLoaded() const { return loaded; }
    bool IsPlaying() const { return playing; }

private:
    AudioManager() : initialized(false), loaded(false), playing(false), volume(1.0f) {}
    ~AudioManager() {
        if (loaded) {
            StopMusicStream(music);
            UnloadMusicStream(music);
            loaded = false;
        }
        if (initialized) {
            CloseAudioDevice();
            initialized = false;
        }
    }

    Music music{};
    bool initialized;
    bool loaded;
    bool playing;
    float volume;
};

