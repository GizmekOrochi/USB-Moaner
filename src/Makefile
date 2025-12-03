BIN_DIR = ../bin
BIN = $(BIN_DIR)/usb_moaner

all: $(BIN)

$(BIN): main.cpp script/*.cpp
	mkdir -p $(BIN_DIR)
	g++ script/*.cpp main.cpp -Iinclude -ludev \
		$(shell pkg-config --cflags --libs sdl2 SDL2_image SDL2_mixer) \
		-o $(BIN)

run: $(BIN)
	$(BIN)

