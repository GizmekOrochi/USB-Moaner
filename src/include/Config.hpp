#pragma once
#include <string>
#include <filesystem>

/**
 * @file Config.hpp
 * @brief Central configuration header for runtime parameters and resource paths.
 *
 * This header defines global constants and helper functions used throughout the project.
 * It contains no logic beyond returning absolute or fallback paths for resources.
 *
 * ## Design Goals
 * - Provide a **single source of truth** for tunable constants (volume, fade speed, etc.).
 * - Support both **development** and **installed** environments automatically.
 *   - In dev mode, assets are loaded from `../resource/...`.
 *   - In installed mode (daemon), assets are loaded from `/opt/usb_moaner/`.
 * - Avoid magic numbers scattered across code by grouping configuration logically.
 *
 * ## Namespaces
 * - **AudioConfig**: audio volume and sound path management.
 * - **FadeConfig**: fade animation timing parameters.
 * - **DisplayConfig**: background image resource handling.
 *
 * ## Dependencies
 * - Requires C++17 `<filesystem>` for path existence checks.
 * - Header-only: no .cpp file.
 */

namespace fs = std::filesystem;

/**
 * @namespace AudioConfig
 * @brief Configuration values and helpers for sound playback.
 */
namespace AudioConfig {
    /// Default playback volume (0–100).
    constexpr int VOLUME_PERCENT = 100;

    /**
     * @brief Resolve the full path to the alert sound file.
     * 
     * Prefers the installed path `/opt/usb_moaner/Effect.mp3`.
     * Falls back to `../resource/FX/Effect.mp3` when running in dev mode.
     */
    inline std::string getSoundPath() {
        std::string installPath = "/opt/usb_moaner/Effect.mp3";
        if (fs::exists(installPath)) return installPath;
        return "../resource/FX/Effect.mp3";
    }
}

/**
 * @namespace FadeConfig
 * @brief Timing configuration for the fade-out animation.
 */
namespace FadeConfig {
    /// Delay (in milliseconds) before the fade starts.
    constexpr int DELAY_BEFORE_FADE_MS = 100;
    /// Fade intensity step per frame (lower = slower fade).
    constexpr int FADE_SPEED = 5;
    /// Delay between frames in milliseconds (~16ms = 60 FPS).
    constexpr int FRAME_DELAY_MS = 16;
}

/**
 * @namespace DisplayConfig
 * @brief Configuration values and helpers for image rendering.
 */
namespace DisplayConfig {
    /**
     * @brief Resolve the full path to the background image file.
     * 
     * Prefers the installed path `/opt/usb_moaner/background.png`.
     * Falls back to `../resource/Layout/background.png` when running in dev mode.
     */
    inline std::string getBackgroundPath() {
        std::string installPath = "/opt/usb_moaner/background.png";
