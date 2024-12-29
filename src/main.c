#include "game.h"
#include "gui/cursor.h"
#include "gui/popup_load.h"
#include "gui/tilemap.h"
#include "gui/toolbar.h"
#include "types.h"

i32 main() {
    WindowOptions window = {.screenWidth  = 1280,
                            .screenHeight = 720,
                            .targetFPS    = 0,
                            .title        = "Motley",
                            .ConfigFlags  = 0,  // FLAG_MSAA_4X_HINT
                            .WindowFlags  = FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI,
                            .iconPath     = "",
                            .guiStylePath = ""};
    InitGameWindow(&window);
    SetGameWindow(&window);
    InitTilemapShaders();
    Texture bg = LoadTexture("assets/bg.png");

    PopupNewState  popupNew  = InitNewTilemapMenu();
    PopupLoadState popupLoad = InitPopupLoad();

    ToolbarState toolbar =
        InitToolbar((v2){window.screenWidth * 0.025f, window.screenHeight * 0.05f});
    TilesetState tileset =
        InitTilesetWindow((v2){window.screenWidth * 0.025f, window.screenHeight * 0.6f});
    TilemapCursor cursor = {.FG = 1, .BG = 0};

    Arena   tilemapArena = InitArena(250 << 10);
    Tilemap tilemap      = {};

    f32 delta;
    while (!WindowShouldClose()) {
        delta = GetFrameTime();

        GameWindowControls(&window);

        UpdateTileCursor(&tilemap, &cursor, &toolbar);
        UpdateToolbar(&toolbar);
        UpdateTileset(&tileset, &tilemap);
        SaveTilemap(&tilemap, "tilemap.txt");  // TODO: Save popup

        BeginDrawing();
        {
            DrawBg(bg);

            DrawTilemap(&tilemap);
            DrawTileCursor(&tilemap, &cursor, &toolbar);
            DrawBoxCursor(&tilemap, &cursor);
            DrawToolbar(&toolbar, &popupNew, &popupLoad, &tilemap, &cursor);
            DrawTileset(&tileset, &tilemap, &cursor);
            if (DrawNewTilemapMenu(&popupNew)) {
                tilemap = InitTilemap(&popupNew, &tilemapArena);
            }
            if (DrawPopupLoad(&popupLoad)) {
                tilemap = InitTilemapFromFile(&popupLoad, &tilemapArena);
            }
        }
        EndDrawing();
    }

    CloseWindow();
}