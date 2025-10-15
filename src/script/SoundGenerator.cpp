#include "SoundGenerator.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

SoundGenerator::SoundGenerator(const std::string& name)
    : appName(name) {}

SoundGenerator::~SoundGenerator() {
    cleanup();
}

bool SoundGenerator::init() {
    // Ensure PulseAudio environment variables exist (important for daemons)
    if (!getenv("PULSE_SERVER")) setenv("PULSE_SERVER", ("unix:/run/user/" + std::to_string(getuid()) + "/pulse/native").c_str(), 1);
    if (!getenv("XDG_RUNTIME_DIR")) setenv("XDG_RUNTIME_DIR", ("/run/user/" + std::to_string(getuid())).c_str(), 1);

    // Identify application in PulseAudio and force Pulse as SDL audio driver
    setenv("PULSE_PROP_application.name", appName.c_str(), 1);
    setenv("PULSE_PROP_media.role", "alert", 1);
    setenv("SDL_AUDIODRIVER", "pulse", 1);

    // Initialize SDL audio subsystem
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "❌ SDL audio init failed: " << SDL_GetError() << "\n";
        return false;
    }

    // Open audio stream through SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "❌ SDL_mixer init failed: " << Mix_GetError() << "\n";
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    audioReady = true;
    return true;
}

int SoundGenerator::getLatestSinkInputId() {
    // Query PulseAudio for the most recent audio stream ID
    FILE* pipe = popen("pactl list short sink-inputs | awk '{print $1}' | tail -n 1", "r");
    if (!pipe) return -1;

    char buffer[16];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe)) result += buffer;
    pclose(pipe);

    try {
        return std::stoi(result);
    } catch (...) {
        return -1;
    }
}

void SoundGenerator::muteAllExcept(int keepId) {
    // Mute all other audio streams except our alert
    std::ostringstream cmd;
    cmd << "for id in $(pactl list short sink-inputs | awk '{print $1}'); do "
        << "if [ \"$id\" != \"" << keepId << "\" ]; then "
        << "pactl set-sink-input-mute $id 1; fi; done";
    system(cmd.str().c_str());
}

void SoundGenerator::restoreAll() {
    // Restore sound to all streams
    system("for id in $(pactl list short sink-inputs | awk '{print $1}'); do "
           "pactl set-sink-input-mute $id 0; done");
}

void SoundGenerator::play(const std::string& soundPath, int volumePercent) {
    if (!audioReady) return;

    // Set global system volume to desired level
    std::string volCmd = "pactl set-sink-volume @DEFAULT_SINK@ " +
                         std::to_string(volumePercent) + "% 2>/dev/null";
    system(volCmd.c_str());

    Mix_Music* music = Mix_LoadMUS(soundPath.c_str());
    if (!music) {
        std::cerr << "⚠️ Could not load sound: " << soundPath << " | " << Mix_GetError() << "\n";
        return;
    }

    // Start playing music once
    Mix_PlayMusic(music, 0);

    // Wait for PulseAudio to register the new stream before muting others
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    int myId = getLatestSinkInputId();
    if (myId != -1) muteAllExcept(myId);

    // Wait until playback finishes
    while (Mix_PlayingMusic()) SDL_Delay(50);

    Mix_FreeMusic(music);
    restoreAll();
}

void SoundGenerator::cleanup() {
    if (audioReady) {
        Mix_HaltMusic();
        Mix_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        audioReady = false;
    }
}
