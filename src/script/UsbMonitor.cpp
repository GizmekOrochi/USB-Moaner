#include "../include/UsbMonitor.hpp"
#include <libudev.h>
#include <iostream>
#include <sys/select.h>

UsbMonitor::UsbMonitor() {
    // Initialize udev context and setup USB device monitoring.
    udev = udev_new();
    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", "usb_device");
    udev_monitor_enable_receiving(mon);
}

UsbMonitor::~UsbMonitor() {
    // Clean up resources allocated by libudev.
    udev_monitor_unref(mon);
    udev_unref(udev);
}

void UsbMonitor::startMonitoring(const Callback& onEvent) {
    // Listen for USB plug/unplug events indefinitely.
    int fd = udev_monitor_get_fd(mon);

    while (true) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        int ret = select(fd + 1, &fds, NULL, NULL, NULL);
        if (ret > 0 && FD_ISSET(fd, &fds)) {
            struct udev_device* dev = udev_monitor_receive_device(mon);
            if (dev) {
                const char* action = udev_device_get_action(dev);
                const char* vendor = udev_device_get_sysattr_value(dev, "idVendor");
                const char* product = udev_device_get_sysattr_value(dev, "idProduct");
                const char* devnode = udev_device_get_devnode(dev);

                // Only trigger when a new USB device is added.
                if (action && std::string(action) == "add") {
                    UsbEvent event(
                        action ? action : "unknown",
                        vendor ? vendor : "unknown",
                        product ? product : "unknown",
                        devnode ? devnode : "unknown"
                    );
                    onEvent(event);
                }

                udev_device_unref(dev);
            }
        }
    }
}
