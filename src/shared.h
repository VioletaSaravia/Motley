#pragma once

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#include "raylib.h"
#include "raymath.h"

#define MALLOC_T(t, n) ((t*)malloc(sizeof(t) * n))

#ifdef __cplusplus
#define TYPE(name) name
#else
#define TYPE(name) (name)
#endif

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef double_t f64;
typedef float_t  f32;

typedef Vector3 v3;
typedef Vector2 v2;

typedef struct {
    i32 x, y, z;
} v3i;

typedef struct {
    u32 x, y, z;
} v3u;

typedef struct {
    i32 x, y;
} v2i;

typedef struct {
    u32 x, y;
} v2u;

#define TAU (PI * 2.0f)

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

typedef struct {
    bool started, enabled;
    f32  freq, damp, resp;  // NOTE: Do not set manually, use f32dSet()
    f32  _xp, _yd, _y;      // _y == current value
    f32  _k1, _k2, _k3;
} f32d;

void f32dCalculateK(f32d* this) {
    this->_k1 = this->damp / (PI * this->freq);
    this->_k2 = 1 / ((2 * PI * this->freq) * (2 * PI * this->freq));
    this->_k3 = this->resp * this->damp / (2 * PI * this->freq);
}

f32d f32dInit(f32 x0) {
    f32d this = (f32d){.started = true,
                       .enabled = true,
                       .freq    = 0.25f,
                       .damp    = 1.0f,
                       .resp    = 0.0f,
                       ._xp     = x0,
                       ._yd     = 0.0f,
                       ._y      = x0,
                       ._k1     = 0.0f,
                       ._k2     = 0.0f,
                       ._k3     = 0.0f};

    f32dCalculateK(&this);

    return this;
}

void f32dSetParameters(f32d* this, f32 freq, f32 damp, f32 resp) {
    this->freq = freq;
    this->damp = damp;
    this->resp = resp;

    f32dCalculateK(this);
}

void f32dUpdate(f32d* this, f32 x) {
    f64 delta = GetFrameTime();

    if (!this->enabled || delta == 0.0f) {
        this->_y = x;
        return;
    }

    f32 xd    = (x - this->_xp) / delta;
    this->_xp = x;

    this->_y = this->_y + delta * this->_yd;

    f32 k2_stable = MAX(this->_k2, 1.1f * (delta * delta / 4 + delta * this->_k1 / 2));
    this->_yd =
        this->_yd + delta * (x + this->_k3 * xd - this->_y - this->_k1 * this->_yd) / k2_stable;
}

v2 PolarToCartesian(f32 radius, f32 angle) {
    return (v2){radius * cos(angle), radius * sin(angle)};
}

v2 CartesianToPolar(v2 coords) {
    return (v2){sqrtf(pow(coords.x, 2) + pow(coords.y, 2)), atanf(coords.y / coords.x)};
}

v3 CalculateOrbit3D(f32 radius, f32 yaw, f32 pitch) {
    v3 ray = {0, 0, 1};

    ray = Vector3RotateByQuaternion(ray, QuaternionFromAxisAngle((v3){0, 1, 0}, TAU - yaw));

    v3 pitchAxis = Vector3CrossProduct(ray, (v3){0, 1, 0});
    ray          = Vector3RotateByQuaternion(ray, QuaternionFromAxisAngle(pitchAxis, TAU - pitch));
    return Vector3Scale(ray, -radius);
}

// === STRINGS ===

bool EndsWith(const char* str, const char* suffix) {
    u64 len        = strlen(str);
    u64 suffix_len = strlen(suffix);

    if (len < suffix_len) {
        return false;
    }

    return strcmp(str + len - suffix_len, suffix) == 0;
}

i32 CountLines(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        TraceLog(LOG_ERROR, "Can't open file");
        return -1;
    }

    i32  line_count = 0;
    char ch;

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            line_count++;
        }
    }

    fclose(file);
    return line_count;
}

// === COLLISION ===

Rectangle CheckOverlapRecs(Rectangle r1, Rectangle r2) {
    float x      = fmaxf(r1.x, r2.x);
    float y      = fmaxf(r1.y, r2.y);
    float width  = fminf(r1.x + r1.width, r2.x + r2.width);
    float height = fminf(r1.y + r1.height, r2.y + r2.height);

    return (x > width || y > height) ? (Rectangle){} : (Rectangle){x, y, width - x, height - y};
}

// === MEMORY ===

#define DEFAULT_ARENA 1024

typedef struct {
    u8* buffer;
    u64 offset;
    u64 size;
} Arena;

Arena InitArena(u64 size) {
    if (!size) size = DEFAULT_ARENA;
    return (Arena){.buffer = MALLOC_T(u8, size), .offset = 0, .size = size};
}

void* ArenaAlloc(Arena* arena, u64 size) {
    if (arena->offset + size > arena->size) arena->offset = 0;

    void* ptr = &arena->buffer[arena->offset];

    arena->offset += size;

    return ptr;
}

#define AALLOC(arena, type, size) ((type*)ArenaAlloc(arena, sizeof(type) * size))

// === SETUP ===

typedef struct {
    u32   screenWidth, screenHeight;
    u32   targetFPS;
    char* title;
    u32   ConfigFlags;
    u32   WindowFlags;
    char* iconPath;
    char* guiStylePath;
} WindowOptions;

void InitGameWindow(WindowOptions* opts) {
    SetConfigFlags(opts->ConfigFlags);
    GuiLoadStyle(opts->guiStylePath);

    InitWindow(opts->screenWidth, opts->screenHeight, opts->title);
    SetWindowState(opts->WindowFlags);
#ifndef PLATFORM_WEB
    SetWindowIcon(LoadImage(opts->iconPath));
#endif
}

void SetGameWindow(WindowOptions* opts) {}

// === KEYBINDS ===

typedef enum {
    // Basic keys
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_ACCEPT,
    ACTION_CANCEL,
    ACTION_MENU,
    ACTION_SELECT,

    // Tile painting keys
    ACTION_PAINT_TILE,
    ACTION_PAINT_FG,
    ACTION_PAINT_BG,
    ACTION_PAINT_ROT,
    ACTION_CURSOR_NEXT_FG_COLOR,
    ACTION_CURSOR_NEXT_BG_COLOR,
    ACTION_CURSOR_SWAP_COLORS,
    ACTION_CURSOR_ROTATE,
    ACTION_CURSOR_NEXT_LAYER,

    // Motley
    ACTION_SAVE,
    ACTION_LOAD,
    ACTION_EXPORT,

    // Editing keys
    ACTION_CUT,
    ACTION_COPY,
    ACTION_PASTE,

    // Camera keys
    ACTION_CAM_LEFT,
    ACTION_CAM_RIGHT,
    ACTION_CAM_UP,
    ACTION_CAM_DOWN,
    ACTION_CAM_ZOOM_IN,
    ACTION_CAM_ZOOM_OUT,
    ACTION_CAM_ZOOM_RESET,

    ACTION_COUNT
} Action;

typedef enum {
    INPUT_TYPE_Keyboard,
    INPUT_TYPE_Mouse,
    INPUT_TYPE_Gamepad1,
    INPUT_TYPE_Gamepad2,
    INPUT_TYPE_Gamepad3,
    INPUT_TYPE_Gamepad4  // etc.
} InputType;

#define MAX_MODS 4

typedef struct {
    InputType Type;
    i32       Key;
    i32       Mods[MAX_MODS];
} Keybind;

#define MAX_KEYBINDS 3

static Keybind Keys[ACTION_COUNT][MAX_KEYBINDS] = {
    {{0, KEY_A}, {0, KEY_LEFT}},
    {{0, KEY_D}, {0, KEY_RIGHT}},
    {{0, KEY_W}, {0, KEY_UP}},
    {{0, KEY_S}, {0, KEY_DOWN}},
    {{0, KEY_C}, {1, MOUSE_BUTTON_LEFT}},
    {{0, KEY_V}, {1, MOUSE_BUTTON_RIGHT}},
    {{0, KEY_ENTER}, {0, KEY_MENU}},
    {{2, GAMEPAD_BUTTON_MIDDLE_LEFT}},
    {{0, KEY_ONE}},
    {{0, KEY_TWO}},
    {{0, KEY_THREE}},
    {{0, KEY_FOUR}},
    {{0, KEY_F}},
    {{0, KEY_B}},
    {{0, KEY_G}},
    {{0, KEY_R}},
    {{0, KEY_T}},
    {{0, KEY_S, {KEY_LEFT_CONTROL}}},
    {},
    {{0, KEY_E, {KEY_LEFT_CONTROL}}},
    {{0, KEY_X, {KEY_LEFT_CONTROL}}},
    {{0, KEY_C, {KEY_LEFT_CONTROL}}},
    {{0, KEY_V, {KEY_LEFT_CONTROL}}},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
};

bool IsActionPressed(Action action) {
    for (u32 i = 0; i < MAX_KEYBINDS; i++) {
        Keybind key = Keys[action][i];

        for (u32 j = 0; j < MAX_MODS; j++) {
            if (key.Mods[j] != 0 && !IsKeyDown(key.Mods[j])) goto OUTER_CONTINUE;
        }

        switch (key.Type) {
            case INPUT_TYPE_Keyboard:
                if (IsKeyPressed(key.Key)) return true;
                break;
            case INPUT_TYPE_Mouse:
                if (IsMouseButtonPressed(key.Key)) return true;
                break;
            default:
                if (IsGamepadButtonPressed(key.Type - 2, key.Key)) return true;
                break;
        }

    OUTER_CONTINUE:
        continue;
    }
    return false;
}

bool IsActionPressedRepeat(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++) {
        Keybind key = Keys[action][i];

        for (u32 j = 0; j < MAX_MODS; j++) {
            if (!IsKeyDown(key.Mods[j])) continue;
        }

        switch (key.Type) {
            case INPUT_TYPE_Keyboard:
                if (IsKeyPressedRepeat(key.Key)) return true;
                break;
            case INPUT_TYPE_Mouse:
                if (IsMouseButtonPressed(key.Key)) return true;  // Huh?
                break;
            default:
                if (IsGamepadButtonPressed(key.Type - 2, key.Key)) return true;  // Huh x2?
                break;
        }
    }
    return false;
}

bool IsActionReleased(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++) {
        Keybind key = Keys[action][i];

        for (u32 j = 0; j < MAX_MODS; j++) {
            if (!IsKeyDown(key.Mods[j])) continue;
        }

        switch (key.Type) {
            case INPUT_TYPE_Keyboard:
                if (IsKeyReleased(key.Key)) return true;
                break;
            case INPUT_TYPE_Mouse:
                if (IsMouseButtonReleased(key.Key)) return true;
                break;
            default:
                if (IsGamepadButtonReleased(key.Type - 2, key.Key)) return true;
                break;
        }
    }
    return false;
}

bool IsActionUp(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++) {
        Keybind key = Keys[action][i];
        switch (key.Type) {
            case INPUT_TYPE_Keyboard:
                if (IsKeyUp(key.Key)) return true;
                break;
            case INPUT_TYPE_Mouse:
                if (IsMouseButtonUp(key.Key)) return true;
                break;
            default:
                if (IsGamepadButtonUp(key.Type - 2, key.Key)) return true;
                break;
        }
    }
    return false;
}

bool IsActionDown(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++) {
        Keybind key = Keys[action][i];

        for (u32 j = 0; j < MAX_MODS; j++) {
            if (!IsKeyDown(key.Mods[j])) continue;
        }

        switch (key.Type) {
            case INPUT_TYPE_Keyboard:
                if (IsKeyDown(key.Key)) return true;
                break;
            case INPUT_TYPE_Mouse:
                if (IsMouseButtonDown(key.Key)) return true;
                break;
            default:
                if (IsGamepadButtonDown(key.Type - 2, key.Key)) return true;
                break;
        }
    }
    return false;
}

// === CAMERAS ===

typedef struct {
    Camera2D Cam;
    f32d     X, Y, Zoom;
    f32d     Radius, Angle;
} OrbitCamera2D;

OrbitCamera2D NewOrbitCamera2D() {
    return (OrbitCamera2D){.Cam    = {{}, {}, 0.0f, 1.0f},
                           .X      = f32dInit(0.0f),
                           .Y      = f32dInit(0.0f),
                           .Zoom   = f32dInit(1.0f),
                           .Radius = f32dInit(0.0f),
                           .Angle  = f32dInit(0.0f)};
}

void SimpleCameraControls(Camera2D* cam, WindowOptions window) {
    f32 delta = GetFrameTime();

    cam->target.x += (IsActionDown(ACTION_CAM_LEFT)    ? -500.0f
                      : IsActionDown(ACTION_CAM_RIGHT) ? 500.0f
                                                       : 0.0f) *
                     delta;
    cam->target.y += (IsActionDown(ACTION_CAM_UP)     ? -500.0f
                      : IsActionDown(ACTION_CAM_DOWN) ? 500.0f
                                                      : 0.0f) *
                     delta;

    if (IsActionDown(ACTION_CAM_ZOOM_RESET))
        cam->zoom = 1.0f;
    else
        cam->zoom *= IsActionPressed(ACTION_CAM_ZOOM_IN)    ? 2.0f
                     : IsActionPressed(ACTION_CAM_ZOOM_OUT) ? 0.5f
                                                            : 1.0f;

    cam->offset = (v2){GetScreenWidth() / 2, GetScreenHeight() / 2};
}

// === OS ===

void Execute(const char* cmd) {
    FILE* fp = popen(cmd, "r");
    char  path[4096 + 1];

    while (fgets(path, sizeof(path), fp) != NULL) {
        path[strcspn(path, "\n")] = '\0';
        printf("%s", path);
    }
}

void GameWindowControls(WindowOptions* window) {
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_K)) SetGameWindow(window);
    if (IsKeyPressed(KEY_F11) || (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)))
        ToggleFullscreen();
}
