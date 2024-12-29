build-linux:
	gcc src/main.c -Iinclude -Llib -lraylib -lm -ldl -lGL -lpthread -lX11 -o build/linux/motley -Wl,-rpath=lib/

run-linux: build-linux
	./build/linux/motley
