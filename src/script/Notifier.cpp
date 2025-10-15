#include "../include/Notifier.hpp"
#include "../include/Config.hpp"
#include "../include/SoundGenerator.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

void Notifier::showMessage(const std::string& title, const std::string& message) const {
    // Ensure graphical and audio environment variables exist for systemd user services
    if (!getenv("DISPLAY")) setenv("DISPLAY", ":0", 1);
    if (!getenv("XDG_RUNTIME_DIR")) setenv("XDG_RUNTIME_DIR", ("/run/user/" + std::to_string(getuid())).c_str(), 1);
    if (!getenv("PULSE_SERVER")) setenv("PULSE_SERVER", ("unix:/run/user/" + std::to_string(getuid()) + "/pulse/native").c_str(), 1);

    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "❌ SDL video init failed: " << SDL_GetError() << "\n";
        return;
    }

    // Initialize SDL_image for PNG/JPG support
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        std::cerr << "❌ SDL_image init failed: " << IMG_GetError() << "\n";
        SDL_Quit();
        return;
    }

    // Load background image (auto-resolves installed vs dev path)
    std::string imgPath = DisplayConfig::getBackgroundPath();
    if (!fs::exists(imgPath)) {
        std::cerr << "⚠️ Background not found: " << imgPath << "\n";
    }

    // Create fullscreen borderless window
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    SDL_Window* window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        dm.w, dm.h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        std::cerr << "❌ SDL window creation failed: " << SDL_GetError() << "\n";
        IMG_Quit(); SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Try loading texture from background image
    SDL_Surface* surface = IMG_Load(imgPath.c_str());
    SDL_Texture* texture = nullptr;
    bool hasImage = false;

    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (texture) {
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            hasImage = true;
        } else {
            std::cerr << "⚠️ Failed to create texture from image: " << SDL_GetError() << "\n";
        }
    }

    // Display first frame (background or fallback color)
    SDL_RenderClear(renderer);
    if (hasImage)
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 90, 255);
        SDL_RenderFillRect(renderer, nullptr);
    }
    SDL_RenderPresent(renderer);

    // Launch sound playback in detached thread
    SoundGenerator sound("NotifierSound");
    if (sound.init()) {
        std::string soundPath = AudioConfig::getSoundPath();
        if (fs::exists(soundPath)) {
            std::thread([&sound, soundPath]() {
                sound.play(soundPath, AudioConfig::VOLUME_PERCENT);
            }).detach();
        } else {
            std::cerr << "⚠️ Sound file missing: " << soundPath << "\n";
        }
    } else {
        std::cerr << "⚠️ Sound system initialization failed.\n";
    }

    // Fade-out animation
    std::this_thread::sleep_for(std::chrono::milliseconds(FadeConfig::DELAY_BEFORE_FADE_MS));
    Uint
