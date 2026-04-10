#include "pong.h"
#include <stdio.h>
#include <GL/gl.h>

void init_pong(Pong* pong, int width, int height)
{
    pong->width = width;
    pong->height = height;
    init_pad(&(pong->left_pad), 0, height, RED_THEME);
    init_pad(&(pong->right_pad), width - 50, height, GREEN_THEME);
    init_ball(&(pong->ball), width / 2, height / 2);
}

void update_pong(Pong* pong, double time)
{
    update_pad(&(pong->left_pad), time);
    update_pad(&(pong->right_pad), time);
    update_ball(&(pong->ball), time);
    bounce_ball(pong);
}

void render_pong(Pong* pong)
{
    render_pad(&(pong->left_pad));
    render_pad(&(pong->right_pad));
    render_ball(&(pong->ball));

    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    for (int i = 0; i < pong->score_left; i++) {
        glVertex2f(20 + i * 25, 10);
        glVertex2f(40 + i * 25, 10);
        glVertex2f(40 + i * 25, 30);
        glVertex2f(20 + i * 25, 30);
    }
    glColor3f(0.0, 1.0, 0.0);
    for (int i = 0; i < pong->score_right; i++) {
        glVertex2f(pong->width - 40 - i * 25, 10);
        glVertex2f(pong->width - 20 - i * 25, 10);
        glVertex2f(pong->width - 20 - i * 25, 30);
        glVertex2f(pong->width - 40 - i * 25, 30);
    }
    glEnd();
}

void set_left_pad_position(Pong* pong, float position)
{
    set_pad_position(&(pong->left_pad), position);
}

void set_left_pad_speed(Pong* pong, float speed)
{
    set_pad_speed(&(pong->left_pad), speed);
}

void set_right_pad_position(Pong* pong, float position)
{
    set_pad_position(&(pong->right_pad), position);
}

void set_right_pad_speed(Pong* pong, float speed)
{
    set_pad_speed(&(pong->right_pad), speed);
}

void bounce_ball(Pong* pong)
{
    if (pong->ball.x - pong->ball.radius < 50) {
        pong->ball.x = pong->ball.radius + 50;
        pong->ball.speed_x *= -1;
    }
    if (pong->ball.x + pong->ball.radius > pong->width - 50) {
        pong->ball.x = pong->width - pong->ball.radius - 50;
        pong->ball.speed_x *= -1;
    }
    if (pong->ball.y - pong->ball.radius < 0) {
        pong->ball.y = pong->ball.radius;
        pong->ball.speed_y *= -1;
    }
    if (pong->ball.y + pong->ball.radius > pong->height) {
        pong->ball.y = pong->height - pong->ball.radius;
        pong->ball.speed_y *= -1;
    }

    if (pong->ball.x - pong->ball.radius < pong->left_pad.x + pong->left_pad.width) {
        if (pong->ball.y + pong->ball.radius > pong->left_pad.y && 
            pong->ball.y - pong->ball.radius < pong->left_pad.y + pong->left_pad.height) {
            pong->ball.x = pong->left_pad.x + pong->left_pad.width + pong->ball.radius;
            pong->ball.speed_x *= -1;
            pong->ball.rotation_speed += 50;
        } else if (pong->ball.x + pong->ball.radius < 0) {
            pong->score_right++;
            printf("Pont! Allas: %d - %d\n", pong->score_left, pong->score_right);
            init_ball(&(pong->ball), pong->width / 2, pong->height / 2);
            pong->ball.speed_x = 300;
        }
    }

    if (pong->ball.x + pong->ball.radius > pong->right_pad.x) {
        if (pong->ball.y + pong->ball.radius > pong->right_pad.y && 
            pong->ball.y - pong->ball.radius < pong->right_pad.y + pong->right_pad.height) {
            pong->ball.x = pong->right_pad.x - pong->ball.radius;
            pong->ball.speed_x *= -1;
            pong->ball.rotation_speed -= 50;
        } else if (pong->ball.x - pong->ball.radius > pong->width) {
            pong->score_left++;
            printf("Pont! Allas: %d - %d\n", pong->score_left, pong->score_right);
            init_ball(&(pong->ball), pong->width / 2, pong->height / 2);
            pong->ball.speed_x = -300;
        }
    }

    if (pong->ball.y - pong->ball.radius < 0) {
        pong->ball.y = pong->ball.radius;
        pong->ball.speed_y *= -1;
        pong->ball.rotation_speed *= -1;
    }
    if (pong->ball.y + pong->ball.radius > pong->height) {
        pong->ball.y = pong->height - pong->ball.radius;
        pong->ball.speed_y *= -1;
        pong->ball.rotation_speed *= -1;
    }
}
