cd /d %~dp0

gcc .\src\main.c -Iinclude/ -Llib/ -lraylib -lwinmm -lopengl32 -lgdi32 -o build\win\maicena

if %errorlevel% neq 0 (
    exit /b %errorlevel%
) else (
    .\build\win\maicena
    pause
)
