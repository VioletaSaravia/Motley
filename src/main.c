#include "game.h"
#include "gui/cursor.h"
#include "gui/popup_load.h"
#include "gui/tilemap.h"
#include "gui/toolbar.h"
#include "types.h"

/*
FIXME:
- Optimize tile drawing shader
- [X] Fix layout error in color picker
- [X] Fix layout error in tileset
- [X] Fix layout error in tile cursor
- [X] Add blank to colors GUI

TODO:
- Undo
- [2/4] Copy/Cut/Paste Regions
- Layer GUI
- Palette picker
*/

static WindowOptions window = {.screenWidth  = 1024,
                               .screenHeight = 768,
                               .targetFPS    = 0,
                               .title        = "Motley",
                               .ConfigFlags  = 0,
                               .WindowFlags  = FLAG_WINDOW_RESIZABLE,
                               .iconPath     = "",
                               .guiStylePath = ""};

#ifndef MAX_TILEMAPS
#define MAX_TILEMAPS 8
#endif

typedef struct {
    ToolbarState  toolbar;
    TilesetState  tileset;
    TilemapCursor cursor;

    PopupNewState  popupNew;
    PopupLoadState popupLoad;

    Arena   tilemapArena;
    Tilemap tilemap[MAX_TILEMAPS];
    u8      tilemapCount;

    Arena undoArena;  // TODO: Undo

    Texture bg;
    Shader  shaders[5];
} Motley;

void SetBackgroundColor(Motley* Motley, v3 fg, v3 bg) {
    SetShaderValue(Motley->shaders[4], GetShaderLocation(Motley->shaders[4], "color1"), &fg,
                   SHADER_UNIFORM_VEC3);
    SetShaderValue(Motley->shaders[4], GetShaderLocation(Motley->shaders[4], "color2"), &bg,
                   SHADER_UNIFORM_VEC3);
}

i32 main() {
    InitGameWindow(&window);

    // === INIT MOTLEY ===
    Motley Motley = {
        .toolbar = InitToolbar((v2){GetScreenWidth() * 0.025f, GetScreenHeight() * 0.05f}),
        .tileset = InitTilesetWindow((v2){window.screenWidth * 0.025f, window.screenHeight * 0.6f}),
        .cursor  = (TilemapCursor){.FG = 2, .BG = 0},
        .popupNew     = InitNewTilemapMenu(),
        .popupLoad    = InitPopupLoad(),
        .tilemapArena = InitArena(250 << 10),
        .undoArena    = InitArena(128 << 8),
        .bg           = LoadTexture("assets/bg.png"),
        .shaders      = {
            LoadShader("src/shaders/tiles.vert", "src/shaders/tile_fg.frag"),
            LoadShader("src/shaders/tiles.vert", "src/shaders/tile_fg_cursor.frag"),
            LoadShader("src/shaders/tiles.vert", "src/shaders/tile_bg.frag"),
            LoadShader("src/shaders/tiles.vert", "src/shaders/tile_bg_cursor.frag"),
            LoadShader("src/shaders/tiles.vert", "src/shaders/wallpaper.frag"),
        }};
    SetBackgroundColor(&Motley, (v3){0.34, 0.34, 0.34}, (v3){0.55, 0.55, 0.55});

    while (!WindowShouldClose()) {
        GameWindowControls(&window);

        // === UPDATE MOTLEY ===
        for (u32 i = 0; i <= Motley.tilemapCount; i++) {
            UpdateTileCursor(&Motley.tilemap[i], &Motley.cursor, &Motley.toolbar);
            UpdateBoxSelect(&Motley.tilemap[i], NULL, &Motley.cursor);
        }
        UpdateToolbar(&Motley.toolbar);
        UpdateTileset(&Motley.tileset, Motley.tilemap);
        SaveTilemap(Motley.tilemap, "tilemap.txt");  // TODO: Save popup

        BeginDrawing();

        // === DRAW MOTLEY ===
        DrawBg(Motley.bg, Motley.shaders[4]);

        for (u32 i = 0; i <= Motley.tilemapCount; i++) {
            DrawTilemap(&Motley.tilemap[i], (Shader[2]){Motley.shaders[0], Motley.shaders[2]});
            DrawTileCursor(&Motley.tilemap[i], &Motley.cursor, &Motley.toolbar,
                           (Shader[2]){Motley.shaders[1], Motley.shaders[3]});
            DrawBoxCursor(&Motley.tilemap[i], &Motley.cursor);
        }

        DrawToolbar(&Motley.toolbar, &Motley.popupNew, &Motley.popupLoad, Motley.tilemap,
                    &Motley.cursor);
        DrawTileset(&Motley.tileset, Motley.tilemap, &Motley.cursor,
                    (Shader[2]){Motley.shaders[0], Motley.shaders[2]});

        if (DrawNewTilemapMenu(&Motley.popupNew)) {
            Motley.tilemap[Motley.tilemapCount++] =
                InitTilemap(&Motley.popupNew, &Motley.tilemapArena);
        }
        if (DrawPopupLoad(&Motley.popupLoad)) {
            Motley.tilemap[Motley.tilemapCount++] =
                LoadTilemap(&Motley.popupLoad, &Motley.tilemapArena);
        }

        EndDrawing();
    }

    free(Motley.tilemapArena.buffer);
    free(Motley.undoArena.buffer);
    CloseWindow();
}
