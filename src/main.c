#include "game.h"
#include "gui/cursor.h"
#include "gui/tilemap.h"
#include "gui/toolbar.h"
#include "types.h"

#define MAX_TILEMAPS 16

i32 main() {
    WindowOptions window = {1280, 720, 0, "Motley", 0};  // FLAG_MSAA_4X_HINT};

    InitGameWindow(&window);
    SetGameWindow(&window);
    InitTilemapShaders();

    NewTilemapMenuState newTmMenu = InitNewTilemapMenu();
    ToolbarState        toolbar =
        InitToolbar((v2){window.screenWidth * 0.025f, window.screenHeight * 0.05f});
    TilesetState tileset =
        NewTilesetWindow((v2){window.screenWidth * 0.025f, window.screenHeight * 0.6f});

    Tilemap* tilemaps[MAX_TILEMAPS] = {};
    u32      tilemapsCount          = 0;

    TilemapCursor cursor = {.FG = 1, .BG = 0, .Box = {}, .State = CURSOR_STATE_SINGLE};

    f32 delta;
    while (!WindowShouldClose()) {
        delta = GetFrameTime();

        GameWindowControls(&window);

        // TODO Proper ZII TO remove all these if checks!
        if (tilemapsCount) UpdateTileCursor(tilemaps[cursor.CurrentTilemap], &cursor, &toolbar);
        UpdateToolbar(&toolbar);

        if (tilemapsCount && IsKeyPressed(KEY_Y))
            SaveTilemap(tilemaps[cursor.CurrentTilemap], "tilemap.txt");

        BeginDrawing();
        ClearBackground((Color){76, 79, 105, 255});

        for (u32 i = 0; i < tilemapsCount; i++) {
            DrawTilemap(tilemaps[i]);
        }

        if (cursor.InMap && cursor.State != CURSOR_STATE_BOXING)
            DrawTileCursor(tilemaps[cursor.CurrentTilemap], &cursor, &toolbar);
        if (cursor.State == CURSOR_STATE_BOXING || cursor.State == CURSOR_STATE_BOXED)
            DrawBoxCursor(tilemaps[cursor.CurrentTilemap], &cursor);

        Toolbar(&toolbar, &newTmMenu, tilemaps[cursor.CurrentTilemap], &cursor);

        if (tilemapsCount) DrawTileset(&tileset, tilemaps[cursor.CurrentTilemap], &cursor);

        if (newTmMenu.NewTilemapWindowActive) NewTilemapMenu(&newTmMenu);
        if (newTmMenu.createTilemap && tilemapsCount < MAX_TILEMAPS) {
            Tilemap newTilemap        = StartTilemapEditor(&newTmMenu, &window);
            tilemaps[tilemapsCount++] = &newTilemap;
        }
        EndDrawing();
    }

    CloseWindow();
}