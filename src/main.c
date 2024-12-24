#include "types.h"
#include "input.h"

// GAME SETUP
const struct {
    u32 screenWidth, screenHeight;
    u32 targetFPS;
    char* title;
    i32 ConfigFlags;
    bool DELETEME;
} game = {
    800, 450,
    60,
    "MAICENA",
    FLAG_MSAA_4X_HINT
};

// PLAYER DATA
struct {
    v3u Pos;
} player;

Action REL_LEFT = ACTION_LEFT;
Action REL_RIGHT = ACTION_RIGHT;
Action REL_DOWN = ACTION_DOWN;
Action REL_UP = ACTION_UP;

// MAP DATA
#define GRIDSIZE 14
#define UNITSIZE 1.212f
#define UNITHEIGHT (0.2394f * UNITSIZE)
#define UNITCOUNT 1

struct {
    u32 Objs[GRIDSIZE][GRIDSIZE];
    u32 Height[GRIDSIZE][GRIDSIZE];
} grid;

struct {
    Model *Models[UNITCOUNT];
} data;

void DrawCubeInGrid(v3i coords, v3 size, Color color){
    DrawCubeV(
        (v3){
            coords.x * UNITSIZE,
            coords.y * UNITHEIGHT,
            coords.z * UNITSIZE
        },
        size,
        color
    );
}

void DrawModelInGrid(v3i coords, Model model) {
    DrawModel(model, (v3){
            coords.x * UNITSIZE,
            coords.y * UNITHEIGHT,
            coords.z * UNITSIZE
        }, 1.0f, WHITE);
}

i32 main() {
    // RAYLIB INIT
    InitWindow(game.screenWidth, game.screenHeight, game.title);
    SetConfigFlags(game.ConfigFlags);
    SetTargetFPS(game.targetFPS);
    
    // TERRAIN INIT
    Image perlin = GenImagePerlinNoise(GRIDSIZE, GRIDSIZE, 0, 0, 1.0f);
    Color* perlinPixels = LoadImageColors(perlin);
    
    for (int x = 0; x < GRIDSIZE; x++) {
        for (int y = 0; y < GRIDSIZE; y++) {
            grid.Objs[x][y] = GetRandomValue(0, 100) < 98 ? 0 : 1;
            grid.Height[x][y] = perlinPixels[y * GRIDSIZE + x].r % 5;
        }
    }

    // MODELS INIT
    Model tree = LoadModel("assets/tree01.glb");
    data.Models[0] = &tree;

    // CAMERA INIT
    v3 cameraOrbit = (v3){GRIDSIZE * 0.5f * UNITSIZE, 0, GRIDSIZE * 0.5f * UNITSIZE};
    Camera camera = {
        {0}, 
        cameraOrbit, 
        { 0.0f, 1.0f, 0.0f }, 
        45.0f,
        CAMERA_PERSPECTIVE
    };
    
    f32 radius = 1.5f * GRIDSIZE;
    DampedF32 yaw = DampedF32Init();
    DampedF32Start(&yaw, -0.25f * PI);
    DampedF32 pitch = DampedF32Init();
    DampedF32Start(&pitch, -10.0f);
    
    while (!WindowShouldClose()) {
        // CAMERA UPDATE
        DampedF32Update(&yaw,
            IsKeyDown(KEY_Q) ? yaw._y - 2.0f : 
            IsKeyDown(KEY_E) ? yaw._y + 2.0f : 
            yaw._y
        );
        DampedF32Update(&pitch, 
            IsKeyDown(KEY_F) ? pitch._y - 1.0f : 
            IsKeyDown(KEY_R) ? pitch._y + 1.0f : 
            pitch._y
        );
        
        camera.position = Vector3Add(CalculateOrbit(radius, yaw._y, pitch._y), cameraOrbit);
        
        // CONTROLS UPDATE
        f32 angle = (camera.position.x > 0.0f ? 1.0f : -1.0f) * 
                    Vector3Angle(
                        (v3){camera.position.x, 0, camera.position.z}, 
                        (v3){0, 0, 1}
                    );
                    
        if (angle > -0.25f * PI && angle <= 0.25f * PI){
            REL_LEFT = ACTION_LEFT;
            REL_RIGHT = ACTION_RIGHT;
            REL_DOWN = ACTION_DOWN;
            REL_UP = ACTION_UP;
        }
        if (angle > 0.25f * PI && angle <= 0.75f * PI){
            REL_LEFT = ACTION_UP;
            REL_RIGHT = ACTION_DOWN;
            REL_DOWN = ACTION_LEFT;
            REL_UP = ACTION_RIGHT;
        }
        if (angle > 0.75f * PI || angle <= -0.75f * PI){
            REL_LEFT = ACTION_RIGHT;
            REL_RIGHT = ACTION_LEFT;
            REL_DOWN = ACTION_UP;
            REL_UP = ACTION_DOWN;
        }
        if (angle > -0.75f * PI && angle < -0.25f * PI){
            REL_LEFT = ACTION_DOWN;
            REL_RIGHT = ACTION_UP;
            REL_DOWN = ACTION_RIGHT;
            REL_UP = ACTION_LEFT;
        }
        
        if (IsActionPressed(REL_LEFT) && !grid.Objs[MIN(player.Pos.x - 1, GRIDSIZE - 1)][player.Pos.z])
            player.Pos.x = MIN(player.Pos.x - 1, GRIDSIZE - 1);
        if (IsActionPressed(REL_RIGHT) && !grid.Objs[(player.Pos.x + 1) % GRIDSIZE][player.Pos.z])
            player.Pos.x = (player.Pos.x + 1) % GRIDSIZE;
        if (IsActionPressed(REL_UP) && !grid.Objs[player.Pos.x][MIN(player.Pos.z - 1, GRIDSIZE - 1)]) 
            player.Pos.z = MIN(player.Pos.z - 1, GRIDSIZE - 1);
        if (IsActionPressed(REL_DOWN) && !grid.Objs[player.Pos.x][(player.Pos.z + 1) % GRIDSIZE]) 
            player.Pos.z = (player.Pos.z + 1) % GRIDSIZE;
        
        // DRAWING
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            
                // Draw player
                DrawCubeInGrid(
                    (v3i){
                        player.Pos.x, 
                        grid.Height[player.Pos.x][player.Pos.z], 
                        player.Pos.z
                    }, 
                    (v3){UNITSIZE, UNITSIZE, UNITSIZE},
                    GREEN
                );
                
                // Draw terrain
                for (i32 i = 0; i < GRIDSIZE; i++){
                    for (i32 j = 0; j < GRIDSIZE; j++) {
                        DrawCubeInGrid(
                            (v3i){i, 0.0f, j},
                            (v3){UNITSIZE, (grid.Height[i][j] + 1) * UNITHEIGHT, UNITSIZE},
                            BLACK
                        );
                    }
                }
                
                
                // Draw objects
                for (i32 i = 0; i < GRIDSIZE; i++){
                    for (i32 j = 0; j < GRIDSIZE; j++) {
                        if (grid.Objs[i][j] != 0) {
                            DrawCubeInGrid(
                                (v3i){i, grid.Height[i][j], j}, 
                                (v3){UNITSIZE, UNITSIZE, UNITSIZE}, 
                                RED);//, *(data.Models[0]));
                        }
                    }
                }

            EndMode3D();

        // GUI
        DrawFPS(10, 10);
        DrawText(TextFormat("Yaw: %.2f", yaw._y), 10, 35, 20, BLACK);
        DrawText(TextFormat("Pitch: %.2f", pitch._y), 10, 60, 20, BLACK);
        DrawText(TextFormat("Player: %d, %d", player.Pos.x, player.Pos.z), 10, 85, 20, BLACK);
        
        EndDrawing();
    }

    CloseWindow();
}