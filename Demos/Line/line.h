#ifndef LINE_H
#define LINE_H

#include <SDL2/SDL.h>

typedef struct Color {
    unsigned char r, g, b;
} Color;

typedef struct Line {
    int x1, y1, x2, y2;
    Color color;
} Line;

void draw_lines(SDL_Renderer* renderer, Line lines[], int count);

#endif