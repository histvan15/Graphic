#ifndef CIRCLE_H
#define CIRCLE_H
#include <SDL2/SDL.h>
#include <stdbool.h>

/**
 * Data of a circle object in Descartes coordinate system
 */
typedef struct Color {
    unsigned char r, g, b;
} Color;

typedef struct Circle
{
	double x;
	double y;
	double radius;
	Color color;
	bool is_hovered;
} Circle;

/**
 * Set the data of the circle
 */
void set_circle_data(Circle* circle, double x, double y, double radius, Color color);
void draw_circle(Circle* circle, void* renderer, int segments);

/**
 * Render the circle using SDL_Renderer
 */
void render_circle(SDL_Renderer* renderer, Circle* circle);

/**
 * Calculate the area of the circle.
 */
double calc_circle_area(const Circle* circle);

#endif // CIRCLE_H
