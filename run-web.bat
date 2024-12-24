cd /d %~dp0

emcc .\src\main.c -Wall -std=c11 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -O3 -sEXPORTED_RUNTIME_METHODS=ccall -sASYNCIFY -DPLATFORM_WEB -Iinclude/ -Llib -l"raylib.web" -sUSE_GLFW=3 --preload-file assets/@assets/ -o build\web\maicena.html

if %errorlevel% neq 0 (
    exit /b %errorlevel%
)