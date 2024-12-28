cd /d %~dp0

REM gcc --shared -o build\win\types.dll .\src\types.c -Iinclude/ -Llib/ -lraylib -lwinmm -lopengl32 -lgdi32

REM gcc --shared -o build\win\input.dll .\src\input.c -Iinclude/ -Llib/ -lraylib -lwinmm -lopengl32 -lgdi32

gcc .\src\main.c -Iinclude/ -std=c11 -Llib/ -lraylib -lwinmm -lopengl32 -lgdi32 -o build\win\motley & REM -Lbuild/win -ltypes -linput 

if %errorlevel% neq 0 (
    exit /b %errorlevel%
) else (
    .\build\win\motley
    pause
)
