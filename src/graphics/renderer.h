#ifndef INTERPLAY_GRAPHICS_RENDERER_H_
#define INTERPLAY_GRAPHICS_RENDERER_H_

#include "../globals.h"
#include "../math/vec.h"

void draw_vec2i(vec2i_t vec, color_t color);

void draw_vec2f(const vec2f_t *vec, color_t color);

void draw_line(vec2i_t p0, vec2i_t p1, uint32_t color);

void draw_outlined_rect(vec2i_t center, vec2i_t size, color_t color);

void draw_filled_rect(vec2i_t center, vec2i_t size, color_t color);

#endif  // INTERPLAY_GRAPHICS_RENDERER_H_
