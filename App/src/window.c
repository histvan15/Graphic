#include "window.h"
#include <stdio.h>

bool initWindow(AppWindow* appWin, const char* title, int width, int height) {
    appWin->width = width;
    appWin->height = height;

    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL hiba: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    appWin->handle = SDL_CreateWindow(title, 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (!appWin->handle) {
        printf("Ablak letrehozasi hiba: %s\n", SDL_GetError());
        return false;
    }

    appWin->glContext = SDL_GL_CreateContext(appWin->handle);
    if (!appWin->glContext) {
        printf("OpenGL kontextus hiba: %s\n", SDL_GetError());
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("GLAD betoltesi hiba!\n");
        return false;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE); 
    glEnable(GL_DEPTH_TEST);
    
    // blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void cleanupWindow(AppWindow* appWin) {
    if (appWin->glContext) SDL_GL_DeleteContext(appWin->glContext);
    if (appWin->handle) SDL_DestroyWindow(appWin->handle);
    SDL_Quit();
}