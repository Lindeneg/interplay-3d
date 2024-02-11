#ifndef INTERPLAY_GRAPHICS_CLIPPING_H_
#define INTERPLAY_GRAPHICS_CLIPPING_H_

#include "../core/texture.h"
#include "../math/vec.h"
#include "./triangle.h"

#define MAX_NUM_POLY_VERTICES 10
#define MAX_NUM_POLY_TRIANGLES 10

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct plane {
    vec3f_t point, normal;
} plane_t;

typedef struct polygon {
    int num_vertices;
    vec3f_t vertices[MAX_NUM_POLY_VERTICES];
    texture_uv_t tex_coords[MAX_NUM_POLY_VERTICES];
} polygon_t;

void init_frustum_planes(float fov_x, float fov_y, float z_near, float z_far);

polygon_t polygon_from_triangle(vec3f_t p0, vec3f_t p1, vec3f_t p2,
                                texture_uv_t t0, texture_uv_t t1,
                                texture_uv_t t2);

void clip_polygon(polygon_t *poly);

void triangles_from_polygon(polygon_t *polygon, triangle_t *triangles,
                            int *num_of_triangles);

#endif  // INTERPLAY_GRAPHICS_CLIPPING_H_
