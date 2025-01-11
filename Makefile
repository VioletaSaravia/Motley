PLATFORM = $(if $(filter Windows_NT,$(OS)),win,linux)

CC = gcc
CFLAGS = -O0 -Iinclude -Wall -Wextra #-std=c11
LDFLAGS = -Llib -l"raylib.$(PLATFORM)"

LINUX_CC = gcc
WINDOWS_CC = x86_64-w64-mingw32-gcc
ANDROID_CC = aarch64-linux-android-gcc
WEB_CC = emcc

all: run-$(PLATFORM)

build-linux:
	$(CC) src/main.c $(CFLAGS) -Llib -l"raylib.linux" -lm -ldl -lGL -lpthread -o build/linux/motley -Wl,-rpath=lib/

run-linux: build-linux
	./build/linux/motley

build-win:
	$(WINDOWS_CC) ./src/main.c $(CFLAGS) -Llib -l"raylib.win" -lwinmm -lopengl32 -lgdi32 -o build/win/motley

run-win: build-win
	./build/win/motley

build-web:
	./tools/emsdk/emsdk_env.sh && $(WEB_CC) ./src/main.c $(CFLAGS) -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -sEXPORTED_RUNTIME_METHODS=ccall -sASYNCIFY -DPLATFORM_WEB -Llib -l"raylib.web" -sUSE_GLFW=3 --preload-file assets/@assets/ --preload-file src/shaders/@src/shaders -o build/web/index.html

run-web: build-web
	cd build/web && python -m http.server 8000 &
	sleep 2 && start http://localhost:8000

clean:
	rm -rf build && mkdir build

setup: clean
	sudo apt update && sudo apt install build-essential git libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
	mkdir -p build/linux build/win build/web build/android
	git submodule update --init --recursive

.PHONY: all linux-debug linux-release windows-debug windows-release android-debug android-release web-debug web-release clean setup
