#include "game.h"
#include "gui_keybind_menu.h"
#include "types.h"

#define GRID_X 20
#define GRID_Y 15
#define TILESIZE 32
#define UNITCOUNT 1

struct {
    u32 Units[GRID_X][GRID_Y];
    u32 Ground[GRID_X][GRID_Y];
} grid;

struct {
    v2u Pos;
} player;

struct {
} data;

i32 main() {
    InitGameWindow(&window);
    SetGameWindow(&window);

    OrbitCamera2D cam = NewOrbitCamera2D();

    KeybindMenuState keybindMenuState = NewKeybindMenu();

    f32 delta;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_R)) SetGameWindow(&window);
        if (IsKeyPressed(KEY_F11) || (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)))
            ToggleFullscreen();

        delta = GetFrameTime();

        // ===== CAMERA CONTROLS =====
        f32dUpdate(&cam.X, IsActionDown(ACTION_CAM_LEFT)    ? cam.X._y - 500.0f
                           : IsActionDown(ACTION_CAM_RIGHT) ? cam.X._y + 500.0f
                                                            : cam.X._y);
        f32dUpdate(&cam.Y, IsActionDown(ACTION_CAM_UP)     ? cam.Y._y - 500.0f
                           : IsActionDown(ACTION_CAM_DOWN) ? cam.Y._y + 500.0f
                                                           : cam.Y._y);
        f32dUpdate(&cam.Zoom, IsActionDown(ACTION_CAM_ZOOM_IN)    ? cam.Zoom._y * 4.0f
                              : IsActionDown(ACTION_CAM_ZOOM_OUT) ? cam.Zoom._y * 0.25f
                                                                  : cam.Zoom._y);
        if (IsActionDown(ACTION_CAM_ZOOM_RESET)) cam.Zoom._y = 1.0f;

        cam.Cam.target = (v2){cam.X._y, cam.Y._y};
        cam.Cam.zoom   = cam.Zoom._y;
        cam.Cam.offset = (v2){window.screenWidth / 2, window.screenHeight / 2};

        // ===== PLAYER CONTROLS =====
        if (IsActionPressed(ACTION_LEFT)) player.Pos.x = MIN(player.Pos.x - 1, GRID_X - 1);
        if (IsActionPressed(ACTION_RIGHT)) player.Pos.x = (player.Pos.x + 1) % GRID_X;
        if (IsActionPressed(ACTION_UP)) player.Pos.y = MIN(player.Pos.y - 1, GRID_Y - 1);
        if (IsActionPressed(ACTION_DOWN)) player.Pos.y = (player.Pos.y + 1) % GRID_Y;

        // ===== DRAWING =====
        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode2D(cam.Cam);
        {
            // TERRAIN
            for (i32 x = 0; x < GRID_X; x++)
                for (i32 y = 0; y < GRID_Y; y++) {
                    if (grid.Ground[x][y] == 0)
                        DrawRectangle(x * TILESIZE, y * TILESIZE, TILESIZE, TILESIZE, BROWN);
                }

            // PLAYER
            DrawRectangle(player.Pos.x * TILESIZE, player.Pos.y * TILESIZE, TILESIZE, TILESIZE,
                          GREEN);
        }
        EndMode2D();

        // ===== GUI =====
        DrawFPS(10, 10);
        //KeybindMenu(&keybindMenuState);
        //RebindMenu(&keybindMenuState);

        EndDrawing();
    }

    CloseWindow();
}