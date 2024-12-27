#include "game.h"
#include "gui/new_tilemap.h"
#include "tilemap.h"
#include "types.h"

#define MAX_TILEMAPS 16

i32 main() {
    WindowOptions window = {1280, 720, 0, "MAICENA", 0};  // FLAG_MSAA_4X_HINT};

    InitGameWindow(&window);
    SetGameWindow(&window);
    InitTilemapShaders();

    Camera2D cam = {.zoom = 1.0f};

    NewTilemapMenuState newTmMenu              = InitNewTilemapMenu();
    Tilemap*            tilemaps[MAX_TILEMAPS] = {};
    u32                 tilemapsCount          = 0;

    TilemapCursor cursor = {.FG = 1, .BG = 0, .Box = {}, .State = CURSOR_STATE_SINGLE};

    f32 delta;
    while (!WindowShouldClose()) {
        delta = GetFrameTime();

        WindowControls(&window);
        if (!newTmMenu.NewTilemapWindowActive) SimpleCameraControls(&cam, window);

        if (!newTmMenu.NewTilemapWindowActive && IsKeyPressed(KEY_PERIOD))
            newTmMenu = InitNewTilemapMenu();

        if (tilemapsCount) UpdateTileCursor(tilemaps[cursor.CurrentTilemap], &cursor, cam);
        if (tilemapsCount && IsKeyPressed(KEY_Y))
            SaveTilemap(tilemaps[cursor.CurrentTilemap], "tilemap.txt");

        BeginDrawing();
        ClearBackground((Color){76, 79, 105, 255});

        BeginMode2D(cam);
        for (u32 i = 0; i < tilemapsCount; i++) {
            DrawTilemap(tilemaps[i]);
        }

        if (cursor.InMap) DrawTileCursor(tilemaps[cursor.CurrentTilemap], &cursor);
        if (cursor.State == CURSOR_STATE_BOXING || cursor.State == CURSOR_STATE_BOXED)
            DrawBoxCursor(tilemaps[cursor.CurrentTilemap], &cursor);
        EndMode2D();

        if (tilemapsCount) DrawTilemapGui(tilemaps[cursor.CurrentTilemap], &cursor);

        if (newTmMenu.NewTilemapWindowActive) NewTilemapMenu(&newTmMenu);
        if (newTmMenu.createTilemap && tilemapsCount < MAX_TILEMAPS) {
            Tilemap newTilemap        = StartTilemapEditor(&newTmMenu);
            tilemaps[tilemapsCount++] = &newTilemap;
        }
        EndDrawing();
    }

    CloseWindow();
}