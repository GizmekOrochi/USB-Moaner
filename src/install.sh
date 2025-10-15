#!/bin/bash
set -e

APP_NAME="usb_moaner"
INSTALL_DIR="/opt/$APP_NAME"
SERVICE_DIR="$HOME/.config/systemd/user"
SERVICE_FILE="$SERVICE_DIR/$APP_NAME.service"
EXEC_PATH="$INSTALL_DIR/$APP_NAME"
ENV_FILE="$SERVICE_DIR/environment"
BINARY_PATH="../bin/$APP_NAME"

IMG_PATH="../resource/Layout/background.png"
SND_PATH="../resource/FX/Effect.mp3"

echo "🔧 Installing $APP_NAME as a user service..."

# 🧱 1️⃣ Install dependencies
echo "📦 Checking and installing dependencies..."
if [ -f /etc/debian_version ]; then
    sudo apt update -y
    sudo apt install -y \
        libsdl2-2.0-0 \
        libsdl2-image-2.0-0 \
        libsdl2-mixer-2.0-0 \
        libudev1 \
        pulseaudio-utils
elif [ -f /etc/redhat-release ]; then
    sudo dnf install -y SDL2 SDL2_image SDL2_mixer systemd-libs pulseaudio-utils
elif [ -f /etc/arch-release ]; then
    sudo pacman -Sy --noconfirm sdl2 sdl2_image sdl2_mixer systemd-libs pulseaudio-utils
else
    echo "⚠️ Unsupported distribution. Please manually install SDL2, SDL2_image, SDL2_mixer, libudev, and pulseaudio-utils."
fi

# ⏹️ 2️⃣ Stop any existing instance
if systemctl --user list-units | grep -q "$APP_NAME.service"; then
    echo "⏹️ Stopping existing $APP_NAME service..."
    systemctl --user stop "$APP_NAME.service" || true
fi

# 🧹 3️⃣ Remove old system service (if any)
if [ -f "/etc/systemd/system/$APP_NAME.service" ]; then
    echo "🧹 Removing old system service..."
    sudo systemctl stop "$APP_NAME" || true
    sudo systemctl disable "$APP_NAME" || true
    sudo rm -f "/etc/systemd/system/$APP_NAME.service"
    sudo systemctl daemon-reload
fi

# 🏗️ 4️⃣ Create install directory
echo "📂 Creating install directory at $INSTALL_DIR"
sudo mkdir -p "$INSTALL_DIR"

# 📦 5️⃣ Copy binary + resources
echo "📦 Installing binary and resources..."
if [ ! -f "$BINARY_PATH" ]; then
    echo "❌ Missing precompiled binary at $BINARY_PATH"
    exit 1
fi

sudo cp -f "$BINARY_PATH" "$INSTALL_DIR/"
if [ -f "$IMG_PATH" ]; then
    sudo cp -f "$IMG_PATH" "$INSTALL_DIR/background.png"
else
    echo "⚠️ Missing image: $IMG_PATH"
fi
if [ -f "$SND_PATH" ]; then
    sudo cp -f "$SND_PATH" "$INSTALL_DIR/Effect.mp3"
else
    echo "⚠️ Missing sound: $SND_PATH"
fi

sudo chmod +x "$EXEC_PATH"
sudo chown -R $USER:$USER "$INSTALL_DIR"
sudo chmod 755 "$INSTALL_DIR"
sudo chmod 644 "$INSTALL_DIR"/*.{png,mp3} 2>/dev/null || true

# 👤 6️⃣ Prepare user service directory
mkdir -p "$SERVICE_DIR"

# 🌍 7️⃣ Capture environment for GUI + sound
echo "🌍 Capturing graphical/audio environment..."
systemctl --user import-environment DISPLAY XAUTHORITY XDG_RUNTIME_DIR WAYLAND_DISPLAY PULSE_SERVER || true
systemctl --user show-environment > "$ENV_FILE"

# 🧾 8️⃣ Create systemd user service
echo "🧩 Creating systemd service at $SERVICE_FILE"
cat > "$SERVICE_FILE" <<EOF
[Unit]
Description=Device Alert (User Service)
After=graphical-session.target sound.target

[Service]
ExecStart=$EXEC_PATH
WorkingDirectory=$INSTALL_DIR
Restart=always
RestartSec=3
ExecStartPre=/bin/sleep 5

EnvironmentFile=-$ENV_FILE

[Install]
WantedBy=default.target
EOF

# 🔁 9️⃣ Reload and enable service
systemctl --user daemon-reload
systemctl --user enable "$APP_NAME.service"
systemctl --user restart "$APP_NAME.service"
loginctl enable-linger "$USER" || true

echo ""
echo "✅ Installation complete!"
echo "🚀 $APP_NAME service is now running and will auto-start at every login."
echo ""
echo "📂 Installed to: $INSTALL_DIR"
echo "💡 Manage with:"
echo "   systemctl --user status $APP_NAME"
echo "   systemctl --user restart $APP_NAME"
echo "   systemctl --user stop $APP_NAME"
echo ""
echo "🧠 Environment file saved at: $ENV_FILE"
