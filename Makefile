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
	$(CC) src/main.c $(CFLAGS) -Llib -l"raylib.linux" -lm -ldl -lGL -lpthread -lX11 -o build/linux/motley -Wl,-rpath=lib/

run-linux: build-linux
	./build/linux/motley

build-win:
	$(CC) ./src/main.c $(CFLAGS) -Llib -l"raylib.win" -lwinmm -lopengl32 -lgdi32 -o build/win/motley

run-win: build-win
	./build/win/motley

build-web:
	./tools/emsdk/emsdk_env.sh && $(WEB_CC) .\src\main.c $(CFLAGS) -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -sEXPORTED_RUNTIME_METHODS=ccall -sASYNCIFY -DPLATFORM_WEB -Llib -l"raylib.web" -sUSE_GLFW=3 --preload-file assets/@assets/ -o build\web\motley.html

run-web: build-web
	cd build/web && python -m http.server 8000 &
	sleep 2 && start http://localhost:8000

clean:
	rm -rf build && mkdir build

setup: clean
	git submodule update --init --recursive

.PHONY: all linux-debug linux-release windows-debug windows-release android-debug android-release web-debug web-release clean setup
