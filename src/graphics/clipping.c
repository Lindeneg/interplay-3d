#include "clipping.h"

#include <math.h>

#include "../utils.h"

#define NUM_FRUSTUM_PLANES 6
plane_t frustum_planes[NUM_FRUSTUM_PLANES];

global_internal void clip_polygon_against_plane(polygon_t *polygon, int plane) {
    vec3f_t plane_point = frustum_planes[plane].point;
    vec3f_t plane_normal = frustum_planes[plane].normal;
    vec3f_t inside_vertices[MAX_NUM_POLY_VERTICES];
    texture_uv_t inside_tex_coords[MAX_NUM_POLY_VERTICES];
    int num_inside_vertices = 0;
    // start current vertex with first polygon
    vec3f_t *current_vertex = &polygon->vertices[0];
    texture_uv_t *current_texcoord = &polygon->tex_coords[0];
    // start previous vertex with last polygon
    vec3f_t *previous_vertex = &polygon->vertices[polygon->num_vertices - 1];
    texture_uv_t *previous_texcoord =
        &polygon->tex_coords[polygon->num_vertices - 1];
    float current_dot;
    // calculate dot product of current and previous
    float previous_dot = vec3f_dot(
        vec3f_subtract_new(*previous_vertex, plane_point), plane_normal);
    // loop all polygons while current is different from last
    while (current_vertex != &polygon->vertices[polygon->num_vertices]) {
        current_dot = vec3f_dot(
            vec3f_subtract_new(*current_vertex, plane_point), plane_normal);
        // if we changed from inside to outside or from outside to inside
        if (is_lesserf(current_dot * previous_dot, 0)) {
            // find interpolation factor t
            float t = previous_dot / (previous_dot - current_dot);
            // calculate intersection point I = Q1 + t(Q2-Q1)
            vec3f_t intersection_point = {
                .x = flerp(previous_vertex->x, current_vertex->x, t),
                .y = flerp(previous_vertex->y, current_vertex->y, t),
                .z = flerp(previous_vertex->z, current_vertex->z, t)};
            // use linear interpolation to get interpolated U and V texture
            // coords
            texture_uv_t interpolated_texcoord = {
                .u = flerp(previous_texcoord->u, current_texcoord->u, t),
                .v = flerp(previous_texcoord->v, current_texcoord->v, t)};
            // insert intersection point to inside_vertices array
            inside_vertices[num_inside_vertices] =
                vec3f_copy(&intersection_point);
            inside_tex_coords[num_inside_vertices] =
                texture_coords_clone(&interpolated_texcoord);
            num_inside_vertices++;
        }
        // if dot is greater than 0, it is inside plane
        if (is_greaterf(current_dot, 0)) {
            inside_vertices[num_inside_vertices] = vec3f_copy(current_vertex);
            inside_tex_coords[num_inside_vertices] =
                texture_coords_clone(current_texcoord);
            num_inside_vertices++;
        }
        // move on to next vertex
        previous_dot = current_dot;
        previous_vertex = current_vertex;
        previous_texcoord = current_texcoord;
        current_vertex++;
        current_texcoord++;
    }
    // copy all inside vertices (and tex_coords) to polygon reference
    for (int i = 0; i < num_inside_vertices; i++) {
        polygon->vertices[i] = vec3f_copy(&inside_vertices[i]);
        polygon->tex_coords[i] = texture_coords_clone(&inside_tex_coords[i]);
    }
    polygon->num_vertices = num_inside_vertices;
}

// frustum planes are defined by a point and a normal vector
// near plane   :  P=(0, 0, z_near), N=(0, 0,  1)
// far plane    :  P=(0, 0, z_far),  N=(0, 0, -1)
// top plane    :  P=(0, 0, 0),     N=(0, -cos(fov_y/2), sin(fov_y/2))
// bottom plane :  P=(0, 0, 0),     N=(0, cos(fov_y/2), sin(fov_y/2))
// left plane   :  P=(0, 0, 0),     N=(cos(fov_x/2), 0, sin(fov_x/2))
// right plane  :  P=(0, 0, 0),     N=(-cos(fov_x/2), 0, sin(fov_x/2))
//           /|\
//         /  | |
//       /\   | |
//     /      | |
//  P*|-->  <-|*|   ----> +z-axis
//     \      | |
//       \/   | |
//         \  | |
//           \|/
void init_frustum_planes(float fov_x, float fov_y, float z_near, float z_far) {
    float cos_half_fov_x = cosf(fov_x / 2.0f);
    float sin_half_fov_x = sinf(fov_x / 2.0f);
    float cos_half_fov_y = cosf(fov_y / 2.0f);
    float sin_half_fov_y = sinf(fov_y / 2.0f);

    frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3f_zero();
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov_x;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3f_zero();
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov_x;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

    frustum_planes[TOP_FRUSTUM_PLANE].point = vec3f_zero();
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov_y;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3f_zero();
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov_y;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

    frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3f_new(0, 0, z_near);
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

    frustum_planes[FAR_FRUSTUM_PLANE].point = vec3f_new(0, 0, z_far);
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

polygon_t polygon_from_triangle(vec3f_t p0, vec3f_t p1, vec3f_t p2,
                                texture_uv_t t0, texture_uv_t t1,
                                texture_uv_t t2) {
    polygon_t poly = {
        .vertices = {p0, p1, p2},
        .tex_coords = {t0, t1, t2},
        .num_vertices = 3,
    };
    return poly;
}

void clip_polygon(polygon_t *poly) {
    clip_polygon_against_plane(poly, LEFT_FRUSTUM_PLANE);
    clip_polygon_against_plane(poly, RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(poly, TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(poly, BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(poly, NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(poly, FAR_FRUSTUM_PLANE);
}

void triangles_from_polygon(polygon_t *polygon, triangle_t triangles[],
                            int *num_of_triangles) {
    int index0 = 0;
    for (int i = 0; i < polygon->num_vertices - 2; i++) {
        int index1 = i + 1;
        int index2 = i + 2;
        triangles[i].points[0] = vec4f_from_vec3f(polygon->vertices[index0]);
        triangles[i].points[1] = vec4f_from_vec3f(polygon->vertices[index1]);
        triangles[i].points[2] = vec4f_from_vec3f(polygon->vertices[index2]);
        triangles[i].tex_coords[0] = polygon->tex_coords[index0];
        triangles[i].tex_coords[1] = polygon->tex_coords[index1];
        triangles[i].tex_coords[2] = polygon->tex_coords[index2];
    }
    *num_of_triangles = polygon->num_vertices - 2;
}

