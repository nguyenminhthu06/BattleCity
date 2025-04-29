// Audio.h
#pragma once
#include <SDL_mixer.h>
#include <string>
#include <unordered_map>

class Audio {
public:
    Audio();
    ~Audio();

    // Music management
    bool loadMusic(const std::string& path);
    void playMusic(int loops = -1);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(int volume); // 0-128

    // Sound effects management
    bool loadSound(const std::string& id, const std::string& path);
    void playSound(const std::string& id);
    void stopAllSounds();
    void setSoundVolume(int volume); // 0-128

    // State management
    void toggleMute();
    bool isMuted() const;

private:
    Mix_Music* backgroundMusic = nullptr;
    std::unordered_map<std::string, Mix_Chunk*> soundEffects;
    int musicVolume = 64; // Default 50%
    int sfxVolume = 128;  // Default 100%
    bool muted = false;
};
