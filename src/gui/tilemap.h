#pragma once

#include <stdlib.h>

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#include "../types.h"
#include "raylib.h"
#include "window.h"

typedef struct {
    Window Window;

    v2  Pos;
    v2i Size;
    v2i tileSize;

    v2* Tile;
    u8* FG;
    u8* BG;
    u8* Rot;

    Texture     Tileset;
    const char* TilesetPath;
    Color*      Palette;
    u32         PaletteSize;
} Tilemap;

typedef enum { CURSOR_STATE_SINGLE, CURSOR_STATE_BOXING, CURSOR_STATE_BOXED } CursorState;

typedef struct {
    v2   Selected;
    u8   FG;
    u8   BG;
    u8   Rot;
    v2   Pos;
    bool InMap;
    u32  CurrentTilemap;

    CursorState State;

    v2i Box[2];
    v2* BoxSelection;

} TilemapCursor;

typedef enum TileRotation { ROT_UP, ROT_RIGHT, ROT_BOT, ROT_LEFT } TileRotation;

Color* LoadPaletteFromHex(const char* path, u32* size) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Couldn't load palette file");
        return MALLOC_T(Color, 2);
    }

    u32    colors  = CountLines(path);
    Color* palette = MALLOC_T(Color, colors);

    char line[16];

    if (EndsWith(path, ".hex")) {
        for (u32 i = 0; i < colors && fgets(line, sizeof(line), file); i++) {
            char* newline = strchr(line, '\n');
            if (newline) *newline = '\0';

            u32 rgb;
            if (sscanf_s(line, "%x", &rgb) != 1) {
                TraceLog(LOG_ERROR, "Error parsing line: %s\n", line);
                return MALLOC_T(Color, 2);
            }

            palette[i].r = (rgb >> 16) & 0xFF;  // Red
            palette[i].g = (rgb >> 8) & 0xFF;   // Green
            palette[i].b = rgb & 0xFF;          // Blue
            palette[i].a = 0xFF;                // Alpha
        }

        TraceLog(LOG_INFO, "Palette loaded successfully");
    }

    fclose(file);

    *size = colors;
    return palette;
}

Tilemap NewTilemap(const char* tileset, const char* palette, v2i tilesize, v2i size, v2 pos) {
    Tilemap map = {
        .Tileset     = LoadTexture(tileset),
        .TilesetPath = tileset,
        .tileSize    = tilesize,
        .Pos         = pos,
        .Size        = size,
        .Palette     = LoadPaletteFromHex(palette, &map.PaletteSize),
    };

    map.Window = (Window){.Active       = true,
                          .Anchor       = pos,
                          .Size         = (v2){size.x * tilesize.x, size.y * tilesize.y},
                          .Title        = "*New tilemap*",
                          .TitlebarSize = (v2){size.x * tilesize.x, 24}};

    u32 tileCount = map.Size.x * map.Size.y;
    map.Tile      = MALLOC_T(v2, tileCount);
    map.FG        = MALLOC_T(u8, tileCount);
    map.BG        = MALLOC_T(u8, tileCount);
    map.Rot       = MALLOC_T(u8, tileCount);

    for (u32 x = 0; x < map.Size.x; x++) {
        for (u32 y = 0; y < map.Size.y; y++) {
            u32 coord       = y * map.Size.x + x;
            map.FG[coord]   = 0;
            map.BG[coord]   = 1;
            map.Tile[coord] = (v2){};
            map.Rot[coord]  = ROT_UP;
        }
    }

    return map;
}

Tilemap LoadTilemap(const char* path) {
    Tilemap map = {};

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Couldn't load tilemap");
        return map;
    }

    TraceLog(LOG_INFO, "Tilemap loaded successfully");
    return map;  // TODO
}

bool SaveTilemap(Tilemap* this, const char* path) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Couldn't save tilemap to file");
        return false;
    }

    fprintf(file, "%.2f %.2f\n", this->Pos.x, this->Pos.y);

    fprintf(file, "%d %d\n", this->Size.x, this->Size.y);

    fprintf(file, "%d %d\n", this->tileSize.x, this->tileSize.y);

    for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
        fprintf(file, "%.2f %.2f ", this->Tile[i].x, this->Tile[i].y);
    }
    fprintf(file, "\n");

    for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
        fprintf(file, "%d ", this->FG[i]);
    }
    fprintf(file, "\n");

    for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
        fprintf(file, "%d ", this->BG[i]);
    }
    fprintf(file, "\n");

    for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
        fprintf(file, "%d ", this->Rot[i]);
    }
    fprintf(file, "\n");

    fprintf(file, this->TilesetPath);
    fprintf(file, "\n");

    for (u32 i = 0; i < this->PaletteSize; i++) {
        fprintf(file, "%d %d %d %d ", this->Palette[i].r, this->Palette[i].g, this->Palette[i].b,
                this->Palette[i].a);
    }
    fprintf(file, "\n");

    fclose(file);

    TraceLog(LOG_INFO, "Tilemap saved successfully");
    return true;
}

static Shader FgTilemap;
static Shader FgCursor;
static Shader BgTilemap;
static Shader BgCursor;

void InitTilemapShaders() {
    FgTilemap = LoadShader("shaders/tiles.vert", "shaders/tile_fg.frag");
    FgCursor  = LoadShader("shaders/tiles.vert", "shaders/tile_fg_cursor.frag");
    BgTilemap = LoadShader("shaders/tiles.vert", "shaders/tile_bg.frag");
    BgCursor  = LoadShader("shaders/tiles.vert", "shaders/tile_bg_cursor.frag");
}

void DrawTilemap(Tilemap* map) {
    // GuiWindowBox((Rectangle){map->Pos.x - 20, map->Pos.y - 20, map->Size.x * map->tileSize.x +
    // 15,
    //                          map->Size.y * map->tileSize.y + 15},
    //              "*New tilemap*");
    UpdateWindow(&map->Window);

    v2 tAnchor = {map->Window.Anchor.x + map->tileSize.x * 0.5f,
                  map->Window.Anchor.y + map->tileSize.y * 0.5f + map->Window.TitlebarSize.y};

    BeginShaderMode(FgTilemap);
    for (u32 x = 0; x < map->Size.x; x++) {
        for (u32 y = 0; y < map->Size.y; y++) {
            u32 coord = y * map->Size.x + x;
            DrawTexturePro(
                map->Tileset,
                (Rectangle){map->tileSize.x * map->Tile[coord].x,
                            map->tileSize.y * map->Tile[coord].y, map->tileSize.x, map->tileSize.y},
                (Rectangle){map->tileSize.x * x + tAnchor.x, map->tileSize.y * y + tAnchor.y,
                            map->tileSize.x, map->tileSize.y},
                (v2){map->tileSize.x * 0.5f, map->tileSize.y * 0.5f}, map->Rot[coord] * 90.0f,
                map->Palette[map->FG[coord]]);
        }
    }
    EndShaderMode();

    BeginShaderMode(BgTilemap);
    for (u32 x = 0; x < map->Size.x; x++) {
        for (u32 y = 0; y < map->Size.y; y++) {
            u32 coord = y * map->Size.x + x;
            DrawTexturePro(
                map->Tileset,
                (Rectangle){map->tileSize.x * map->Tile[coord].x,
                            map->tileSize.y * map->Tile[coord].y, map->tileSize.x, map->tileSize.y},
                (Rectangle){map->tileSize.x * x + tAnchor.x, map->tileSize.y * y + tAnchor.y,
                            map->tileSize.x, map->tileSize.y},
                (v2){map->tileSize.x * 0.5f, map->tileSize.y * 0.5f}, map->Rot[coord] * 90.0f,
                map->Palette[map->BG[coord]]);
        }
    }
    EndShaderMode();
}

typedef struct {
    Vector2 anchor;

    bool createTilemap;

    bool NewTilemapWindowActive;
    bool TilesetPathEditMode;
    char TilesetPathText[256];
    bool tileSizeXEditMode;
    i32  tileSizeXValue;
    bool tileSizeYEditMode;
    i32  tileSizeYValue;
    bool PalettePathEditMode;
    char PalettePathText[256];
    bool mapSizeXEditMode;
    i32  mapSizeXValue;
    bool mapSizeYEditMode;
    i32  mapSizeYValue;

} NewTilemapMenuState;

NewTilemapMenuState InitNewTilemapMenu() {
    NewTilemapMenuState state = {0};

    state.anchor = (Vector2){GetScreenWidth() * 0.5f - 140, GetScreenHeight() * 0.5f - 76};

    state.NewTilemapWindowActive = false;
    state.TilesetPathEditMode    = false;
    strcpy(state.TilesetPathText, "assets/mrmox3.png");
    state.tileSizeXEditMode   = false;
    state.tileSizeXValue      = 24;
    state.tileSizeYEditMode   = false;
    state.tileSizeYValue      = 24;
    state.PalettePathEditMode = false;
    strcpy(state.PalettePathText, "assets/dreamscape8.hex");
    state.mapSizeXEditMode = false;
    state.mapSizeXValue    = 20;
    state.mapSizeYEditMode = false;
    state.mapSizeYValue    = 15;

    return state;
}
static void CreateTilemapButton(NewTilemapMenuState* state) { state->createTilemap = true; }

Tilemap StartTilemapEditor(NewTilemapMenuState* state, WindowOptions* window) {
    state->createTilemap          = false;
    state->NewTilemapWindowActive = false;

    return NewTilemap(state->TilesetPathText, state->PalettePathText,
                      (v2i){state->tileSizeXValue, state->tileSizeYValue},
                      (v2i){state->mapSizeXValue, state->mapSizeYValue}, (v2){});
}

void NewTilemapMenu(NewTilemapMenuState* state) {
    const char* NewTilemapWindowText    = "New Tilemap";
    const char* tileSizeLabelText       = "Tile Size";
    const char* mapSizeLabelText        = "Map Size";
    const char* CreateTilemapButtonText = "Create Tilemap";

    if (state->NewTilemapWindowActive) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
            IsPointInRectangle(GetMousePosition(),
                               (Rectangle){state->anchor.x, state->anchor.y, 275, 25})) {
            state->anchor = Vector2Add(state->anchor, GetMouseDelta());
        }

        state->NewTilemapWindowActive = !GuiWindowBox(
            (Rectangle){state->anchor.x + 0, state->anchor.y + 0, 280, 152}, NewTilemapWindowText);
        if (GuiTextBox((Rectangle){state->anchor.x + 8, state->anchor.y + 32, 120, 24},
                       state->TilesetPathText, 128, state->TilesetPathEditMode))
            state->TilesetPathEditMode = !state->TilesetPathEditMode;
        if (GuiValueBox((Rectangle){state->anchor.x + 192, state->anchor.y + 32, 32, 24}, "",
                        &state->tileSizeXValue, 0, 100, state->tileSizeXEditMode))
            state->tileSizeXEditMode = !state->tileSizeXEditMode;
        if (GuiValueBox((Rectangle){state->anchor.x + 232, state->anchor.y + 32, 32, 24}, "",
                        &state->tileSizeYValue, 0, 100, state->tileSizeYEditMode))
            state->tileSizeYEditMode = !state->tileSizeYEditMode;
        if (GuiTextBox((Rectangle){state->anchor.x + 8, state->anchor.y + 72, 120, 24},
                       state->PalettePathText, 128, state->PalettePathEditMode))
            state->PalettePathEditMode = !state->PalettePathEditMode;
        if (GuiValueBox((Rectangle){state->anchor.x + 192, state->anchor.y + 72, 32, 24}, "",
                        &state->mapSizeXValue, 0, 100, state->mapSizeXEditMode))
            state->mapSizeXEditMode = !state->mapSizeXEditMode;
        if (GuiValueBox((Rectangle){state->anchor.x + 232, state->anchor.y + 72, 32, 24}, "",
                        &state->mapSizeYValue, 0, 100, state->mapSizeYEditMode))
            state->mapSizeYEditMode = !state->mapSizeYEditMode;
        GuiLabel((Rectangle){state->anchor.x + 136, state->anchor.y + 32, 48, 24},
                 tileSizeLabelText);
        GuiLabel((Rectangle){state->anchor.x + 136, state->anchor.y + 72, 56, 24},
                 mapSizeLabelText);
        if (GuiButton((Rectangle){state->anchor.x + 72, state->anchor.y + 112, 120, 24},
                      CreateTilemapButtonText))
            CreateTilemapButton(state);
    }
}