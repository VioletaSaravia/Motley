cd /d %~dp0

gcc .\src\main.c -Iinclude/ -std=c11 -Llib/ -l"raylib.win" -lwinmm -lopengl32 -lgdi32 -o build\win\motley

if %errorlevel% neq 0 (
    exit /b %errorlevel%
) else (
    .\build\win\motley
    pause
)
