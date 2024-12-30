cd /d %~dp0

./tools/emsdk/emsdk_env.ps1

emcc .\src\main.c -sFULL_ES3 -Wall -std=c11 -DPLATFORM_WEB -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -sEXPORTED_RUNTIME_METHODS=ccall -sASYNCIFY -DPLATFORM_WEB -Iinclude/ -Llib -l"raylib.web" -sUSE_GLFW=3 --preload-file assets/@assets/ --preload-file src/shaders/@src/shaders/ -o build\web\motley.html

if %errorlevel% neq 0 (
    exit /b %errorlevel%
)