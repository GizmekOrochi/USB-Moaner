# 🧩 USB Moaner

**USB Moaner** is a playful Linux daemon that reacts whenever a new USB device is connected.  

---

# 💡 USB Moaner

To install, just run `./src/install.sh`

---

## ⚙️ Features

- Plays a custom MP3 alert when a USB device is plugged in  
- Displays a fullscreen borderless image overlay  
- Runs automatically at every system start (via `systemd --user`)  

---

## 🖥️ Requirements

Works on: **Ubuntu**, **Debian**, **Mint**, **Arch**, **Fedora**, **Manjaro**

Dependencies (installed automatically by `install.sh` in src):

```bash
libsdl2-2.0-0
libsdl2-image-2.0-0
libsdl2-mixer-2.0-0
libudev1
pulseaudio-utils
