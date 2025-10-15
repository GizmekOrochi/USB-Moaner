#pragma once
#include "UsbEvent.hpp"
#include <functional>

/**
 * @class UsbMonitor
 * @brief Continuously monitors the Linux system for USB plug/unplug events using libudev.
 *
 * This class serves as the **hardware event listener** for the entire application.
 * It interfaces directly with the Linux kernel's `udev` subsystem to detect
 * when USB devices are connected or disconnected.
 *
 * ## Responsibilities
 * - Initialize and configure a `udev_monitor` to listen for `usb_device` events.
 * - Block on the `udev` file descriptor using `select()` until an event occurs.
 * - Parse the event’s metadata (action, vendor ID, product ID, device node).
 * - Construct a `UsbEvent` object and forward it to the provided callback.
 *
 * ## Design Notes
 * - Uses **RAII** for `udev` and `udev_monitor` cleanup.
 * - Operates in a **blocking loop**; this method is meant to run in the main thread.
 * - Only triggers callbacks for `"add"` (device plugged in) actions.
 * - Encapsulates all direct libudev interactions, isolating low-level details
 *   from higher-level application logic (`App` and `Notifier`).
 *
 * ## Usage Example
 * ```cpp
 * UsbMonitor monitor;
 * monitor.startMonitoring([](const UsbEvent& e) {
 *     std::cout << e.toString() << std::endl;
 * });
 * ```
 *
 * ## Dependencies
 * - libudev (Linux device manager)
 * - Standard C headers: `<sys/select.h>` for polling file descriptors
 */
class UsbMonitor {
public:
    /// Type alias for the event callback function.
    using Callback = std::function<void(const UsbEvent&)>;

    /**
     * @brief Constructs and initializes the udev monitoring context.
     * 
     * Prepares the libudev monitor to capture `usb_device` events.
     */
    UsbMonitor();

    /**
     * @brief Releases all libudev resources.
     *
     * Called automatically when the monitor object is destroyed.
     */
    ~UsbMonitor();

    /**
     * @brief Begins continuous USB event monitoring.
     *
     * Blocks the calling thread, waiting for incoming events.
     * Each time a new USB device is added, the provided callback is executed.
     *
     * @param onEvent Function to call when a USB event is detected.
     */
    void startMonitoring(const Callback& onEvent);

private:
    struct udev* udev;              ///< Pointer to the libudev context.
    struct udev_monitor* mon;       ///< Pointer to the active udev monitor.
};
