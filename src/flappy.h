#pragma once
#include <time.h>

#include "shared.h"

#define MAX_PIPES 5

/*
TODO:
- [] Start Over
- [] Graphics
- [] Sound
- [] Difficulties

FIXME:
- [] Rotating animation
*/

struct Flappy {
    Rectangle TopPipes[MAX_PIPES];
    Rectangle BotPipes[MAX_PIPES];
    f32       PrevPipePos[MAX_PIPES];
    u8        NextPipe;
    f32       PipeTimer;

    f32  PlayerPos;
    f32  PlayerSpeed;
    f32d PlayerRotation;
    bool PlayerJumpDisabled;
    f32  JumpTimer;

    bool Lost;
    u32  Points;
} F = {};

i32 GAP_HEIGHT    = 200;
i32 PIPE_WIDTH    = 110;
f32 PIPE_SPEED    = 125;
f32 PIPE_TIMER    = 3.25f;
f32 GRAVITY       = 400;
f32 JUMP_SPEED    = 200;
f32 PLAYER_X      = 0;
i32 PLAYER_RADIUS = 30;
f32 JUMP_COOLDOWN = 0.1;

void InitFlappy() {
    srand(time(NULL));

    PLAYER_X = GetScreenWidth() * 0.2f;

    for (u32 i = 0; i < MAX_PIPES; i++) {
        F.TopPipes[i].x = 99999;
        F.BotPipes[i].x = 99999;
    }
    F.PipeTimer = PIPE_TIMER;
    F.PlayerPos = GetScreenHeight() / 2;

    F.PlayerRotation = f32dInit(0);
}

void UpdateFlappy() {
    f32 delta = GetFrameTime();

    if (F.Lost) {
        if (IsKeyPressed(KEY_SPACE))
            ;

        BeginDrawing();
        ClearBackground(BLUE);
        DrawText("YOU LOST!", GetScreenWidth() / 2, GetScreenHeight() / 2, 50, WHITE);
        EndDrawing();
        return;
    }
    F.PipeTimer += delta;

    for (u32 i = 0; i < MAX_PIPES; i++) {
        if (CheckCollisionCircleRec((v2){PLAYER_X, F.PlayerPos}, PLAYER_RADIUS, F.TopPipes[i]) ||
            CheckCollisionCircleRec((v2){PLAYER_X, F.PlayerPos}, PLAYER_RADIUS, F.BotPipes[i]) ||
            F.PlayerPos > GetScreenHeight()) {
            F.Lost = true;
        }

        F.PrevPipePos[i] = F.TopPipes[i].x;
        F.TopPipes[i].x -= PIPE_SPEED * delta;
        F.BotPipes[i].x -= PIPE_SPEED * delta;
    }

    for (u32 i = 0; i < MAX_PIPES; i++) {
        if (F.TopPipes[i].x <= PLAYER_X && F.PrevPipePos[i] > PLAYER_X) F.Points++;
    }

    if (F.PipeTimer >= PIPE_TIMER) {
        i32 height             = rand() % (GetScreenHeight() - GAP_HEIGHT * 2) + GAP_HEIGHT;
        F.TopPipes[F.NextPipe] = (Rectangle){GetScreenWidth(), 0, PIPE_WIDTH, height};
        F.BotPipes[F.NextPipe] = (Rectangle){GetScreenWidth(), height + GAP_HEIGHT, PIPE_WIDTH,
                                             GetScreenHeight() - height};

        F.NextPipe  = (F.NextPipe + 1) % MAX_PIPES;
        F.PipeTimer = (rand() % 75) / 75.0f;
    }

    if (IsKeyPressed(KEY_SPACE) && !F.PlayerJumpDisabled) {
        F.PlayerSpeed        = -JUMP_SPEED;
        F.PlayerJumpDisabled = true;
    }

    f32dUpdate(&F.PlayerRotation, F.PlayerJumpDisabled ? -45.0f * 10 : 70.0f);

    if (F.PlayerJumpDisabled) F.JumpTimer += delta;
    if (F.JumpTimer >= JUMP_COOLDOWN) {
        F.JumpTimer          = 0.0f;
        F.PlayerJumpDisabled = false;
    }

    F.PlayerPos += F.PlayerSpeed * delta;
    F.PlayerSpeed += GRAVITY * delta;

    BeginDrawing();
    ClearBackground(BLUE);

    for (u32 i = 0; i < MAX_PIPES; i++) {
        DrawRectangleRec(F.TopPipes[i], GREEN);
        DrawRectangleRec(F.BotPipes[i], GREEN);
    }

    DrawRectanglePro((Rectangle){PLAYER_X, F.PlayerPos, PLAYER_RADIUS * 2, PLAYER_RADIUS * 2},
                     (v2){PLAYER_RADIUS, PLAYER_RADIUS}, F.PlayerRotation._y, YELLOW);

    DrawText(TextFormat("Points: %u", F.Points), 10, 10, 30, WHITE);

    DrawFPS(10, 50);
    EndDrawing();
}

void CloseFlappy() {}