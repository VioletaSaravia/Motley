#include "shared.h"

#ifdef MOTLEY
#include "motley.h"
#endif

#ifdef PLATFORM_WEB
#include <emscripten.h>
#endif

static WindowOptions window = {.screenWidth  = 1024,
                               .screenHeight = 768,
                               .targetFPS    = 0,
                               .title        = "Motley",
                               .ConfigFlags  = 0,
                               .WindowFlags  = FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT,
                               .iconPath     = "",
                               .guiStylePath = ""};

void Init() {
    InitGameWindow(&window);
    InitMotley();
}

void Update() {
    GameWindowControls(&window);
    UpdateMotley();
}

void Close() {
    CloseMotley();
    CloseWindow();
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
