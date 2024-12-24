#pragma once

#include "types.h"

void DrawCubeInGrid(v3i coords, Color color){
    DrawCubeV(
        (v3){
            coords.x * UNITSIZE,
            coords.y * UNITHEIGHT,
            coords.z * UNITSIZE
        },
        (v3){UNITSIZE, UNITSIZE, UNITSIZE},
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