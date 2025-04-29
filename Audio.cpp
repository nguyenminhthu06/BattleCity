// Audio.cpp
#include "Audio.h"
#include <iostream>

Audio::Audio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer initialization error: " << Mix_GetError() << std::endl;
    }
}

Audio::~Audio() {
    // Free all sound effects
    for (auto& pair : soundEffects) {
        Mix_FreeChunk(pair.second);
    }
    soundEffects.clear();

    // Free music
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
    }

    Mix_CloseAudio();
}

// Music methods
bool Audio::loadMusic(const std::string& path) {
    backgroundMusic = Mix_LoadMUS(path.c_str());
    if (!backgroundMusic) {
        std::cerr << "Failed to load music: " << path << " - " << Mix_GetError() << std::endl;
        return false;
    }
    return true;
}

void Audio::playMusic(int loops) {
    if (backgroundMusic && !muted) {
        Mix_VolumeMusic(musicVolume);
        Mix_PlayMusic(backgroundMusic, loops);
    }
}

void Audio::stopMusic() {
    Mix_HaltMusic();
}

void Audio::pauseMusic() {
    Mix_PauseMusic();
}

void Audio::resumeMusic() {
    if (!muted) {
        Mix_ResumeMusic();
    }
}

void Audio::setMusicVolume(int volume) {
    musicVolume = volume;
    if (!muted) {
        Mix_VolumeMusic(musicVolume);
    }
}

// Sound effects methods
bool Audio::loadSound(const std::string& id, const std::string& path) {
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (!sound) {
        std::cerr << "Failed to load sound: " << path << " - " << Mix_GetError() << std::endl;
        return false;
    }
    soundEffects[id] = sound;
    return true;
}

void Audio::playSound(const std::string& id) {
    if (muted) return;

    auto it = soundEffects.find(id);
    if (it != soundEffects.end()) {
        Mix_VolumeChunk(it->second, sfxVolume);
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void Audio::stopAllSounds() {
    Mix_HaltChannel(-1);
}

void Audio::setSoundVolume(int volume) {
    sfxVolume = volume;
    if (!muted) {
        for (auto& pair : soundEffects) {
            Mix_VolumeChunk(pair.second, sfxVolume);
        }
    }
}

// Mute control
void Audio::toggleMute() {
    muted = !muted;
    if (muted) {
        Mix_VolumeMusic(0);
        Mix_Volume(-1, 0); // All channels
    } else {
        Mix_VolumeMusic(musicVolume);
        Mix_Volume(-1, sfxVolume); // All channels
    }
}

bool Audio::isMuted() const {
    return muted;
}
