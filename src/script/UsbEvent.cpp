#include "../include/UsbEvent.hpp"

// Simple value constructor: initializes all fields.
UsbEvent::UsbEvent(const std::string& action,
                   const std::string& vendor,
                   const std::string& product,
                   const std::string& devnode)
    : action(action), vendor(vendor), product(product), devnode(devnode) {}

// Returns a human-readable representation of the event.
std::string UsbEvent::toString() const {
    return "Action: " + action +
           "\nVendor: " + vendor +
           "\nProduct: " + product +
           "\nNode: " + devnode;
}
