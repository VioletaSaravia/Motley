#include "game.h"
#include "gui/cursor.h"
#include "gui/tilemap.h"
#include "gui/toolbar.h"
#include "types.h"

i32 main() {
    WindowOptions window = {.screenWidth  = 1280,
                            .screenHeight = 720,
                            .targetFPS    = 0,
                            .title        = "Motley",
                            .ConfigFlags  = 0,  // FLAG_MSAA_4X_HINT
                            .WindowFlags  = FLAG_WINDOW_RESIZABLE,
                            .iconPath     = ""};

    InitGameWindow(&window);
    SetGameWindow(&window);
    InitTilemapShaders();

    NewTilemapMenuState newTmMenu =
        InitNewTilemapMenu((v2){GetScreenWidth() * 0.5f - 140, GetScreenHeight() * 0.5f - 76});
    ToolbarState toolbar =
        InitToolbar((v2){window.screenWidth * 0.025f, window.screenHeight * 0.05f});
    TilesetState tileset =
        InitTilesetWindow((v2){window.screenWidth * 0.025f, window.screenHeight * 0.6f});
    TilemapCursor cursor = {.FG = 1, .BG = 0, .Box = {}, .State = CURSOR_STATE_SINGLE};

    Tilemap tilemap = {};

    f32 delta;
    while (!WindowShouldClose()) {
        delta = GetFrameTime();

        GameWindowControls(&window);

        UpdateTileCursor(&tilemap, &cursor, &toolbar);
        UpdateToolbar(&toolbar);
        UpdateTileset(&tileset, &tilemap);
        SaveTilemap(&tilemap, "tilemap.txt");

        BeginDrawing();
        {
            ClearBackground((Color){76, 79, 105, 255});

            DrawTilemap(&tilemap);
            DrawTileCursor(&tilemap, &cursor, &toolbar);
            DrawBoxCursor(&tilemap, &cursor);
            DrawToolbar(&toolbar, &newTmMenu, &tilemap, &cursor);
            DrawTileset(&tileset, &tilemap, &cursor);
            if (DrawNewTilemapMenu(&newTmMenu)) {
                tilemap = InitTilemap(&newTmMenu);
            }
        }
        EndDrawing();
    }

    CloseWindow();
}