#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "circle.h"

#define MAX_CIRCLE_COUNT 100

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Circle Interactor", 100, 100, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Circle circles[MAX_CIRCLE_COUNT];
    int circle_count = 0;
    Circle* dragged_circle = NULL;

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (circle_count < MAX_CIRCLE_COUNT) {
                    Color random_color = {rand() % 256, rand() % 256, rand() % 256};
                    set_circle_data(&circles[circle_count], event.button.x, event.button.y, 30 + rand() % 50, random_color);
                    circle_count++;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
                for (int i = 0; i < circle_count; i++) {
                    double dx = event.button.x - circles[i].x;
                    double dy = event.button.y - circles[i].y;
                    if (sqrt(dx*dx + dy*dy) <= circles[i].radius) {
                        dragged_circle = &circles[i];
                        break;
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) dragged_circle = NULL;

            // Mozgatás és Hover (kijelölés) kezelése
            if (event.type == SDL_MOUSEMOTION) {
                for (int i = 0; i < circle_count; i++) {
                    double dx = event.motion.x - circles[i].x;
                    double dy = event.motion.y - circles[i].y;
                    circles[i].is_hovered = (sqrt(dx*dx + dy*dy) <= circles[i].radius);
                }
                if (dragged_circle) {
                    dragged_circle->x = event.motion.x;
                    dragged_circle->y = event.motion.y;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        for (int i = 0; i < circle_count; i++) {
            render_circle(renderer, &circles[i]);
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}