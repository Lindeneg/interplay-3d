#include "renderer.h"

#include <stdlib.h>

#include "./screen.h"

void draw_vec2i(vec2i_t vec, color_t color) {
    screen_put_back_buffer(vec.x, vec.y, color);
}

void draw_vec2f(const vec2f_t *vec, color_t color) {
    screen_put_back_buffer((int)vec->x, (int)vec->y, color);
}

void draw_line(vec2i_t p0, vec2i_t p1, uint32_t color) {
    // delta x
    int dx = abs(p1.x - p0.x);
    // right or left?
    int sx = p0.x < p1.x ? 1 : -1;
    int dy = abs(p1.y - p0.y) * -1;
    // up or down?
    int sy = p0.y < p1.y ? 1 : -1;
    int error = dx + dy;
    while (true) {
        screen_put_back_buffer(p0.x, p0.y, color);
        if (p0.x == p1.x && p0.y == p1.y) break;
        int double_err = error * 2;
        if (double_err >= dy) {
            if (p0.x == p1.x) break;
            error = error + dy;
            p0.x += sx;
        }
        if (double_err <= dx) {
            if (p0.y == p1.y) break;
            error = error + dx;
            p0.y += sy;
        }
    }
}

void draw_outlined_rect(vec2i_t center, vec2i_t size, color_t color) {
    int half_width = size.x / 2;
    int half_height = size.y / 2;
    vec2i_t top_left = {center.x + half_width, center.y - half_height};
    vec2i_t top_right = {center.x - half_width, center.y - half_height};
    vec2i_t bottom_right = {center.x - half_width, center.y + half_height};
    vec2i_t bottom_left = {center.x + half_width, center.y + half_height};
    draw_line(top_left, top_right, color);
    draw_line(top_right, bottom_right, color);
    draw_line(bottom_right, bottom_left, color);
    draw_line(bottom_left, top_left, color);
}

void draw_filled_rect(vec2i_t center, vec2i_t size, color_t color) {
    for (int y = 0; y < size.x; y++) {
        for (int x = 0; x < size.y; x++) {
            int x1 = center.x + x;
            int y1 = center.y + y;
            screen_put_back_buffer(x1, y1, color);
        }
    }
}

