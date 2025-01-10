#pragma once

#include "shared.h"

/*
FIXME 1.0:
- [X] Fix layout error in color picker
- [X] Fix layout error in tileset
- [X] Fix layout error in tile cursor
- [X] Add blank to colors' GUI
- Rotation exporting

TODO 1.0:
- Undo
- [X] Copy/Cut/Paste Regions
- [X] Action mods
- [X] Exporting

TODO 1.1:
- ImgUI
- Palette picker
- Layer GUI
- Optimize tile drawing shader
- Rectangle draw
- Tile offset?
- Paint individual tile?
- Freehand tiles?
*/

static struct {
    bool Editing;
    bool Moving;
} GlobalGuiState = {};

typedef struct {
    v2          Anchor;
    v2          Size;
    bool        Active;
    bool        Moving;
    const char* Title;
} Window;

Window InitWindowBox(const char* title, v2 size) {
    return (Window){.Active = true,
                    .Anchor = (v2){GetScreenWidth() * 0.5f - size.x * 0.5f,
                                   GetScreenHeight() * 0.5f - size.y * 0.5f},
                    .Size   = size,
                    .Title  = title};
}

void UpdateWindow(Window* state) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(),
                               (Rectangle){state->Anchor.x, state->Anchor.y, state->Size.x,
                                           RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT})) {
        state->Moving         = true;
        GlobalGuiState.Moving = true;
    }

    if (state->Moving) {
        state->Anchor = Vector2Add(state->Anchor, GetMouseDelta());
    }

    if ((IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && state->Moving) ||
        IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        state->Moving         = false;
        GlobalGuiState.Moving = false;
    }

    bool xPressed = GuiWindowBox(
        (Rectangle){state->Anchor.x, state->Anchor.y, state->Size.x, state->Size.y}, state->Title);

    if (xPressed && state->Active) {
        state->Active          = false;
        GlobalGuiState.Editing = false;
    }
}

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

    Texture     Tileset;
    const char* TilesetPath;
    Color*      Palette;  // 0 = BLANK, 1 = WHITE, 2 = BLACK, 3+ = COLORS
    u32         PaletteSize;
} Tilemap;

typedef enum { CURSOR_STATE_SINGLE, CURSOR_STATE_BOXING, CURSOR_STATE_BOXED } CursorState;

typedef struct {
    v2i  Selected;
    u8   FG;
    u8   BG;
    u8   Rot;
    bool InMap;
    u32  CurrentTilemap;

    CursorState State;

    v2u   Box[2];
    Arena CopyArena;
} TilemapCursor;

typedef enum TileRotation { ROT_UP, ROT_RIGHT, ROT_BOT, ROT_LEFT } TileRotation;

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

typedef struct {
    Vector2 anchor01;

    Rectangle ScrollPanelScrollView;
    Vector2   ScrollPanelScrollOffset;
    Vector2   ScrollPanelBoundsOffset;
    bool      RebindButtons[ACTION_COUNT * MAX_KEYBINDS];

    Rectangle layoutRecs[4];
} KeybindMenuState;

typedef struct {
    Window Window;

    bool TilemapPathTextBoxEditMode;
    char TilemapPathTextBoxText[256];

} PopupLoadState;

typedef struct {
    Window Window;

    bool PaintTileActive;
    bool PaintFgActive;
    bool PaintBgActive;
    bool PaintRotActive;
} ToolbarState;

typedef struct {
    Window Window;
    f32    Scale;
} TilesetState;

typedef struct {
    v2u *From, To;
    v2u  Loc, Size;
} EditEvent;

#define MAX_TILEMAPS 8

#define MAX_UNDO 64

typedef struct {
    ToolbarState  toolbar;
    TilesetState  tileset;
    TilemapCursor cursor;

    PopupNewState  popupNew;
    PopupLoadState popupLoad;

    Arena   tilemapArena;
    Tilemap tilemap[MAX_TILEMAPS];
    u8      tilemapCount;

    Arena     undoArena;
    EditEvent undo[MAX_UNDO];

    Texture bg;
    Shader  shaders[5];
} Motley;

Motley M;

KeybindMenuState NewKeybindMenu() {
    KeybindMenuState state = {};

    state.layoutRecs[0] = (Rectangle){state.anchor01.x + 0, state.anchor01.y + 0, 264, 264};
    state.layoutRecs[1] = (Rectangle){state.anchor01.x + 32, state.anchor01.y + 24, 120, 24};
    state.layoutRecs[2] = (Rectangle){state.anchor01.x + 104, state.anchor01.y + 24, 48, 24};

    return state;
}

void KeybindMenu(KeybindMenuState* state) {
    GuiScrollPanel((Rectangle){state->layoutRecs[0].x, state->layoutRecs[0].y,
                               state->layoutRecs[0].width - state->ScrollPanelBoundsOffset.x,
                               state->layoutRecs[0].height - state->ScrollPanelBoundsOffset.y},
                   "", state->layoutRecs[0], &state->ScrollPanelScrollOffset,
                   &state->ScrollPanelScrollView);

    char buf[8];
    for (u32 i = 0; i < ACTION_COUNT * MAX_KEYBINDS; i += MAX_KEYBINDS) {
        sprintf(buf, "%d", i);

        GuiLabel((Rectangle){.x = 0, .y = 0 + i * 24, .width = 48, .height = 24}, buf);

        state->RebindButtons[i]     = GuiButton((Rectangle){.x      = state->layoutRecs[2].x,
                                                            .y      = state->layoutRecs[2].y + i * 8,
                                                            .width  = 48,
                                                            .height = 24},
                                                buf);
        state->RebindButtons[i + 1] = GuiButton((Rectangle){.x     = state->layoutRecs[2].x + 24,
                                                            .y     = state->layoutRecs[2].y + i * 8,
                                                            .width = 48,
                                                            .height = 24},
                                                buf);
        state->RebindButtons[i + 2] = GuiButton((Rectangle){.x     = state->layoutRecs[2].x + 48,
                                                            .y     = state->layoutRecs[2].y + i * 8,
                                                            .width = 48,
                                                            .height = 24},
                                                buf);
        state->RebindButtons[i + 3] = GuiButton((Rectangle){.x     = state->layoutRecs[2].x + 72,
                                                            .y     = state->layoutRecs[2].y + i * 8,
                                                            .width = 48,
                                                            .height = 24},
                                                buf);
    }
}

void RebindMenu(KeybindMenuState* state) {
    for (u32 i = 0; i < ACTION_COUNT * MAX_KEYBINDS; i++) {
        if (state->RebindButtons[i]) {
            Keys[(u32)floorf(i / 4)][i % 4] = (Keybind){INPUT_TYPE_Keyboard, 0};
        }
    }
}

v2u GetHoveredTile(const Tilemap* map) {
    v2 tAnchor = Vector2Add(map->Window.Anchor, (v2){0, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT});
    return (v2u){(GetMouseX() - tAnchor.x) / map->tileSize.x,
                 (GetMouseY() - tAnchor.y) / map->tileSize.y};
}

void DrawTileCursor(const Tilemap* map, const TilemapCursor* cursor, const ToolbarState* toolbar,
                    Shader shaders[2]) {
    SetMouseCursor(cursor->InMap && cursor->State != CURSOR_STATE_BOXING ? MOUSE_CURSOR_CROSSHAIR
                                                                         : MOUSE_CURSOR_DEFAULT);
    if (!cursor->InMap || cursor->State == CURSOR_STATE_BOXING || !map->Window.Active) return;

    v2  selected = toolbar->PaintTileActive ? (v2){map->tileSize.x * cursor->Selected.x,
                                                   map->tileSize.y * cursor->Selected.y}
                                            : (v2){};
    f32 rotation = toolbar->PaintRotActive ? cursor->Rot * 90.0f : 0.0f;

    v2  tAnchor     = Vector2Add(map->Window.Anchor, (v2){0, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT});
    v2u hoveredTile = GetHoveredTile(map);
    v2  halfTile    = {map->tileSize.x * 0.5f, map->tileSize.y * 0.5f};

    if (toolbar->PaintFgActive) {
        BeginShaderMode(shaders[0]);
        DrawTexturePro(map->Tileset,
                       (Rectangle){selected.x, selected.y, map->tileSize.x, map->tileSize.y},
                       (Rectangle){hoveredTile.x * map->tileSize.x + tAnchor.x + halfTile.x,
                                   hoveredTile.y * map->tileSize.y + tAnchor.y + halfTile.y,
                                   map->tileSize.x, map->tileSize.y},
                       halfTile, rotation, map->Palette[cursor->FG]);
        EndShaderMode();
    }

    if (toolbar->PaintBgActive) {
        BeginShaderMode(shaders[1]);
        DrawTexturePro(map->Tileset,
                       (Rectangle){selected.x, selected.y, map->tileSize.x, map->tileSize.y},
                       (Rectangle){hoveredTile.x * map->tileSize.x + tAnchor.x + halfTile.x,
                                   hoveredTile.y * map->tileSize.y + tAnchor.y + halfTile.y,
                                   map->tileSize.x, map->tileSize.y},
                       halfTile, rotation, map->Palette[cursor->BG]);
        EndShaderMode();
    }
}

struct {
    TilemapLayer Data[MAX_LAYERS];
    v2u          Len;
} CopyData = {};

void CopyBoxedTiles(const Tilemap* map, const TilemapCursor* cursor) {
    if (CopyData.Data[0].Tile) free(CopyData.Data[0].Tile);
    if (CopyData.Data[0].FG) free(CopyData.Data[0].FG);
    if (CopyData.Data[0].BG) free(CopyData.Data[0].BG);
    if (CopyData.Data[0].Rot) free(CopyData.Data[0].Rot);

    v2u startBox = (v2u){
        cursor->Box[0].x < cursor->Box[1].x ? cursor->Box[0].x : cursor->Box[1].x,
        cursor->Box[0].y < cursor->Box[1].y ? cursor->Box[0].y : cursor->Box[1].y,
    };
    v2u endBox = (v2u){
        cursor->Box[0].x >= cursor->Box[1].x ? cursor->Box[0].x : cursor->Box[1].x,
        cursor->Box[0].y >= cursor->Box[1].y ? cursor->Box[0].y : cursor->Box[1].y,
    };

    CopyData.Len = (v2u){endBox.x - startBox.x, endBox.y - startBox.y};

    u32 size              = CopyData.Len.x * CopyData.Len.y;
    CopyData.Data[0].Tile = MALLOC_T(v2u, size);
    CopyData.Data[0].FG   = MALLOC_T(u8, size);
    CopyData.Data[0].BG   = MALLOC_T(u8, size);
    CopyData.Data[0].Rot  = MALLOC_T(u8, size);

    // Crashes when cycling halfway through hehe
    // CopyData.Data[0].Tile = AALLOC(&cursor->CopyArena, v2u, size);
    // CopyData.Data[0].FG   = AALLOC(&cursor->CopyArena, u8, size);
    // CopyData.Data[0].BG   = AALLOC(&cursor->CopyArena, u8, size);
    // CopyData.Data[0].Rot  = AALLOC(&cursor->CopyArena, u8, size);

    for (u32 y = startBox.y; y < endBox.y; y++) {
        for (u32 x = startBox.x; x < endBox.x; x++) {
            u32 toTile   = (y - startBox.y) * CopyData.Len.x + (x - startBox.x);
            u32 fromTile = y * map->Size.x + x;
            printf("Copying from %u to %u\n", fromTile, toTile);

            CopyData.Data[0].Tile[toTile] = map->Layer[0].Tile[fromTile];
            CopyData.Data[0].FG[toTile]   = map->Layer[0].FG[fromTile];
            CopyData.Data[0].BG[toTile]   = map->Layer[0].BG[fromTile];
            CopyData.Data[0].Rot[toTile]  = map->Layer[0].Rot[fromTile];
        }
    }
}

void DeleteBoxedTiles(Tilemap* map, const TilemapCursor* cursor) {
    v2u from = {cursor->Box[0].x < cursor->Box[1].x ? cursor->Box[0].x : cursor->Box[1].x,
                cursor->Box[0].y < cursor->Box[1].y ? cursor->Box[0].y : cursor->Box[1].y};
    v2u to = {cursor->Box[0].x >= cursor->Box[1].x ? cursor->Box[0].x : cursor->Box[1].x,
              cursor->Box[0].y >= cursor->Box[1].y ? cursor->Box[0].y : cursor->Box[1].y};

    for (u32 x = from.x; x < to.x; x++) {
        for (u32 y = from.y; y < to.y; y++) {
            u32 coord                 = y * map->Size.x + x;
            map->Layer[0].FG[coord]   = 0;
            map->Layer[0].BG[coord]   = 0;
            map->Layer[0].Tile[coord] = (v2u){};
            map->Layer[0].Rot[coord]  = ROT_UP;
        }
    }
}

void PasteBoxedTiles(Tilemap* map, const TilemapCursor* cursor, const ToolbarState* toolbar) {
    v2u hoveredTile = GetHoveredTile(map);

    for (u32 x = 0; x < CopyData.Len.x; x++) {
        for (u32 y = 0; y < CopyData.Len.y; y++) {
            if (hoveredTile.y + y >= map->Size.y) continue;
            if (hoveredTile.x + x >= map->Size.x) continue;

            u32 coordTo   = (hoveredTile.y + y) * map->Size.x + (hoveredTile.x + x);
            u32 coordFrom = y * (CopyData.Len.x) + x;

            if (toolbar->PaintTileActive)
                map->Layer[0].Tile[coordTo] = CopyData.Data[0].Tile[coordFrom];
            if (toolbar->PaintFgActive) map->Layer[0].FG[coordTo] = CopyData.Data[0].FG[coordFrom];
            if (toolbar->PaintBgActive) map->Layer[0].BG[coordTo] = CopyData.Data[0].BG[coordFrom];
            if (toolbar->PaintRotActive)
                map->Layer[0].Rot[coordTo] = CopyData.Data[0].Rot[coordFrom];
        }
    }
}

void SetFgColor(u8 id, TilemapCursor* cursor, const Tilemap* map, ToolbarState* toolbar) {
    u8 newFg   = id % map->PaletteSize;
    cursor->FG = newFg != cursor->BG ? newFg : ((newFg + 1) % map->PaletteSize);
}

void SetBgColor(u8 id, TilemapCursor* cursor, const Tilemap* map, ToolbarState* toolbar) {
    u8 newBg   = id % map->PaletteSize;
    cursor->BG = newBg != cursor->FG ? newBg : ((newBg + 1) % map->PaletteSize);
}

void SwapColors(TilemapCursor* cursor) {
    u8 swap    = cursor->FG;
    cursor->FG = cursor->BG;
    cursor->BG = swap;
}

void UpdateTileCursor(Tilemap* map, TilemapCursor* cursor, ToolbarState* toolbar) {
    if (!map->Window.Active || GlobalGuiState.Editing || GlobalGuiState.Moving) return;

    v2 halfTile = {map->tileSize.x * 0.5f, map->tileSize.y * 0.5f};
    v2 mousePos = GetMousePosition();

    cursor->InMap = CheckCollisionPointRec(
        mousePos, (Rectangle){map->Window.Anchor.x, map->Window.Anchor.y + 24,
                              map->Size.x * map->tileSize.x, map->Size.y * map->tileSize.y});

    v2 absPos = Vector2Subtract(
        mousePos, Vector2Add(map->Window.Anchor, (v2){0, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT}));
    v2i coords = {absPos.x / map->tileSize.x, absPos.y / map->tileSize.y};

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && cursor->InMap) {
        i32 i = (coords.y * map->Size.x + coords.x) * cursor->InMap;

        if (toolbar->PaintTileActive)
            map->Layer[map->SelectedLayer].Tile[i] = (v2u){cursor->Selected.x, cursor->Selected.y};
        if (toolbar->PaintFgActive) map->Layer[map->SelectedLayer].FG[i] = cursor->FG;
        if (toolbar->PaintBgActive) map->Layer[map->SelectedLayer].BG[i] = cursor->BG;
        if (toolbar->PaintRotActive) map->Layer[map->SelectedLayer].Rot[i] = cursor->Rot;
    }

    // ===== PALETTE BUTTONS =====
    if (IsActionPressed(ACTION_CURSOR_NEXT_LAYER))
        map->SelectedLayer = (map->SelectedLayer + 1) % MAX_LAYERS;

    if (IsActionPressed(ACTION_LEFT) && cursor->Selected.x != 0) cursor->Selected.x -= 1;
    if (IsActionPressed(ACTION_RIGHT) &&
        cursor->Selected.x < map->Tileset.width / map->tileSize.x - 1)
        cursor->Selected.x += 1;
    if (IsActionPressed(ACTION_UP) && cursor->Selected.y != 0) cursor->Selected.y -= 1;
    if (IsActionPressed(ACTION_DOWN) &&
        cursor->Selected.y < map->Tileset.height / map->tileSize.y - 1)
        cursor->Selected.y += 1;

    if (IsKeyPressed(KEY_R)) cursor->Rot = (cursor->Rot + 1) % 4;
    if (IsKeyPressed(KEY_F)) SetFgColor(cursor->FG + 1, cursor, map, toolbar);
    if (IsKeyPressed(KEY_B)) SetBgColor(cursor->BG + 1, cursor, map, toolbar);
    if (IsKeyPressed(KEY_G)) SwapColors(cursor);
}

void UpdateBoxSelect(Tilemap* mapFrom, Tilemap* mapTo, TilemapCursor* cursor,
                     const ToolbarState* toolbar) {
    if (IsActionPressed(ACTION_COPY) && cursor->State == CURSOR_STATE_BOXED) {
        cursor->State = CURSOR_STATE_SINGLE;

        CopyBoxedTiles(mapFrom, cursor);
    }

    if (IsActionPressed(ACTION_CUT) && cursor->State == CURSOR_STATE_BOXED) {
        cursor->State = CURSOR_STATE_SINGLE;

        CopyBoxedTiles(mapFrom, cursor);
        DeleteBoxedTiles(mapFrom, cursor);
    }

    if (cursor->State == CURSOR_STATE_BOXING && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        cursor->State = CURSOR_STATE_BOXED;
    }

    if (!cursor->InMap) return;

    v2  tAnchor = Vector2Add(mapFrom->Window.Anchor, (v2){0, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT});
    v2u hoveredTile = (v2u){(GetMouseX() - tAnchor.x) / mapFrom->tileSize.x,
                            (GetMouseY() - tAnchor.y) / mapFrom->tileSize.y};

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        cursor->State  = CURSOR_STATE_BOXING;
        cursor->Box[0] = hoveredTile;
    }

    if (cursor->State == CURSOR_STATE_BOXING) {
        cursor->Box[1] = (v2u){hoveredTile.x + 1, hoveredTile.y + 1};
    }

    if (cursor->State == CURSOR_STATE_BOXED && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        cursor->State = CURSOR_STATE_SINGLE;
    }

    if (IsActionPressed(ACTION_PASTE) && cursor->State != CURSOR_STATE_BOXING) {
        PasteBoxedTiles(mapTo, cursor, toolbar);
    }
}

void DrawBoxCursor(const Tilemap* map, const TilemapCursor* cursor) {
    if (cursor->State == CURSOR_STATE_SINGLE) return;

    v2  tAnchor = Vector2Add(map->Window.Anchor, (v2){0, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT});
    v2u start   = {cursor->Box[0].x * map->tileSize.x + tAnchor.x,
                   cursor->Box[0].y * map->tileSize.y + tAnchor.y};
    v2u end     = {cursor->Box[1].x * map->tileSize.x + tAnchor.x,
                   cursor->Box[1].y * map->tileSize.y + tAnchor.y};

    DrawRectangleLinesEx(
        (Rectangle){start.x < end.x ? start.x : end.x, start.y < end.y ? start.y : end.y,
                    start.x < end.x ? end.x - start.x : start.x - end.x,
                    start.y < end.y ? end.y - start.y : start.y - end.y},
        2, GRAY);
}

PopupLoadState InitPopupLoad() {
    PopupLoadState state = {
        .Window = {.Size = {168, 112}, .Active = false, .Title = "Load Tilemap"}};

    state.Window.Anchor = (v2){GetScreenWidth() / 2 - state.Window.Size.x / 2,
                               GetScreenHeight() / 2 - state.Window.Size.y / 2};

    return state;
}

bool DrawPopupLoad(PopupLoadState* state) {
    static const char* LoadButtonText       = "Load";
    static const char* TilemapPathLabelText = "Path";

    if (!state->Window.Active) return false;

    UpdateWindow(&state->Window);

    if (GuiTextBox((Rectangle){state->Window.Anchor.x + 48, state->Window.Anchor.y + 32, 104, 24},
                   state->TilemapPathTextBoxText, 128, state->TilemapPathTextBoxEditMode))
        state->TilemapPathTextBoxEditMode = !state->TilemapPathTextBoxEditMode;
    GuiLabel((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 32, 32, 24},
             TilemapPathLabelText);

    if (GuiButton((Rectangle){state->Window.Anchor.x + 24, state->Window.Anchor.y + 72, 120, 24},
                  LoadButtonText)) {
        state->Window.Active = false;
        return true;
    }

    return false;
}

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

            palette[i].r = (rgb >> 16) & 0xFF;
            palette[i].g = (rgb >> 8) & 0xFF;
            palette[i].b = rgb & 0xFF;
            palette[i].a = 0xFF;
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
            map.Layer[0].BG[coord]   = 0;
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
    if (GlobalGuiState.Editing) return false;
    if (!this->Window.Active || !(IsActionPressed(ACTION_SAVE))) return false;

    FILE* file = fopen(path, "w");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Couldn't save tilemap to file");
        return false;
    }

    fprintf(file, "%d %d\n", this->Size.x, this->Size.y);

    fprintf(file, "%d %d\n", this->tileSize.x, this->tileSize.y);

    for (u32 j = 0; j < MAX_LAYERS; j++) {
        for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
            fprintf(file, "%u %u ", this->Layer[j].Tile[i].x, this->Layer[j].Tile[i].y);
        }
        fprintf(file, "\n");

        for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
            fprintf(file, "%d ", this->Layer[j].FG[i]);
        }
        fprintf(file, "\n");

        for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
            fprintf(file, "%d ", this->Layer[j].BG[i]);
        }
        fprintf(file, "\n");

        for (u32 i = 0; i < this->Size.x * this->Size.y; i++) {
            fprintf(file, "%d ", this->Layer[j].Rot[i]);
        }
        fprintf(file, "\n");
    }

    fprintf(file, "%s", this->TilesetPath);
    fprintf(file, "\n");

    fprintf(file, "%u\n", this->PaletteSize);

    for (u32 i = 0; i < this->PaletteSize; i++) {
        fprintf(file, "%d %d %d %d ", this->Palette[i].r, this->Palette[i].g, this->Palette[i].b,
                this->Palette[i].a);
    }
    fprintf(file, "\n");

    fclose(file);

    TraceLog(LOG_INFO, "Tilemap saved successfully");

    this->Window.Title = path;
    return true;
}

Tilemap LoadTilemap(PopupLoadState* state, Arena* arena) {
    FILE* file = fopen(state->TilemapPathTextBoxText, "r");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Couldn't open tilemap file");
        return (Tilemap){};
    }

    Tilemap this = {};

    fscanf(file, "%d %d", &this.Size.x, &this.Size.y);

    fscanf(file, "%d %d", &this.tileSize.x, &this.tileSize.y);

    u32 tileCount = this.Size.x * this.Size.y;

    for (u32 j = 0; j < MAX_LAYERS; j++) {
        this.Layer[j].Tile = AALLOC(arena, v2u, tileCount);
        this.Layer[j].FG   = AALLOC(arena, u8, tileCount);
        this.Layer[j].BG   = AALLOC(arena, u8, tileCount);
        this.Layer[j].Rot  = AALLOC(arena, u8, tileCount);

        for (u32 i = 0; i < tileCount; i++)
            fscanf(file, "%u %u", &this.Layer[j].Tile[i].x, &this.Layer[j].Tile[i].y);

        for (u32 i = 0; i < tileCount; i++) fscanf(file, "%hhu", &this.Layer[j].FG[i]);

        for (u32 i = 0; i < tileCount; i++) fscanf(file, "%hhu", &this.Layer[j].BG[i]);

        for (u32 i = 0; i < tileCount; i++) fscanf(file, "%hhu", &this.Layer[j].Rot[i]);
    }

    char tilesetPath[256];
    fscanf(file, "%255s", tilesetPath);
    this.TilesetPath = strdup(tilesetPath);
    this.Tileset     = LoadTexture(this.TilesetPath);

    fscanf(file, "%u", &this.PaletteSize);
    this.Palette = AALLOC(arena, Color, this.PaletteSize);
    for (u32 i = 0; i < this.PaletteSize; i++) {
        fscanf(file, "%hhu %hhu %hhu %hhu", &this.Palette[i].r, &this.Palette[i].g,
               &this.Palette[i].b, &this.Palette[i].a);
    }

    fclose(file);

    this.Window = InitWindowBox(state->TilemapPathTextBoxText,
                                (v2){this.Size.x * this.tileSize.x, this.Size.y * this.tileSize.y});

    TraceLog(LOG_INFO, "Tilemap loaded successfully");
    return this;
}

void DrawBg(Texture bg, Shader shader) {
    BeginShaderMode(shader);
    // FIXME: Texture wrap doesn't work?
    DrawTexture(bg, 0, 0, WHITE);
    DrawTexture(bg, bg.width, 0, WHITE);
    DrawTexture(bg, 0, bg.height, WHITE);
    DrawTexture(bg, bg.width, bg.height, WHITE);
    EndShaderMode();
}

void DrawTilemap(Tilemap* map, Shader shaders[2]) {
    if (!map->Window.Active) return;

    UpdateWindow(&map->Window);

    v2 tAnchor = {map->Window.Anchor.x + map->tileSize.x * 0.5f,
                  map->Window.Anchor.y + map->tileSize.y * 0.5f + 24};

    BeginShaderMode(shaders[1]);
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

    BeginShaderMode(shaders[0]);
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

ToolbarState InitToolbar(v2 anchor) {
    ToolbarState state = {.PaintFgActive   = true,
                          .PaintRotActive  = true,
                          .PaintTileActive = true,
                          .PaintBgActive   = true};

    state.Window =
        (Window){.Active = true, .Anchor = anchor, .Size = (v2){136, 344}, .Title = "Toolbar"};

    return state;
}

void UpdateToolbar(ToolbarState* state) {
    if (GlobalGuiState.Editing) return;
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_T)) state->Window.Active = true;

    if (IsActionPressed(ACTION_PAINT_TILE)) state->PaintTileActive = !state->PaintTileActive;
    if (IsActionPressed(ACTION_PAINT_FG)) state->PaintFgActive = !state->PaintFgActive;
    if (IsActionPressed(ACTION_PAINT_BG)) state->PaintBgActive = !state->PaintBgActive;
    if (IsActionPressed(ACTION_PAINT_ROT)) state->PaintRotActive = !state->PaintRotActive;
}

void DrawToolbar(ToolbarState* state, PopupNewState* newTilemapMenuState,
                 PopupLoadState* popupLoadState, Tilemap* tilemap, TilemapCursor* cursor) {
    static const char* NewButtonText  = "#08#New";
    static const char* LoadButtonText = "#05#Load";
    static const char* PaintTileText  = "#23#";
    static const char* PaintFgText    = "#165#";
    static const char* PaintBgText    = "#168#";
    static const char* PaintRotText   = "#076#";

    if (!state->Window.Active) return;

    UpdateWindow(&state->Window);

    GuiLine((Rectangle){state->Window.Anchor.x + 12, state->Window.Anchor.y + 56, 116, 16}, NULL);

    if (GuiButton((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 32, 56, 24},
                  NewButtonText))
        newTilemapMenuState->Window.Active = true;

    if (GuiButton((Rectangle){state->Window.Anchor.x + 72, state->Window.Anchor.y + 32, 56, 24},
                  LoadButtonText))
        popupLoadState->Window.Active = true;

    GuiToggle((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 72, 24, 24},
              PaintTileText, &state->PaintTileActive);
    GuiToggle((Rectangle){state->Window.Anchor.x + 40, state->Window.Anchor.y + 72, 24, 24},
              PaintFgText, &state->PaintFgActive);
    GuiToggle((Rectangle){state->Window.Anchor.x + 72, state->Window.Anchor.y + 72, 24, 24},
              PaintBgText, &state->PaintBgActive);
    GuiToggle((Rectangle){state->Window.Anchor.x + 104, state->Window.Anchor.y + 72, 24, 24},
              PaintRotText, &state->PaintRotActive);

    if (!tilemap->Window.Active) return;

    u32 i = 0;
    for (u32 y = 0; y < 4; y++) {
        for (u32 x = 0; x < 4; x++) {
            Rectangle r = {state->Window.Anchor.x + 8 + x * 32,
                           state->Window.Anchor.y + 104 + y * 32, 24, 24};

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                CheckCollisionPointRec(GetMousePosition(), r) && i != cursor->BG)
                SetFgColor(i, cursor, tilemap, state);
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) &&
                CheckCollisionPointRec(GetMousePosition(), r) && i != cursor->FG)
                SetBgColor(i, cursor, tilemap, state);

            DrawRectangleRec(r, tilemap->Palette[i]);

            if (cursor->FG == i)
                DrawRectangleLinesEx((Rectangle){r.x, r.y, r.width, r.height}, 3, GREEN);
            if (cursor->BG == i)
                DrawRectangleLinesEx((Rectangle){r.x, r.y, r.width, r.height}, 3, YELLOW);

            if (i++ >= tilemap->PaletteSize) goto OUTER_BREAK;
        }
    }
OUTER_BREAK:
    return;
}

TilesetState InitTilesetWindow(v2 pos) {
    return (TilesetState){
        .Scale = 0.5f, .Window = {.Active = true, .Anchor = pos, .Title = "Tileset", .Size = {}}};
}

void UpdateTileset(TilesetState* state, const Tilemap* map) {
    if (!state->Window.Active || !map->Window.Active) return;

    // 24 = mrmo-x3 size
    state->Scale = map->tileSize.x >= 24 ? 0.5f : 1.0f;

    state->Window.Size =
        (v2){map->Tileset.width * state->Scale,
             map->Tileset.height * state->Scale + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT};
}

void DrawTileset(TilesetState* state, const Tilemap* map, TilemapCursor* cursor,
                 Shader shaders[2]) {
    if (!state->Window.Active || !map->Window.Active) return;

    UpdateWindow(&state->Window);

    v2 tAnchor = Vector2Add(state->Window.Anchor, (v2){0, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT});

    BeginShaderMode(shaders[0]);
    DrawTextureEx(map->Tileset, tAnchor, 0.0f, state->Scale, map->Palette[cursor->FG]);
    EndShaderMode();

    BeginShaderMode(shaders[1]);
    DrawTextureEx(map->Tileset, tAnchor, 0.0f, state->Scale, map->Palette[cursor->BG]);
    EndShaderMode();

    // TILE SELECTION BOXES
    v2u scaledTile = (v2u){map->tileSize.x * state->Scale, map->tileSize.y * state->Scale};

    DrawRectangleLines((u32)(cursor->Selected.x * scaledTile.x) + tAnchor.x + 1,
                       (u32)(cursor->Selected.y * scaledTile.y) + tAnchor.y + 1, scaledTile.x,
                       scaledTile.y, GREEN);

    if (CheckCollisionPointRec(GetMousePosition(),
                               (Rectangle){tAnchor.x, tAnchor.y, map->Tileset.width * state->Scale,
                                           map->Tileset.height * state->Scale})) {
        v2u hoveredTile = {(GetMouseX() - tAnchor.x) / scaledTile.x,
                           (GetMouseY() - tAnchor.y) / scaledTile.y};

        DrawRectangleLines(hoveredTile.x * scaledTile.x + tAnchor.x + 1,
                           hoveredTile.y * scaledTile.y + tAnchor.y + 1, scaledTile.x, scaledTile.y,
                           BLUE);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            cursor->Selected = (v2i){hoveredTile.x, hoveredTile.y};
        }
    }
}

Image ExportTilemap(const Tilemap* map) {
    Image result =
        GenImageColor(map->Size.x * map->tileSize.x, map->Size.y * map->tileSize.y, BLANK);
    Image tileset = LoadImageFromTexture(map->Tileset);

    for (u32 y = 0; y < result.height; y++) {
        for (u32 x = 0; x < result.width; x++) {
            u32 mapCoord = (y / map->tileSize.y) * map->Size.x + (x / map->tileSize.x);

            v2u tilesetPixel = {
                map->Layer[0].Tile[mapCoord].x * map->tileSize.x + (x % map->tileSize.x),
                map->Layer[0].Tile[mapCoord].y * map->tileSize.y + (y % map->tileSize.y)};

            // TODO
            switch (map->Layer[0].Rot[mapCoord]) {
                case ROT_UP:
                    break;

                case ROT_RIGHT:
                    break;

                case ROT_BOT:
                    break;

                case ROT_LEFT:
                    break;
            }

            Color color = GetImageColor(tileset, tilesetPixel.x, tilesetPixel.y);

            for (u32 i = 0; i < 1 /* MAX_LAYERS */; i++) {
                u8 fgColor = map->Layer[i].FG[mapCoord];
                u8 bgColor = map->Layer[i].BG[mapCoord];

                // Assumes the tileset is B&W
                if (color.r == 0) {
                    if (bgColor != 0) ImageDrawPixel(&result, x, y, map->Palette[bgColor]);
                } else {
                    if (fgColor != 0) ImageDrawPixel(&result, x, y, map->Palette[fgColor]);
                }
            }
        }
    }

    return result;
}

void SetBackgroundColor(Motley* Motley, v3 fg, v3 bg) {
    SetShaderValue(Motley->shaders[4], GetShaderLocation(Motley->shaders[4], "color1"), &fg,
                   SHADER_UNIFORM_VEC3);
    SetShaderValue(Motley->shaders[4], GetShaderLocation(Motley->shaders[4], "color2"), &bg,
                   SHADER_UNIFORM_VEC3);
}

void InitMotley() {
    M = (Motley){
        .toolbar   = InitToolbar((v2){GetScreenWidth() * 0.025f, GetScreenHeight() * 0.05f}),
        .tileset   = InitTilesetWindow((v2){GetScreenWidth() * 0.025f, GetScreenHeight() * 0.6f}),
        .cursor    = (TilemapCursor){.FG = 2, .BG = 0, .CopyArena = InitArena(250 << 10)},
        .popupNew  = InitNewTilemapMenu(),
        .popupLoad = InitPopupLoad(),
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
    SetBackgroundColor(&M, (v3){0.34, 0.34, 0.34}, (v3){0.55, 0.55, 0.55});
}

void UpdateMotley() {
    for (u32 i = 0; i < M.tilemapCount; i++) {
        UpdateTileCursor(&M.tilemap[i], &M.cursor, &M.toolbar);
        UpdateBoxSelect(&M.tilemap[i], &M.tilemap[i], &M.cursor, &M.toolbar);
    }
    UpdateToolbar(&M.toolbar);
    UpdateTileset(&M.tileset, M.tilemap);

    SaveTilemap(M.tilemap, "tilemap.txt");
    if (IsActionPressed(ACTION_EXPORT)) ExportImage(ExportTilemap(&M.tilemap[0]), "tilemap.png");

    // === DRAW  ===
    BeginDrawing();

    DrawBg(M.bg, M.shaders[4]);

    for (u32 i = 0; i < M.tilemapCount; i++) {
        DrawTilemap(&M.tilemap[i], (Shader[2]){M.shaders[0], M.shaders[2]});
        DrawTileCursor(&M.tilemap[i], &M.cursor, &M.toolbar,
                       (Shader[2]){M.shaders[1], M.shaders[3]});
        DrawBoxCursor(&M.tilemap[i], &M.cursor);
    }

    DrawToolbar(&M.toolbar, &M.popupNew, &M.popupLoad, M.tilemap, &M.cursor);
    DrawTileset(&M.tileset, M.tilemap, &M.cursor, (Shader[2]){M.shaders[0], M.shaders[2]});

    if (DrawNewTilemapMenu(&M.popupNew)) {
        M.tilemap[M.tilemapCount++] = InitTilemap(&M.popupNew, &M.tilemapArena);
    }
    if (DrawPopupLoad(&M.popupLoad)) {
        M.tilemap[M.tilemapCount++] = LoadTilemap(&M.popupLoad, &M.tilemapArena);
    }

    EndDrawing();
}

void CloseMotley() {
    free(M.tilemapArena.buffer);
    free(M.undoArena.buffer);
    free(M.cursor.CopyArena.buffer);
}
