#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

class AudioManager {
public:
    static AudioManager& Get() {
        static AudioManager instance; // created on first use, destroyed automatically
        return instance;
    }

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // Initialize audio subsystem
    bool Init() {
        if (!initialized) {
            InitAudioDevice();
            initialized = IsAudioDeviceReady();
            if (!initialized) TraceLog(LOG_WARNING, "AudioManager: audio device not ready");
        }
        return initialized;
    }

    // Music API
    bool LoadMusic(const std::string& path) {
        if (!initialized && !Init()) return false;
        if (musicLoaded) {
            StopMusicStream(music);
            UnloadMusicStream(music);
            musicLoaded = false;
            musicPlaying = false;
        }
        music = LoadMusicStream(path.c_str());
        musicLoaded = (music.ctxData != nullptr);
        if (!musicLoaded) TraceLog(LOG_WARNING, "AudioManager: failed to load music '%s' (working dir: %s)", path.c_str(), GetWorkingDirectory());
        return musicLoaded;
    }
    void PlayMusic() { if (musicLoaded) { PlayMusicStream(music); musicPlaying = true; } }
    void PauseMusic() { if (musicLoaded) PauseMusicStream(music); musicPlaying = false; }
    void ResumeMusic() { if (musicLoaded) ResumeMusicStream(music); musicPlaying = true; }
    void StopMusic() { if (musicLoaded) { StopMusicStream(music); musicPlaying = false; } }
    void UpdateMusic() {
        if (!musicLoaded) return;
        UpdateMusicStream(music);
        float played = GetMusicTimePlayed(music);
        float length = GetMusicTimeLength(music);
        if (length > 0.0f && played >= length) {
            PlayMusicStream(music);
            musicPlaying = true;
        }
    }
    // Call the global raylib function explicitly (avoid calling this class method recursively)
    void SetMusicVolume(float v) { if (musicLoaded) ::SetMusicVolume(music, v); musicVolume = v; }
    bool IsMusicLoaded() const { return musicLoaded; }
    bool IsMusicPlaying() const { return musicPlaying; }

    // Sound effects API
    bool LoadSoundEffect(const std::string& key, const std::string& path) {
        if (!initialized && !Init()) return false;
        // If previously loaded under same key, unload first
        auto it = sounds.find(key);
        if (it != sounds.end()) {
            UnloadSound(it->second);
            sounds.erase(it);
        }
        Sound s = LoadSound(path.c_str());
        // store sound even if loading failed; PlaySound will no-op if invalid
        sounds.emplace(key, s);
        return true;
    }

    void PlaySoundEffect(const std::string& key) {
        auto it = sounds.find(key);
        if (it != sounds.end()) PlaySound(it->second);
    }

    void UnloadAllSounds() {
        for (auto& p : sounds) UnloadSound(p.second);
        sounds.clear();
    }

    // Cleanup handled in destructor
    void SetMasterVolume(float v) {
        masterVolume = v;
        ::SetMasterVolume(masterVolume); // call global raylib function explicitly
    }

private:
    AudioManager() : initialized(false), musicLoaded(false), musicPlaying(false), musicVolume(1.0f), masterVolume(1.0f) {}
    ~AudioManager() {
        if (musicLoaded) {
            StopMusicStream(music);
            UnloadMusicStream(music);
            musicLoaded = false;
        }
        UnloadAllSounds();
        if (initialized) {
            CloseAudioDevice();
            initialized = false;
        }
    }

    Music music{};
    bool initialized;
    bool musicLoaded;
    bool musicPlaying;
    float musicVolume;
    float masterVolume;

    std::unordered_map<std::string, Sound> sounds;
};
