#pragma once
#include <string>

/**
 * @class UsbEvent
 * @brief Represents a high-level description of a USB device event.
 *
 * This class acts as a lightweight data container for USB plug/unplug events
 * detected by the system via **libudev**.
 *
 * ## Purpose
 * - Abstracts low-level udev device information into a readable form.
 * - Provides a uniform way for other modules (like `Notifier`) to access
 *   event metadata such as the device vendor, product ID, and node path.
 *
 * ## Design Notes
 * - Instances of this class are created by `UsbMonitor` whenever a new
 *   USB action is detected.
 * - This class is **immutable** after construction — its members are
 *   publicly accessible for convenience (since it’s a simple data struct).
 * - `toString()` is provided to format the event into a human-readable string,
 *   useful for logging or on-screen display.
 */
class UsbEvent {
public:
    /// The action performed on the device (e.g., "add", "remove").
    std::string action;

    /// Vendor ID of the device, as reported by udev (e.g., "046d").
    std::string vendor;

    /// Product ID of the device, as reported by udev (e.g., "c534").
    std::string product;

    /// The system device node (e.g., "/dev/bus/usb/001/004").
    std::string devnode;

    /**
     * @brief Constructs a new UsbEvent with all device details.
     * @param action  The type of event ("add" or "remove").
     * @param vendor  The device vendor ID string.
     * @param product The device product ID string.
     * @param devnode The full device node path.
     */
    UsbEvent(const std::string& action,
             const std::string& vendor,
             const std::string& product,
             const std::string& devnode);

    /**
     * @brief Returns a formatted string containing all event details.
     * @return A human-readable multi-line string describing the USB event.
     */
    std::string toString() const;
};
