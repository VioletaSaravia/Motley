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

// ====== KEYBINDS =====

typedef enum {
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_ACCEPT,
    ACTION_CANCEL,
    ACTION_MENU,
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
    KeyTypeKeyboard,
    KeyTypeMouse,
    KeyTypeGamepad1,
    KeyTypeGamepad2,
    KeyTypeGamepad3,
    KeyTypeGamepad4  // etc.
} KeyType;

typedef struct {
    KeyType Type;
    i32     Key;
} Keybind;

#define MAX_KEYBINDS 3

static Keybind Keys[ACTION_COUNT][MAX_KEYBINDS] = {
    {{0, KEY_A}, {0, KEY_LEFT}, {2, GAMEPAD_BUTTON_LEFT_FACE_LEFT}},
    {{0, KEY_D}, {0, KEY_RIGHT}, {2, GAMEPAD_BUTTON_LEFT_FACE_RIGHT}},
    {{0, KEY_W}, {0, KEY_UP}, {2, GAMEPAD_BUTTON_LEFT_FACE_UP}},
    {{0, KEY_S}, {0, KEY_DOWN}, {2, GAMEPAD_BUTTON_LEFT_FACE_DOWN}},
    {{0, KEY_C}, {1, MOUSE_BUTTON_LEFT}, {2, GAMEPAD_BUTTON_RIGHT_FACE_DOWN}},
    {{0, KEY_V}, {1, MOUSE_BUTTON_RIGHT}, {2, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT}},
    {{0, KEY_ENTER}, {0, KEY_MENU}, {2, GAMEPAD_BUTTON_MIDDLE_RIGHT}},
    {{0, KEY_J}, {2, GAMEPAD_BUTTON_UNKNOWN}},
    {{0, KEY_L}, {2, GAMEPAD_BUTTON_UNKNOWN}},
    {{0, KEY_I}, {2, GAMEPAD_BUTTON_UNKNOWN}},
    {{0, KEY_K}, {2, GAMEPAD_BUTTON_UNKNOWN}},
    {{0, KEY_M}, {2, GAMEPAD_BUTTON_RIGHT_TRIGGER_1}},
    {{0, KEY_N}, {2, GAMEPAD_BUTTON_LEFT_TRIGGER_1}},
    {{0, KEY_B}, {2, GAMEPAD_BUTTON_RIGHT_THUMB}}};

GAME_API bool IsActionPressed(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++) {
        Keybind key = Keys[action][i];
        switch (key.Type) {
            case KeyTypeKeyboard:
                if (IsKeyPressed(key.Key)) return true;
                break;
            case KeyTypeMouse:
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
            case KeyTypeKeyboard:
                if (IsKeyPressedRepeat(key.Key)) return true;
                break;
            case KeyTypeMouse:
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
            case KeyTypeKeyboard:
                if (IsKeyReleased(key.Key)) return true;
                break;
            case KeyTypeMouse:
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
            case KeyTypeKeyboard:
                if (IsKeyUp(key.Key)) return true;
                break;
            case KeyTypeMouse:
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
            case KeyTypeKeyboard:
                if (IsKeyDown(key.Key)) return true;
                break;
            case KeyTypeMouse:
                if (IsMouseButtonDown(key.Key)) return true;
                break;
            default:
                if (IsGamepadButtonDown(key.Type - 2, key.Key)) return true;
                break;
        }
    }
    return false;
}

// ===== SETUP =====

typedef struct {
    u32   screenWidth, screenHeight;
    u32   targetFPS;
    char* title;
    i32   ConfigFlags;
    Image Icon;
} WindowOptions;

static WindowOptions window = {800, 450, 0, "MAICENA", FLAG_MSAA_4X_HINT};

void InitGameWindow(WindowOptions* opts) {
    SetConfigFlags(opts->ConfigFlags);

    InitWindow(opts->screenWidth, opts->screenHeight, opts->title);

    opts->Icon = LoadImage("");
    SetWindowIcon(opts->Icon);
}

void SetGameWindow(WindowOptions* opts) { SetTargetFPS(opts->targetFPS); }

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

// ===== HELPERS =====

void Execute(const char* cmd) {
    FILE* fp = popen(cmd, "r");
    char  path[4096 + 1];

    while (fgets(path, sizeof(path), fp) != NULL) {
        path[strcspn(path, "\n")] = '\0';
        printf(path);
    }
}