#include "camera.h"
#include <math.h>

Camera initCamera(vec3 startPosition) {
    Camera cam;
    cam.position = startPosition;
    cam.front = (vec3){0.0f, 0.0f, -1.0f};
    cam.up = (vec3){0.0f, 1.0f,  0.0f};
    cam.yaw = -90.0f;
    cam.pitch = -10.0f;
    return cam;
}

void processKeyboardInput(Camera* cam, const Uint8* state, float deltaTime) {
    float cameraSpeed = 2.5f * deltaTime;
    vec3 nextPos = cam->position;
    
    if (state[SDL_SCANCODE_W]) nextPos = vec3_add(nextPos, vec3_scale(cam->front, cameraSpeed));
    if (state[SDL_SCANCODE_S]) nextPos = vec3_sub(nextPos, vec3_scale(cam->front, cameraSpeed));
    if (state[SDL_SCANCODE_A]) nextPos = vec3_sub(nextPos, vec3_scale(vec3_normalize(vec3_cross(cam->front, cam->up)), cameraSpeed));
    if (state[SDL_SCANCODE_D]) nextPos = vec3_add(nextPos, vec3_scale(vec3_normalize(vec3_cross(cam->front, cam->up)), cameraSpeed));

    float minX = -0.6f, maxX = 0.6f;
    float minZ = -0.6f, maxZ = 0.6f;

    bool collisionX = (nextPos.x > minX && nextPos.x < maxX && cam->position.z > minZ && cam->position.z < maxZ);
    bool collisionZ = (cam->position.x > minX && cam->position.x < maxX && nextPos.z > minZ && nextPos.z < maxZ);

    if (!collisionX) cam->position.x = nextPos.x;
    if (!collisionZ) cam->position.z = nextPos.z;
    cam->position.y = nextPos.y;
}

void processMouseInput(Camera* cam, float xoffset, float yoffset) {
    cam->yaw += xoffset;
    cam->pitch += yoffset;
    if (cam->pitch > 89.0f)  cam->pitch = 89.0f;
    if (cam->pitch < -89.0f) cam->pitch = -89.0f;
    vec3 front;
    front.x = cos(radians(cam->yaw)) * cos(radians(cam->pitch));
    front.y = sin(radians(cam->pitch));
    front.z = sin(radians(cam->yaw)) * cos(radians(cam->pitch));
    cam->front = vec3_normalize(front);
}