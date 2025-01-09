// #include "flappy.h"
#include "motley.h"
#include "shared.h"

#ifdef PLATFORM_WEB
#include <emscripten.h>
#endif

WindowOptions window = {.screenWidth  = 800,
                        .screenHeight = 640,
                        .targetFPS    = 0,
                        .title        = "Motley",
                        .ConfigFlags  = FLAG_MSAA_4X_HINT,
                        .WindowFlags  = FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT,
                        .iconPath     = "",
                        .guiStylePath = ""};

void Init() {
    InitGameWindow(&window);
    InitMotley();
    // InitFlappy();
}

void Update() {
    GameWindowControls(&window);
    UpdateMotley();
    // UpdateFlappy();
}

void Close() {
    CloseWindow();
    CloseMotley();
    // CloseFlappy();
}

i32 main() {
    Init();
#ifdef PLATFORM_WEB
    emscripten_set_main_loop(Update, window.targetFPS, true);
#else
    while (!WindowShouldClose()) Update();
#endif
    Close();
}
