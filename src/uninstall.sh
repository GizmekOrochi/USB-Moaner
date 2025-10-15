#!/bin/bash
set -e

APP_NAME="usb_moaner"
INSTALL_DIR="/opt/$APP_NAME"
SERVICE_FILE="$HOME/.config/systemd/user/$APP_NAME.service"

echo "🧹 Uninstalling $APP_NAME..."

# 1️⃣ Stop the user service if running
if systemctl --user list-units | grep -q "$APP_NAME.service"; then
    echo "⏹️ Stopping user service..."
    systemctl --user stop "$APP_NAME.service" || true
    systemctl --user disable "$APP_NAME.service" || true
fi

# 2️⃣ Remove the user service file
if [ -f "$SERVICE_FILE" ]; then
    echo "🗑️ Removing service file: $SERVICE_FILE"
    rm -f "$SERVICE_FILE"
    systemctl --user daemon-reload
else
    echo "⚠️ No user service file found."
fi

# 3️⃣ Remove lingering configuration (optional)
if loginctl show-user "$USER" | grep -q "Linger=yes"; then
    echo "💤 Disabling lingering for user $USER (optional)..."
    loginctl disable-linger "$USER" || true
fi

# 4️⃣ Remove installation directory
if [ -d "$INSTALL_DIR" ]; then
    echo "🧽 Removing install directory: $INSTALL_DIR"
    sudo rm -rf "$INSTALL_DIR"
else
    echo "⚠️ No install directory found at $INSTALL_DIR."
fi

# 5️⃣ Reload user systemd daemon
systemctl --user daemon-reload

echo ""
echo "✅ $APP_NAME fully uninstalled."
echo "🧾 If you want to reinstall it later, just run ./install.sh again."
