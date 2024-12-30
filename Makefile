ifeq ($(OS),Windows_NT)
    PLATFORM = win
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = linux
    else ifeq ($(UNAME_S),Darwin)
        $(error macOS not supported)
    endif
endif

all: run-$(PLATFORM)

build-linux:
	gcc src/main.c -Iinclude -Llib -l"raylib.linux" -lm -ldl -lGL -lpthread -lX11 -o build/linux/motley -Wl,-rpath=lib/

run-linux: build-linux
	./build/linux/motley

build-win:
	gcc ./src/main.c -Iinclude/ -std=c11 -Llib/ -l"raylib.win" -lwinmm -lopengl32 -lgdi32 -o build/win/motley

run-win: build-win
	./build/win/motley

build-web:
	./tools/emsdk/emsdk_env.sh && emcc .\src\main.c -Wall -std=c11 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -sEXPORTED_RUNTIME_METHODS=ccall -sASYNCIFY -DPLATFORM_WEB -Iinclude/ -Llib -l"raylib.web" -sUSE_GLFW=3 --preload-file assets/@assets/ -o build\web\motley.html

run-web: build-web
	python -m http.server &
	sleep 2 && start http://localhost:8000
