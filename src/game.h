#pragma once

#include "raylib.h"
#include "raymath.h"

#ifdef _WIN32
#ifdef GAME_EXPORTS
#define GAME_API __declspec(dllexport)
#else
#define GAME_API
// __declspec(dllimport)
#endif
#else
#define GAME_API
#endif

#include "types.h"

// ===== SETUP =====

typedef struct {
    u32   screenWidth, screenHeight;
    u32   targetFPS;
    char* title;
    u32   ConfigFlags;
    u32   WindowFlags;
    char* iconPath;
} WindowOptions;

void InitGameWindow(WindowOptions* opts) {
    SetConfigFlags(opts->ConfigFlags);

    InitWindow(opts->screenWidth, opts->screenHeight, opts->title);
    SetWindowState(opts->WindowFlags);
    SetWindowIcon(LoadImage(opts->iconPath));
}

void SetGameWindow(WindowOptions* opts) {}

// ====== KEYBINDS =====

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

typedef struct {
    InputType Type;
    // TODO: Modifiers
    i32 Key;
} Keybind;

#define MAX_KEYBINDS 3

static Keybind Keys[ACTION_COUNT][MAX_KEYBINDS] = {{{0, KEY_A}, {0, KEY_LEFT}},
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
                                                   {{0, KEY_R}}};

GAME_API bool IsActionPressed(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++) {
        Keybind key = Keys[action][i];
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
    }
    return false;
}

GAME_API bool IsActionPressedRepeat(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++) {
        Keybind key = Keys[action][i];
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

GAME_API bool IsActionReleased(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++) {
        Keybind key = Keys[action][i];
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

GAME_API bool IsActionUp(Action action) {
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

GAME_API bool IsActionDown(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++) {
        Keybind key = Keys[action][i];
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

// ===== CAMERAS =====

typedef struct {
    Camera2D Cam;
    f32d     X, Y, Zoom;
    f32d     Radius, Angle;
} OrbitCamera2D;

GAME_API OrbitCamera2D NewOrbitCamera2D() {
    return (OrbitCamera2D){.Cam    = {{}, {}, 0.0f, 1.0f},
                           .X      = f32dInit(0.0f),
                           .Y      = f32dInit(0.0f),
                           .Zoom   = f32dInit(1.0f),
                           .Radius = f32dInit(0.0f),
                           .Angle  = f32dInit(0.0f)};
}

GAME_API void SimpleCameraControls(Camera2D* cam, WindowOptions window) {
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

    cam->offset = (v2){window.screenWidth / 2, window.screenHeight / 2};
}

// ===== OS =====

void Execute(const char* cmd) {
    FILE* fp = popen(cmd, "r");
    char  path[4096 + 1];

    while (fgets(path, sizeof(path), fp) != NULL) {
        path[strcspn(path, "\n")] = '\0';
        printf(path);
    }
}

void GameWindowControls(WindowOptions* window) {
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_R)) SetGameWindow(window);
    if (IsKeyPressed(KEY_F11) || (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)))
        ToggleFullscreen();
}