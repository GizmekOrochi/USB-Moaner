#pragma once
#include "UsbMonitor.hpp"
#include "Notifier.hpp"

/**
 * @class App
 * @brief Core application controller coordinating the USB monitor and notifier.
 *
 * The `App` class represents the main runtime logic of the program.  
 * It wires together the hardware event listener (`UsbMonitor`) and the
 * visual/audio alert system (`Notifier`) into a simple reactive loop.
 *
 * ## Responsibilities
 * - Instantiate and manage the `UsbMonitor` and `Notifier` components.
 * - Listen for USB device plug events and trigger the notification display.
 * - Serve as the single entry point for the application (`main.cpp` simply calls `app.run()`).
 *
 * ## Dependencies
 * - `UsbMonitor` (libudev backend)
 * - `Notifier` (SDL2-based display and sound)
 */
class App {
public:
    /// @brief Starts the application event loop.
    void run();
};
