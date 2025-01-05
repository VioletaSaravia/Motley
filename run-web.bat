cd /d %~dp0

./tools/emsdk/emsdk_env.ps1

emcc .\src\main.c -DPLATFORM_WEB -s USE_GLFW=3 -Iinclude/ -Llib -l"raylib.web" --preload-file assets/@assets/ --preload-file src/shaders/@src/shaders/ -o build\web\index.html

python -m http.server

if %errorlevel% neq 0 (
    exit /b %errorlevel%
)