#pragma once

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#include "../game.h"
#include "../types.h"
#include "raylib.h"
#include "tilemap.h"
#include "toolbar.h"

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
    v2u hoveredTile = (v2u){(GetMouseX() - tAnchor.x) / map->tileSize.x,
                            (GetMouseY() - tAnchor.y) / map->tileSize.y};
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
    v2u          Start, End;
} CopyData = {};

void CopyBoxedTiles(const Tilemap* map, const TilemapCursor* cursor) {}

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

void PasteBoxedTiles(Tilemap* map, const TilemapCursor* cursor) {
    for (u32 x = CopyData.Start.x; x < CopyData.End.x; x++) {
        for (u32 y = CopyData.Start.y; x < CopyData.End.y; y++) {
            u32 coordTo                 = y * map->Size.x + x;
            u32 coordFrom               = y * (CopyData.End.x - CopyData.Start.x) + x;
            map->Layer[0].Tile[coordTo] = CopyData.Data[0].Tile[coordFrom];
            map->Layer[0].FG[coordTo]   = CopyData.Data[0].FG[coordFrom];
            map->Layer[0].BG[coordTo]   = CopyData.Data[0].BG[coordFrom];
            map->Layer[0].Rot[coordTo]  = CopyData.Data[0].Rot[coordFrom];
        }
    }
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
    if (IsKeyPressed(KEY_FIVE)) map->SelectedLayer = map->SelectedLayer + 1 % MAX_LAYERS;

    if (IsActionPressed(ACTION_LEFT) && cursor->Selected.x != 0) cursor->Selected.x -= 1;
    if (IsActionPressed(ACTION_RIGHT) &&
        cursor->Selected.x < map->Tileset.width / map->tileSize.x - 1)
        cursor->Selected.x += 1;
    if (IsActionPressed(ACTION_UP) && cursor->Selected.y != 0) cursor->Selected.y -= 1;
    if (IsActionPressed(ACTION_DOWN) &&
        cursor->Selected.y < map->Tileset.height / map->tileSize.y - 1)
        cursor->Selected.y += 1;
    if (IsKeyPressed(KEY_R)) cursor->Rot = (cursor->Rot + 1) % 4;

    if (IsKeyPressed(KEY_F)) {
        u8 newFg                    = (cursor->FG + 1) % map->PaletteSize;
        cursor->FG                  = newFg != cursor->BG ? newFg : (newFg + 1) % map->PaletteSize;
        toolbar->ColorPickerFgValue = map->Palette[cursor->FG];
    }
    if (IsKeyPressed(KEY_B)) {
        u8 newBg                    = (cursor->BG + 1) % map->PaletteSize;
        cursor->BG                  = newBg != cursor->FG ? newBg : (newBg + 1) % map->PaletteSize;
        toolbar->ColorPickerBgValue = map->Palette[cursor->BG];
    }
    if (IsKeyPressed(KEY_G)) {
        u8 swap    = cursor->FG;
        cursor->FG = cursor->BG;
        cursor->BG = swap;
    }
}

void UpdateBoxSelect(Tilemap* mapFrom, Tilemap* mapTo, TilemapCursor* cursor) {
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

    if (cursor->State == CURSOR_STATE_BOXING && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        cursor->State = CURSOR_STATE_BOXED;
    }

    if (cursor->State == CURSOR_STATE_BOXED && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        cursor->State = CURSOR_STATE_SINGLE;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C) && cursor->State == CURSOR_STATE_BOXED) {
        cursor->State = CURSOR_STATE_SINGLE;

        // CopyBoxedTiles(mapFrom, cursor);
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_X) && cursor->State == CURSOR_STATE_BOXED) {
        cursor->State = CURSOR_STATE_SINGLE;

        // CopyBoxedTiles(mapFrom, cursor);
        DeleteBoxedTiles(mapFrom, cursor);
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V) && cursor->BoxSelection &&
        cursor->State != CURSOR_STATE_BOXING) {
        PasteBoxedTiles(mapTo, cursor);
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
