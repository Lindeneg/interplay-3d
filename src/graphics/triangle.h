#ifndef INTERPLAY_GRAPHICS_TRIANGLE_H_
#define INTERPLAY_GRAPHICS_TRIANGLE_H_

#include "../core/texture.h"
#include "../globals.h"
#include "../math/vec.h"

typedef struct triangle_face {
    int a, b, c;
    texture_uv_t a_uv, b_uv, c_uv;
    color_t color;
} triangle_face_t;

typedef struct triangle {
    vec4f_t points[3];
    texture_uv_t tex_coords[3];
    const texture_t *texture;
    color_t color;
} triangle_t;

typedef struct filled_triangle_point {
    int x, y;
    float z, w;
} filled_triangle_point_t;

typedef struct textured_triangle_point {
    int x, y;
    float z, w, u, v;
} textured_triangle_point_t;

// get triangle normal vector
vec3f_t get_triangle_normal(vec4f_t vertices[3]);

// draw unfilled/outlined triangle
void draw_triangle(vec2i_t p0, vec2i_t p1, vec2i_t p2, color_t color);

// draw triangle filled with solid color
void draw_filled_triangle(filled_triangle_point_t *p0,
                          filled_triangle_point_t *p1,
                          filled_triangle_point_t *p2, color_t color);

// draw triangle filled with texture
void draw_textured_triangle(textured_triangle_point_t *p0,
                            textured_triangle_point_t *p1,
                            textured_triangle_point_t *p2,
                            const texture_t *texture);

#endif  // INTERPLAY_GRAPHICS_TRIANGLE_H_
