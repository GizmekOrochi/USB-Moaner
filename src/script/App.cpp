#include "../include/App.hpp"
#include "../include/UsbMonitor.hpp"
#include "../include/Notifier.hpp"
#include "../include/UsbEvent.hpp"

void App::run() {
    UsbMonitor monitor;
    Notifier notifier;

    monitor.startMonitoring([&notifier](const UsbEvent& event) {
        notifier.showMessage("Anime Girl Moaning noices", event.toString());
    });
}
