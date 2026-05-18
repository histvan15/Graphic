#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "engine.h"
#include "camera.h"
#include "window.h"
#include "scene.h"

int main(int argc, char* argv[]) {
    AppWindow appWin;
    if (!initWindow(&appWin, "Sziget Projekt", 800, 600)) {
        return -1;
    }

    Camera camera = initCamera((vec3){0.0f, 1.0f, 4.0f});
    Scene scene = initScene();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float lightIntensity = 1.0f;
    int running = 1;
    bool showSugo = false;
    SDL_Event event;

    while (running) {
        float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;

            // sugo toggle F1
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_F1) {
                    showSugo = !showSugo;
                }
            }

            if (event.type == SDL_MOUSEMOTION) {
                float xoffset = event.motion.xrel * 0.1f;
                float yoffset = -event.motion.yrel * 0.1f;
                processMouseInput(&camera, xoffset, yoffset);
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);

        // movement
        processKeyboardInput(&camera, state, deltaTime);
        if(state[SDL_SCANCODE_ESCAPE]) running = 0;
        
        // lights
        if (state[SDL_SCANCODE_KP_PLUS]  || state[SDL_SCANCODE_EQUALS]) lightIntensity += 1.0f * deltaTime;
        if (state[SDL_SCANCODE_KP_MINUS] || state[SDL_SCANCODE_MINUS])  lightIntensity -= 1.0f * deltaTime;
        if (lightIntensity > 1.5f) lightIntensity = 1.5f;
        if (lightIntensity < 0.0f) lightIntensity = 0.0f;

        renderScene(&scene, &camera, lightIntensity, currentFrame, appWin.width, appWin.height, showSugo);

        SDL_GL_SwapWindow(appWin.handle);
    }

    cleanupWindow(&appWin);
    
    return 0;
}