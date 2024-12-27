#pragma once

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib.h"
#include "types.h"

typedef struct {
    v2          Pos;
    v2i         Size;
    v2i         tileSize;
    v2*         Tile;
    u8*         FG;
    u8*         BG;
    u8*         Rot;
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
    i32         Box[2];
    v2*         BoxSelection;

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

Tilemap NewTilemap(const char* tileset, const char* palette, v2i tilesize, v2i size) {
    Tilemap map = {};

    map.Tileset     = LoadTexture(tileset);
    map.TilesetPath = tileset;
    map.tileSize    = tilesize;
    map.Pos         = (v2){};
    map.Size        = size;

    map.Palette = LoadPaletteFromHex(palette, &map.PaletteSize);

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
    BeginShaderMode(FgTilemap);
    for (u32 x = 0; x < map->Size.x; x++) {
        for (u32 y = 0; y < map->Size.y; y++) {
            u32 coord = y * map->Size.x + x;
            DrawTexturePro(
                map->Tileset,
                (Rectangle){map->tileSize.x * map->Tile[coord].x,
                            map->tileSize.y * map->Tile[coord].y, map->tileSize.x, map->tileSize.y},
                (Rectangle){map->tileSize.x * x + map->Pos.x, map->tileSize.y * y + map->Pos.y,
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
                (Rectangle){map->tileSize.x * x + map->Pos.x, map->tileSize.y * y + map->Pos.y,
                            map->tileSize.x, map->tileSize.y},
                (v2){map->tileSize.x * 0.5f, map->tileSize.y * 0.5f}, map->Rot[coord] * 90.0f,
                map->Palette[map->BG[coord]]);
        }
    }
    EndShaderMode();

    GuiGroupBox((Rectangle){map->Pos.x - 20, map->Pos.y - 20, map->Size.x * map->tileSize.x + 15,
                            map->Size.y * map->tileSize.y + 15},
                "*New tilemap*");
}

void DrawTileCursor(Tilemap* map, TilemapCursor* cursor) {
    BeginShaderMode(FgCursor);
    DrawTexturePro(
        map->Tileset,
        (Rectangle){map->tileSize.x * cursor->Selected.x, map->tileSize.y * cursor->Selected.y,
                    map->tileSize.x, map->tileSize.y},
        (Rectangle){cursor->Pos.x - fmod(cursor->Pos.x, map->tileSize.x),
                    cursor->Pos.y - fmod(cursor->Pos.y, map->tileSize.y), map->tileSize.x,
                    map->tileSize.y},
        (v2){map->tileSize.x * 0.5f, map->tileSize.y * 0.5f}, cursor->Rot * 90.0f,
        map->Palette[cursor->FG]);
    EndShaderMode();

    BeginShaderMode(BgCursor);
    DrawTexturePro(
        map->Tileset,
        (Rectangle){map->tileSize.x * cursor->Selected.x, map->tileSize.y * cursor->Selected.y,
                    map->tileSize.x, map->tileSize.y},
        (Rectangle){cursor->Pos.x - fmod(cursor->Pos.x, map->tileSize.x),
                    cursor->Pos.y - fmod(cursor->Pos.y, map->tileSize.y), map->tileSize.x,
                    map->tileSize.y},
        (v2){map->tileSize.x * 0.5f, map->tileSize.y * 0.5f}, cursor->Rot * 90.0f,
        map->Palette[cursor->BG]);
    EndShaderMode();
}

void UpdateTileCursor(Tilemap* map, TilemapCursor* cursor, Camera2D cam) {
    cursor->Pos = GetScreenToWorld2D(
        Vector2Add(GetMousePosition(), Vector2Scale((v2){map->tileSize.x, map->tileSize.y}, 0.5f)),
        cam);
    cursor->InMap = cursor->Pos.x < (map->Size.x * map->tileSize.x + map->Pos.x) &&
                    cursor->Pos.x > map->Pos.x &&
                    cursor->Pos.y < (map->Size.y * map->tileSize.y + map->Pos.y) &&
                    cursor->Pos.y > map->Pos.y;

    i32 coord = cursor->InMap ? (i32)(cursor->Pos.y / map->tileSize.y + map->Pos.y) * map->Size.x +
                                    (i32)(cursor->Pos.x / map->tileSize.x + map->Pos.x)
                              : 0;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && cursor->InMap) {
        map->FG[coord]   = cursor->FG;
        map->BG[coord]   = cursor->BG;
        map->Rot[coord]  = cursor->Rot;
        map->Tile[coord] = cursor->Selected;
    }

    // ===== PALETTE BUTTONS =====
    if (IsActionPressed(ACTION_LEFT) && cursor->Selected.x != 0) cursor->Selected.x -= 1;
    if (IsActionPressed(ACTION_RIGHT) &&
        cursor->Selected.x < map->Tileset.width / map->tileSize.x - 1)
        cursor->Selected.x += 1;
    if (IsActionPressed(ACTION_UP) && cursor->Selected.y != 0) cursor->Selected.y -= 1;
    if (IsActionPressed(ACTION_DOWN) &&
        cursor->Selected.y < map->Tileset.height / map->tileSize.y - 1)
        cursor->Selected.y += 1;
    if (IsKeyPressed(KEY_R)) cursor->Rot = (cursor->Rot + 1) % 4;

    if (IsKeyPressed(KEY_F)) cursor->FG = (cursor->FG + 1) % map->PaletteSize;
    if (IsKeyPressed(KEY_B)) cursor->BG = (cursor->BG + 1) % map->PaletteSize;
    if (IsKeyPressed(KEY_G)) {
        u8 swap    = cursor->FG;
        cursor->FG = cursor->BG;
        cursor->BG = swap;
    }

    // ===== BOX SELECT =====
    if (!cursor->InMap) return;

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        cursor->State  = CURSOR_STATE_BOXING;
        cursor->Box[0] = coord;
    }

    if (cursor->State == CURSOR_STATE_BOXING) {
        cursor->Box[1] = coord;
    }

    if (cursor->State == CURSOR_STATE_BOXING && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        cursor->State = CURSOR_STATE_BOXED;
    }

    if (cursor->State == CURSOR_STATE_BOXED && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        cursor->State = CURSOR_STATE_SINGLE;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_X) && cursor->State == CURSOR_STATE_BOXED) {
        if (cursor->BoxSelection) free(cursor->BoxSelection);

        cursor->BoxSelection = MALLOC_T(v2, cursor->Box[1] - cursor->Box[0] + 1);

        for (u32 i = cursor->Box[0]; i <= cursor->Box[1]; i++) {
            cursor->BoxSelection[i - cursor->Box[0]] = map->Tile[i];
            map->Tile[i]                             = (v2){};
        }

        cursor->State = CURSOR_STATE_SINGLE;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V) && cursor->BoxSelection &&
        cursor->State != CURSOR_STATE_BOXING) {
        // FIXME: DOESN'T CONSIDER BOX WIDTH!!!
        for (u32 i = 0; i <= cursor->Box[1] - cursor->Box[0]; i++) {
            map->Tile[i + coord] = cursor->BoxSelection[i];
        }
    }
}

void DrawBoxCursor(Tilemap* map, TilemapCursor* cursor) {
    DrawRectangleLines(
        (cursor->Box[0] % map->Size.x) * map->tileSize.x - map->tileSize.x * 0.5f,
        (cursor->Box[0] / map->Size.x) * map->tileSize.y - map->tileSize.y * 0.5f,
        ((cursor->Box[1] - cursor->Box[0]) % map->Size.x) * map->tileSize.x + map->tileSize.x,
        ((cursor->Box[1] - cursor->Box[0]) / map->Size.x) * map->tileSize.y + map->tileSize.y,
        GRAY);
}

void DrawTilemapGui(Tilemap* map, TilemapCursor* cursor) {
    f32 tilesetScale = 1.0f / 3.0f;
    v2i scaledTile   = (v2i){map->tileSize.x * tilesetScale, map->tileSize.y * tilesetScale};

    BeginShaderMode(FgTilemap);
    DrawTextureEx(map->Tileset, (v2){0, 0}, 0.0f, tilesetScale, map->Palette[cursor->FG]);
    EndShaderMode();

    BeginShaderMode(BgTilemap);
    DrawTextureEx(map->Tileset, (v2){0, 0}, 0.0f, tilesetScale, map->Palette[cursor->BG]);
    EndShaderMode();

    DrawRectangleLines(cursor->Selected.x * scaledTile.x, cursor->Selected.y * scaledTile.y,
                       scaledTile.x, scaledTile.y, GREEN);

    if (GetMouseX() < map->Tileset.width * tilesetScale &
        GetMouseY() < map->Tileset.height * tilesetScale) {
        v2 cur = {GetMouseX() - GetMouseX() % scaledTile.x,
                  GetMouseY() - GetMouseY() % scaledTile.y};
        DrawRectangleLines(cur.x, cur.y, scaledTile.x, scaledTile.y, BLUE);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            cursor->Selected = (v2){cur.x / scaledTile.x, cur.y / scaledTile.y};
        }
    }

    for (u32 i = 0; i < map->PaletteSize; i++) {
        Rectangle rect = {0 + i * 20, map->Tileset.height * tilesetScale + 20, 10, 10};

        DrawRectangleRec(rect, map->Palette[i]);

        if (cursor->FG == i) DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, GREEN);
        if (cursor->BG == i) DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, YELLOW);
    }
}