#ifndef CAMERA_H
#define CAMERA_H

#include "engine.h"
#include <SDL2/SDL.h>

typedef struct {
    vec3 position;
    vec3 front;
    vec3 up;
    float yaw;
    float pitch;
} Camera;

Camera initCamera(vec3 startPosition);

void processKeyboardInput(Camera* cam, const Uint8* state, float deltaTime);

void processMouseInput(Camera* cam, float xoffset, float yoffset);

#endif