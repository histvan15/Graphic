#include "circle.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <SDL2/SDL.h>

void set_circle_data(Circle* circle, double x, double y, double radius, Color color)
{
	circle->x = x;
	circle->y = y;
	if (radius > 0.0) {
		circle->radius = radius;
	} else {
		circle->radius = NAN;
	}
	circle->color = color;
	circle->is_hovered = false;
}
void render_circle(SDL_Renderer* renderer, Circle* circle) {
    int segments = 60; 
    double step = 2.0 * M_PI / segments;

    SDL_SetRenderDrawColor(renderer, circle->color.r, circle->color.g, circle->color.b, 255);

    for (int i = 0; i < segments; i++) {
        double angle1 = i * step;
        double angle2 = (i + 1) * step;

        SDL_RenderDrawLine(renderer,
            circle->x + cos(angle1) * circle->radius,
            circle->y + sin(angle1) * circle->radius,
            circle->x + cos(angle2) * circle->radius,
            circle->y + sin(angle2) * circle->radius);
    }

    if (circle->is_hovered) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, circle->x - 5, circle->y, circle->x + 5, circle->y);
        SDL_RenderDrawLine(renderer, circle->x, circle->y - 5, circle->x, circle->y + 5);
    }
}
double calc_circle_area(const Circle* circle)
{
	double area = circle->radius * circle->radius * M_PI;
	return area;
}
