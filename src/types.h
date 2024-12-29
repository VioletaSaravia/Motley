#pragma once

#ifdef _WIN32
#ifdef TYPES_EXPORTS
#define TYPES_API __declspec(dllexport)
#else
#define TYPES_API
// __declspec(dllimport)
#endif
#else
#define TYPES_API
#endif

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

TYPES_API f32d f32dInit(f32 x0) {
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

TYPES_API void f32dSetParameters(f32d* this, f32 freq, f32 damp, f32 resp) {
    this->freq = freq;
    this->damp = damp;
    this->resp = resp;

    f32dCalculateK(this);
}

TYPES_API void f32dUpdate(f32d* this, f32 x) {
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

TYPES_API v2 PolarToCartesian(f32 radius, f32 angle) {
    return (v2){radius * cos(angle), radius * sin(angle)};
}

TYPES_API v2 CartesianToPolar(v2 coords) {
    return (v2){sqrtf(pow(coords.x, 2) + pow(coords.y, 2)), atanf(coords.y / coords.x)};
}

TYPES_API v3 CalculateOrbit3D(f32 radius, f32 yaw, f32 pitch) {
    v3 ray = {0, 0, 1};

    ray = Vector3RotateByQuaternion(ray, QuaternionFromAxisAngle((v3){0, 1, 0}, TAU - yaw));

    v3 pitchAxis = Vector3CrossProduct(ray, (v3){0, 1, 0});
    ray          = Vector3RotateByQuaternion(ray, QuaternionFromAxisAngle(pitchAxis, TAU - pitch));
    return Vector3Scale(ray, -radius);
}

// ===== STRINGS =====

TYPES_API bool EndsWith(const char* str, const char* suffix) {
    u64 len        = strlen(str);
    u64 suffix_len = strlen(suffix);

    if (len < suffix_len) {
        return false;
    }

    return strcmp(str + len - suffix_len, suffix) == 0;
}

TYPES_API i32 CountLines(const char* filename) {
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

// ===== COLLISION =====

TYPES_API Rectangle CheckOverlapRecs(Rectangle r1, Rectangle r2) {
    float x      = fmaxf(r1.x, r2.x);
    float y      = fmaxf(r1.y, r2.y);
    float width  = fminf(r1.x + r1.width, r2.x + r2.width);
    float height = fminf(r1.y + r1.height, r2.y + r2.height);

    return (x > width || y > height) ? (Rectangle){} : (Rectangle){x, y, width - x, height - y};
}

// ===== MEMORY =====

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
