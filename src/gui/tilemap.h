#pragma once

#include <stdlib.h>
#include <string.h>

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#include "../types.h"
#include "popup_load.h"
#include "raylib.h"
#include "window.h"

typedef struct {
    v2u* Tile;
    u8*  FG;
    u8*  BG;
    u8*  Rot;
} TilemapLayer;

#define MAX_LAYERS 2

typedef struct {
    Window Window;

    v2u Size;
    v2u tileSize;

    TilemapLayer Layer[MAX_LAYERS];
    u8           SelectedLayer;
    // v2u* Tile;
    // u8*  FG;
    // u8*  BG;
    // u8*  Rot;

    Texture     Tileset;
    const char* TilesetPath;
    Color*      Palette;
    u32         PaletteSize;
} Tilemap;

typedef struct {
    Window Window;

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

} PopupNewState;

typedef enum { CURSOR_STATE_SINGLE, CURSOR_STATE_BOXING, CURSOR_STATE_BOXED } CursorState;

typedef struct {
    v2i  Selected;
    u8   FG;
    u8   BG;
    u8   Rot;
    bool InMap;
    u32  CurrentTilemap;

    CursorState State;

    v2i Box[2];
    v2* BoxSelection;

} TilemapCursor;

typedef enum TileRotation { ROT_UP, ROT_RIGHT, ROT_BOT, ROT_LEFT } TileRotation;

Color* LoadPaletteFromHex(const char* path, /* out var */ u32* size) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Couldn't load palette file");
        return MALLOC_T(Color, 2);
    }

    u32    colors  = CountLines(path) + 3;
    Color* palette = MALLOC_T(Color, colors);

    char line[16];

    palette[0] = BLANK;
    palette[1] = WHITE;
    palette[2] = BLACK;

    if (EndsWith(path, ".hex")) {
        for (u32 i = 3; i < colors && fgets(line, sizeof(line), file); i++) {
            char* newline = strchr(line, '\n');
            if (newline) *newline = '\0';

            u32 rgb;
            if (sscanf(line, "%x", &rgb) != 1) {
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

Tilemap InitTilemap(const PopupNewState* state, Arena* arena) {
    u32     paletteSize = 0;
    Color*  palette     = LoadPaletteFromHex(state->PalettePathText, &paletteSize);
    Tilemap map         = {
                .Tileset     = LoadTexture(state->TilesetPathText),
                .TilesetPath = state->TilesetPathText,
                .tileSize    = (v2u){state->tileSizeXValue, state->tileSizeYValue},
                .Size        = (v2u){state->mapSizeXValue, state->mapSizeYValue},
                .Palette     = palette,
                .PaletteSize = paletteSize,
    };

    map.Window = InitWindowBox("*New tilemap*",
                               (v2){map.Size.x * map.tileSize.x, map.Size.y * map.tileSize.y});

    u32 tileCount = map.Size.x * map.Size.y;
    for (u8 i = 0; i < MAX_LAYERS; i++) {
        map.Layer[i].Tile = AALLOC(arena, v2u, tileCount);
        map.Layer[i].FG   = AALLOC(arena, u8, tileCount);
        map.Layer[i].BG   = AALLOC(arena, u8, tileCount);
        map.Layer[i].Rot  = AALLOC(arena, u8, tileCount);
    }

    for (u32 x = 0; x < map.Size.x; x++) {
        for (u32 y = 0; y < map.Size.y; y++) {
            u32 coord                = y * map.Size.x + x;
            map.Layer[0].FG[coord]   = 0;
            map.Layer[0].BG[coord]   = 1;
            map.Layer[0].Tile[coord] = (v2u){};
            map.Layer[0].Rot[coord]  = ROT_UP;
            map.Layer[1].FG[coord]   = 0;
            map.Layer[1].BG[coord]   = 1;
            map.Layer[1].Tile[coord] = (v2u){};
            map.Layer[1].Rot[coord]  = ROT_UP;
        }
    }

    return map;
}

bool SaveTilemap(Tilemap* this, const char* path) {
    // if (GlobalGuiState.Editing) return false;
    // if (!this->Window.Active || !IsKeyDown(KEY_LEFT_CONTROL) || !IsKeyPressed(KEY_S)) return
    // false;

    // FILE* file = fopen(path, "w");
    // if (file == NULL) {
    //     TraceLog(LOG_ERROR, "Couldn't save tilemap to file");
    //     return false;
    // }

    // fprintf(file, "%d %d\n", this->Size.x, this->Size.y);

    // fprintf(file, "%d %d\n", this->tileSize.x, this->tileSize.y);

    // for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
    //     fprintf(file, "%.2f %.2f ", this->Tile[i].x, this->Tile[i].y);
    // }
    // fprintf(file, "\n");

    // for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
    //     fprintf(file, "%d ", this->FG[i]);
    // }
    // fprintf(file, "\n");

    // for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
    //     fprintf(file, "%d ", this->BG[i]);
    // }
    // fprintf(file, "\n");

    // for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
    //     fprintf(file, "%d ", this->Rot[i]);
    // }
    // fprintf(file, "\n");

    // fprintf(file, this->TilesetPath);
    // fprintf(file, "\n");

    // fprintf(file, "%u\n", this->PaletteSize);

    // for (u32 i = 0; i < this->PaletteSize; i++) {
    //     fprintf(file, "%d %d %d %d ", this->Palette[i].r, this->Palette[i].g, this->Palette[i].b,
    //             this->Palette[i].a);
    // }
    // fprintf(file, "\n");

    // fclose(file);

    // TraceLog(LOG_INFO, "Tilemap saved successfully");

    // this->Window.Title = path;
    return true;
}

Tilemap InitTilemapFromFile(PopupLoadState* state, Arena* arena) {
    FILE* file = fopen(state->TilemapPathTextBoxText, "r");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Couldn't open tilemap file");
        return (Tilemap){};
    }

    Tilemap this = {};

    // fscanf(file, "%d %d", &this.Size.x, &this.Size.y);

    // fscanf(file, "%d %d", &this.tileSize.x, &this.tileSize.y);

    // u32 tileCount = this.Size.x * this.Size.y;
    // this.Tile     = AALLOC(arena, v2u, tileCount);
    // this.FG       = AALLOC(arena, u8, tileCount);
    // this.BG       = AALLOC(arena, u8, tileCount);
    // this.Rot      = AALLOC(arena, u8, tileCount);

    // for (u32 i = 0; i < tileCount; i++) {
    //     fscanf(file, "%f %f", &this.Tile[i].x, &this.Tile[i].y);
    // }

    // for (u32 i = 0; i < tileCount; i++) {
    //     fscanf(file, "%hhu", &this.FG[i]);
    // }

    // for (u32 i = 0; i < tileCount; i++) {
    //     fscanf(file, "%hhu", &this.BG[i]);
    // }

    // for (u32 i = 0; i < tileCount; i++) {
    //     fscanf(file, "%hhu", &this.Rot[i]);
    // }

    // char tilesetPath[256];
    // fscanf(file, "%255s", tilesetPath);
    // this.TilesetPath = strdup(tilesetPath);
    // this.Tileset     = LoadTexture(this.TilesetPath);

    // fscanf(file, "%u", &this.PaletteSize);
    // this.Palette = AALLOC(arena, Color, this.PaletteSize);
    // for (u32 i = 0; i < this.PaletteSize; i++) {
    //     fscanf(file, "%hhu %hhu %hhu %hhu", &this.Palette[i].r, &this.Palette[i].g,
    //            &this.Palette[i].b, &this.Palette[i].a);
    // }

    // fclose(file);

    // this.Window = InitWindowBox(state->TilemapPathTextBoxText,
    //                             (v2){this.Size.x * this.tileSize.x, this.Size.y *
    //                             this.tileSize.y});

    // TraceLog(LOG_INFO, "Tilemap loaded successfully");
    return this;
}

static Shader FgTilemap;
static Shader FgCursor;
static Shader BgTilemap;
static Shader BgCursor;
static Shader wallpaperColor;

void InitTilemapShaders() {
    FgTilemap = LoadShader("src/shaders/tiles.vert", "src/shaders/tile_fg.frag");
    FgCursor  = LoadShader("src/shaders/tiles.vert", "src/shaders/tile_fg_cursor.frag");
    BgTilemap = LoadShader("src/shaders/tiles.vert", "src/shaders/tile_bg.frag");
    BgCursor  = LoadShader("src/shaders/tiles.vert", "src/shaders/tile_bg_cursor.frag");

    wallpaperColor = LoadShader("src/shaders/tiles.vert", "src/shaders/wallpaper.frag");
    v3 col1        = {0.34, 0.34, 0.34};
    v3 col2        = {0.55, 0.55, 0.55};
    SetShaderValue(wallpaperColor, GetShaderLocation(wallpaperColor, "color1"), &col1,
                   SHADER_UNIFORM_VEC3);
    SetShaderValue(wallpaperColor, GetShaderLocation(wallpaperColor, "color2"), &col2,
                   SHADER_UNIFORM_VEC3);
}

void DrawBg(Texture bg) {
    BeginShaderMode(wallpaperColor);
    // TODO: Texture wrap doesn't work?
    DrawTexture(bg, 0, 0, WHITE);
    DrawTexture(bg, bg.width, 0, WHITE);
    DrawTexture(bg, 0, bg.height, WHITE);
    DrawTexture(bg, bg.width, bg.height, WHITE);
    EndShaderMode();
}

void DrawTilemap(Tilemap* map) {
    if (!map->Window.Active) return;

    UpdateWindow(&map->Window);

    v2 tAnchor = {map->Window.Anchor.x + map->tileSize.x * 0.5f,
                  map->Window.Anchor.y + map->tileSize.y * 0.5f + 24};

    BeginShaderMode(BgTilemap);
    for (u32 x = 0; x < map->Size.x; x++) {
        for (u32 y = 0; y < map->Size.y; y++) {
            u32 coord = y * map->Size.x + x;
            DrawTexturePro(
                map->Tileset,
                (Rectangle){map->tileSize.x * map->Layer[0].Tile[coord].x,
                            map->tileSize.y * map->Layer[0].Tile[coord].y, map->tileSize.x,
                            map->tileSize.y},
                (Rectangle){map->tileSize.x * x + tAnchor.x, map->tileSize.y * y + tAnchor.y,
                            map->tileSize.x, map->tileSize.y},
                (v2){map->tileSize.x * 0.5f, map->tileSize.y * 0.5f},
                map->Layer[0].Rot[coord] * 90.0f, map->Palette[map->Layer[0].BG[coord]]);
        }
    }
    EndShaderMode();

    BeginShaderMode(FgTilemap);
    // TODO Should be a single draw.
    for (u8 i = 0; i < MAX_LAYERS; i++)
        for (u32 x = 0; x < map->Size.x; x++) {
            for (u32 y = 0; y < map->Size.y; y++) {
                u32 coord = y * map->Size.x + x;
                DrawTexturePro(
                    map->Tileset,
                    (Rectangle){map->tileSize.x * map->Layer[i].Tile[coord].x,
                                map->tileSize.y * map->Layer[i].Tile[coord].y, map->tileSize.x,
                                map->tileSize.y},
                    (Rectangle){map->tileSize.x * x + tAnchor.x, map->tileSize.y * y + tAnchor.y,
                                map->tileSize.x, map->tileSize.y},
                    (v2){map->tileSize.x * 0.5f, map->tileSize.y * 0.5f},
                    map->Layer[i].Rot[coord] * 90.0f, map->Palette[map->Layer[i].FG[coord]]);
            }
        };
    EndShaderMode();
}

PopupNewState InitNewTilemapMenu() {
    PopupNewState state = {
        .Window = {.Active = false,
                   .Anchor = {GetScreenWidth() * 0.5f - 140, GetScreenHeight() * 0.5f - 76},
                   .Title  = "New tilemap",
                   .Size   = {272, 144}}};

    state.TilesetPathEditMode = false;
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

bool DrawNewTilemapMenu(PopupNewState* state) {
    if (!state->Window.Active) return false;

    GlobalGuiState.Editing = true;

    static const char* tileSizeLabelText       = "Tile Size";
    static const char* mapSizeLabelText        = "Map Size";
    static const char* CreateTilemapButtonText = "Create Tilemap";

    UpdateWindow(&state->Window);
    v2 tAnchor = state->Window.Anchor;

    if (GuiTextBox((Rectangle){tAnchor.x + 8, tAnchor.y + 32, 120, 24}, state->TilesetPathText, 128,
                   state->TilesetPathEditMode))
        state->TilesetPathEditMode = !state->TilesetPathEditMode;
    if (GuiValueBox((Rectangle){tAnchor.x + 192, tAnchor.y + 32, 32, 24}, "",
                    &state->tileSizeXValue, 0, 100, state->tileSizeXEditMode))
        state->tileSizeXEditMode = !state->tileSizeXEditMode;
    if (GuiValueBox((Rectangle){tAnchor.x + 232, tAnchor.y + 32, 32, 24}, "",
                    &state->tileSizeYValue, 0, 100, state->tileSizeYEditMode))
        state->tileSizeYEditMode = !state->tileSizeYEditMode;
    if (GuiTextBox((Rectangle){tAnchor.x + 8, tAnchor.y + 72, 120, 24}, state->PalettePathText, 128,
                   state->PalettePathEditMode))
        state->PalettePathEditMode = !state->PalettePathEditMode;
    if (GuiValueBox((Rectangle){tAnchor.x + 192, tAnchor.y + 72, 32, 24}, "", &state->mapSizeXValue,
                    0, 100, state->mapSizeXEditMode))
        state->mapSizeXEditMode = !state->mapSizeXEditMode;
    if (GuiValueBox((Rectangle){tAnchor.x + 232, tAnchor.y + 72, 32, 24}, "", &state->mapSizeYValue,
                    0, 100, state->mapSizeYEditMode))
        state->mapSizeYEditMode = !state->mapSizeYEditMode;
    GuiLabel((Rectangle){tAnchor.x + 136, tAnchor.y + 32, 48, 24}, tileSizeLabelText);
    GuiLabel((Rectangle){tAnchor.x + 136, tAnchor.y + 72, 56, 24}, mapSizeLabelText);

    bool newTilemap =
        GuiButton((Rectangle){tAnchor.x + 72, tAnchor.y + 112, 120, 24}, CreateTilemapButtonText);

    state->Window.Active   = !newTilemap;
    GlobalGuiState.Editing = !newTilemap;
    return newTilemap;
}