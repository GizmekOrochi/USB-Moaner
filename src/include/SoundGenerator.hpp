#pragma once
#include <string>

/**
 * @class SoundGenerator
 * @brief Handles sound playback and system audio control for USB alerts.
 *
 * The `SoundGenerator` class encapsulates all logic related to audio playback
 * in the USB Moaner daemon. It uses **SDL2_mixer** for decoding and playing
 * MP3 files and integrates with **PulseAudio** to ensure consistent sound
 * output even when running as a systemd user daemon.
 *
 * ## Responsibilities
 * - Initialize and shut down the SDL2 audio subsystem safely.
 * - Play a sound effect (MP3) asynchronously at a specified volume.
 * - Use PulseAudio (`pactl`) to:
 *   - Maximize system volume for the alert sound.
 *   - Mute other running audio streams during playback.
 *   - Restore all sound streams afterward.
 *
 * ## Design Notes
 * - The class automatically configures environment variables such as:
 *   - `PULSE_SERVER`
 *   - `XDG_RUNTIME_DIR`
 *   - `SDL_AUDIODRIVER`
 *   ensuring audio works inside a daemon context (no graphical session needed).
 * - All system-level interactions are handled through shell commands (`pactl`).
 * - Playback is blocking but lightweight; it runs in a detached thread from
 *   the `Notifier` to avoid freezing rendering.
 *
 * ## Typical Lifecycle
 * 1. Call `init()` once to set up SDL audio and PulseAudio.
 * 2. Call `play()` with the path to a sound file (e.g., `alert.mp3`).
 * 3. Call `cleanup()` during shutdown to release resources.
 *
 * ## Dependencies
 * - SDL2 (`libsdl2-2.0-0`)
 * - SDL2_mixer (`libsdl2-mixer-2.0-0`)
 * - PulseAudio (`pulseaudio-utils`)
 *
 * Example usage:
 * ```cpp
 * SoundGenerator sound("NotifierSound");
 * if (sound.init()) {
 *     sound.play("/opt/usb_moaner/alert.mp3", 100);
 * }
 * ```
 */
class SoundGenerator {
public:
    /// @brief Construct the sound generator with an optional application name.
    explicit SoundGenerator(const std::string& name = "NotifierSound");

    /// @brief Automatically stops and cleans up the audio subsystem on destruction.
    ~SoundGenerator();

    /**
     * @brief Initializes the audio subsystem and configures environment variables.
     * @return `true` if initialization succeeded, `false` otherwise.
     */
    bool init();

    /**
     * @brief Plays an audio file at the specified volume.
     *
     * Automatically mutes all other audio streams while playing and restores
     * them afterward. Safe to call from separate threads.
     *
     * @param soundPath      Absolute path to the MP3 file.
     * @param volumePercent  Playback volume (0–100).
     */
    void play(const std::string& soundPath, int volumePercent = 100);

    /// @brief Shuts down SDL audio and releases all resources.
    void cleanup();

private:
    std::string appName;  ///< Name used to identify the app in PulseAudio.
    bool audioReady = false; ///< Indicates if audio was successfully initialized.

    /// @brief Retrieve the latest active PulseAudio stream ID.
    int getLatestSinkInputId();

    /// @brief Mute all other audio streams except this app’s.
    void muteAllExcept(int keepId);

    /// @brief Restore volume for all previously muted streams.
    void restoreAll();
};
