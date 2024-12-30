#include "game.h"
#include "gui/cursor.h"
#include "gui/popup_load.h"
#include "gui/tilemap.h"
#include "gui/toolbar.h"
#include "types.h"

static WindowOptions window = {
    .screenWidth  = 1024,
    .screenHeight = 768, 
    .targetFPS    = 0,
    .title        = "Motley",
    .ConfigFlags  = 0,  // FLAG_MSAA_4X_HINT
    .WindowFlags  = FLAG_WINDOW_RESIZABLE,
    .iconPath     = "",
    .guiStylePath = ""
};

static struct {
    ToolbarState toolbar;
    TilesetState tileset;
    TilemapCursor cursor;

    PopupNewState popupNew;
    PopupLoadState popupLoad;

    Arena tilemapArena;
    Tilemap tilemap;

    Arena undoHistoryArena; // TODO:

    Texture bg;
} Motley = {};

void InitMotley() {
    InitTilemapShaders();
    Motley.bg = LoadTexture("assets/bg.png");

    Motley.popupNew  = InitNewTilemapMenu();
    Motley.popupLoad = InitPopupLoad();

    Motley.toolbar =
        InitToolbar((v2){GetScreenWidth() * 0.025f, GetScreenHeight() * 0.05f});
    Motley.tileset =
        InitTilesetWindow((v2){window.screenWidth * 0.025f, window.screenHeight * 0.6f});
    Motley.cursor = (TilemapCursor){.FG = 1, .BG = 0};

    Motley.tilemapArena = InitArena(250 << 10);
    Motley.tilemap      = (Tilemap){};
}

void UpdateMotley() {
        UpdateTileCursor(&Motley.tilemap, &Motley.cursor, &Motley.toolbar);
        UpdateToolbar(&Motley.toolbar);
        UpdateTileset(&Motley.tileset, &Motley.tilemap);
        SaveTilemap(&Motley.tilemap, "tilemap.txt");  // TODO: Save popup
}

void CloseMotley() {
    if (Motley.tilemapArena.buffer != NULL) free(Motley.tilemapArena.buffer);
}

void DrawMotley() {
            DrawBg(Motley.bg);

            DrawTilemap(&Motley.tilemap);
            DrawTileCursor(&Motley.tilemap, &Motley.cursor, &Motley.toolbar);
            DrawBoxCursor(&Motley.tilemap, &Motley.cursor);

            DrawToolbar(&Motley.toolbar, &Motley.popupNew, &Motley.popupLoad, &Motley.tilemap, &Motley.cursor);
            DrawTileset(&Motley.tileset, &Motley.tilemap, &Motley.cursor);

            if (DrawNewTilemapMenu(&Motley.popupNew)) {
                Motley.tilemap = InitTilemap(&Motley.popupNew, &Motley.tilemapArena);
            }
            if (DrawPopupLoad(&Motley.popupLoad)) {
                Motley.tilemap = InitTilemapFromFile(&Motley.popupLoad, &Motley.tilemapArena);
            }
}

i32 main() {
    InitGameWindow(&window);
    InitMotley();

    while (!WindowShouldClose()) {
        GameWindowControls(&window);

        UpdateMotley();

        BeginDrawing();
        DrawMotley();
        EndDrawing();
    }

    CloseMotley();
    CloseWindow();
}
