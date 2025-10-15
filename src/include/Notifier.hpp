#pragma once
#include <string>

/**
 * @class Notifier
 * @brief Displays a fullscreen visual alert and plays a sound when a USB device is detected.
 *
 * The `Notifier` is the **presentation layer** of the USB Moaner system.
 * It combines graphical display (via SDL2 and SDL2_image) with audio playback
 * (via `SoundGenerator`) to create an immediate, visible and audible response
 * to USB plug-in events.
 *
 * ## Responsibilities
 * - Initialize and configure SDL video subsystems in both GUI and daemon contexts.
 * - Display a fullscreen, borderless window (simulating a modal overlay).
 * - Render an image background, or a fallback color if missing.
 * - Play an audio clip (via `SoundGenerator`) simultaneously.
 * - Apply a smooth fade-out animation over time.
 *
 * ## Design Notes
 * - Works both when executed from a desktop session and as a **systemd user daemon**.
 * - Automatically sets environment variables (`DISPLAY`, `XDG_RUNTIME_DIR`, `PULSE_SERVER`)
 *   to ensure compatibility when launched in headless environments.
 * - Uses `std::filesystem` to detect correct asset paths:
 *   - Development mode → `../resource/Layout/background.png`
 *   - Installed daemon → `/opt/usb_moaner/background.png`
 * - The fade animation uses `SDL_SetTextureAlphaMod()` for performance and simplicity.
 *
 * ## Lifecycle
 * 1. `showMessage()` initializes SDL and creates a fullscreen renderer.
 * 2. Loads the background image and displays it.
 * 3. Spawns a thread to play the alert sound.
 * 4. Gradually fades the screen to black.
 * 5. Cleans up all SDL resources and exits gracefully.
 *
 * ## Dependencies
 * - SDL2 (`libsdl2-2.0-0`)
 * - SDL2_image (`libsdl2-image-2.0-0`)
 * - SDL2_mixer (`libsdl2-mixer-2.0-0`)
 * - `SoundGenerator` for audio control.
 * - `Config.hpp` for timing and resource path settings.
 *
 * ## Example
 * ```cpp
 * Notifier notifier;
 * notifier.showMessage("USB Connected", "New device detected!");
 * ```
 */
class Notifier {
public:
    /**
     * @brief Displays a fullscreen alert window and plays a sound.
     * 
     * This function blocks execution while rendering the fade animation.
     * It is designed to be triggered by the USB monitoring loop whenever
     * a new device is connected.
     *
     * @param title   Window title (not visible in fullscreen mode).
     * @param message Optional descriptive message for logs or overlays.
     */
    void showMessage(const std::string& title, const std::string& message) const;
};
