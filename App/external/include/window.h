#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Window* handle;
    SDL_GLContext glContext;
    int width;
    int height;
} AppWindow;

bool initWindow(AppWindow* appWin, const char* title, int width, int height);

void cleanupWindow(AppWindow* appWin);

#endif