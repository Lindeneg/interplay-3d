#include "triangle.h"

#include "./renderer.h"
#include "./screen.h"
#include "../utils.h"

#define ptr_swap(type, ptr1, ptr2) \
    type tmp = *ptr1;              \
    *ptr1 = *ptr2;                 \
    *ptr2 = tmp;

#define sort_triangle_points(type, p0, p1, p2) \
    if (p0->y > p1->y) {                       \
        ptr_swap(type, p0, p1)                 \
    }                                          \
    if (p1->y > p2->y) {                       \
        ptr_swap(type, p1, p2)                 \
    }                                          \
    if (p0->y > p1->y) {                       \
        ptr_swap(type, p0, p1)                 \
    }

#define to_vec3f(var, obj) \
    vec4f_t var = {(float)obj->x, (float)obj->y, (float)obj->z, (float)obj->w};

global_internal void int_swap(int *a, int *b){ptr_swap(int, a, b)}

// find barycentric weights alpha, beta, and gamma for point p
//         (B)
//         /|\
//        / | \
//       /  |  \
//      /  (P)  \
//     /  /   \  \
//    / /       \ \
//   //           \\
//  (A)------------(C)
global_internal vec3f_t
    barycentric_weights(vec2f_t a, vec2f_t b, vec2f_t c, vec2f_t p) {
    // find the vectors between the vertices ABC and point p
    vec2f_t ac = vec2f_subtract_new(c, a);
    vec2f_t ab = vec2f_subtract_new(b, a);
    vec2f_t ap = vec2f_subtract_new(p, a);
    vec2f_t pc = vec2f_subtract_new(c, p);
    vec2f_t pb = vec2f_subtract_new(b, p);
    // area of ABC using 2D cross product is || AC x AB ||
    float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x);
    // alpha is area of PBC divided by ABC
    float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;
    // beta is area of APC divided by ABC
    float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;
    // gamma is easily found since barycentric coordinates always add up to 1
    float gamma = 1 - alpha - beta;
    return vec3f_new(alpha, beta, gamma);
}

// draw pixel at position (x,y) with depth interpolation
global_internal void draw_triangle_pixel(int x, int y, vec4f_t point_a,
                                         vec4f_t point_b, vec4f_t point_c,
                                         color_t color) {
    // create three vec2's to find interpolation
    vec2f_t p = vec2f_new((float)x, (float)y);
    vec2f_t a = vec2f_from_vec4f(point_a);
    vec2f_t b = vec2f_from_vec4f(point_b);
    vec2f_t c = vec2f_from_vec4f(point_c);
    // calculate barycentric coordinates point 'p' inside triangle
    vec3f_t weights = barycentric_weights(a, b, c, p);
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;
    // interpolate value of 1/w for current pixel
    float interpolated_reciprocal_w = (1.0f / point_a.w) * alpha +
                                      (1.0f / point_b.w) * beta +
                                      (1.0f / point_c.w) * gamma;
    // adjust 1/w so pixels that are closer to camera have smaller value
    interpolated_reciprocal_w = 1.0f - interpolated_reciprocal_w;
    // only draw pixel if depth is less than previously stored in z-buffer
    if (is_lesserf(interpolated_reciprocal_w, screen_z_buffer_at(x, y))) {
        screen_put_back_buffer(x, y, color);
        screen_put_z_buffer(x, y, interpolated_reciprocal_w);
    }
}

// draw textured pixel at position (x,y) with depth interpolation
global_internal void draw_textured_triangle_pixel(
    int x, int y, vec4f_t point_a, vec4f_t point_b, vec4f_t point_c,
    texture_uv_t a_uv, texture_uv_t b_uv, texture_uv_t c_uv,
    const texture_t *texture) {
    if (!texture || !texture->data) return;
    // create three vec2's to find interpolation
    vec2f_t p = vec2f_new((float)x, (float)y);
    vec2f_t a = vec2f_from_vec4f(point_a);
    vec2f_t b = vec2f_from_vec4f(point_b);
    vec2f_t c = vec2f_from_vec4f(point_c);
    // calculate barycentric coordinates point 'p' inside triangle
    vec3f_t weights = barycentric_weights(a, b, c, p);
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;
    // declare interpolated U V 1/w
    float interpolated_u;
    float interpolated_v;
    float interpolated_inv_w;
    // interpolate all U/w and V/w values using barycentric weights and factor
    // 1/w
    interpolated_u = (a_uv.u / point_a.w) * alpha +
                     (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    interpolated_v = (a_uv.v / point_a.w) * alpha +
                     (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;
    // also interpolate 1/w for current pixel
    interpolated_inv_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta +
                         (1 / point_c.w) * gamma;
    // divide back both interpolated values by 1/w
    interpolated_u /= interpolated_inv_w;
    interpolated_v /= interpolated_inv_w;
    // map UV coordinates to full texture width and height
    int tex_x =
        abs((int)(interpolated_u * (float)texture->data->w)) % texture->data->w;
    int tex_y =
        abs((int)(interpolated_v * (float)texture->data->h)) % texture->data->h;
    // adjust 1/w so pixels that are closer to camera have smaller value
    interpolated_inv_w = 1.0f - interpolated_inv_w;
    // only draw pixel if depth is less than previously stored in z-buffer
    if (is_lesserf(interpolated_inv_w, screen_z_buffer_at(x, y))) {
        screen_put_back_buffer(
            x, y,
            ((uint32_t *)
                 texture->data->pixels)[(texture->data->w * tex_y) + tex_x]);
        screen_put_z_buffer(x, y, interpolated_inv_w);
    }
}

vec3f_t get_triangle_normal(vec4f_t vertices[3]) {
    // Get individual vectors from A, B, and C vertices to compute normal
    vec3f_t vector_a = vec3f_from_vec4f(vertices[0]); /*   A   */
    vec3f_t vector_b = vec3f_from_vec4f(vertices[1]); /*  / \  */
    vec3f_t vector_c = vec3f_from_vec4f(vertices[2]); /* C---B */
    // Get the vector subtraction of B-A and C-A
    vec3f_t vector_ab = vec3f_subtract_new(vector_b, vector_a);
    vec3f_t vector_ac = vec3f_subtract_new(vector_c, vector_a);
    vec3f_normalize(&vector_ab);
    vec3f_normalize(&vector_ac);
    // Compute the face normal (using cross product to find perpendicular)
    vec3f_t normal = vec3f_cross(vector_ab, vector_ac);
    vec3f_normalize(&normal);
    return normal;
}

void draw_triangle(vec2i_t p0, vec2i_t p1, vec2i_t p2, color_t color) {
    draw_line(p0, p1, color);
    draw_line(p1, p2, color);
    draw_line(p2, p0, color);
}

// draw filled triangle with flat-top/flat-bottom method
// split original triangle in two, half flat-bottom and half flat-top
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)---------\
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
void draw_filled_triangle(filled_triangle_point_t *p0,
                          filled_triangle_point_t *p1,
                          filled_triangle_point_t *p2, uint32_t color) {
    // sort vertices by y-coordinate ascending
    sort_triangle_points(filled_triangle_point_t, p0, p1, p2)
        // create three vector points after sorting
        to_vec3f(point_a, p0) to_vec3f(point_b, p1) to_vec3f(point_c, p2)
        // render upper part (flat-bottom)
        float inv_slope_1 = 0.0f;
    float inv_slope_2 = 0.0f;
    if (p1->y - p0->y != 0)
        inv_slope_1 = (float)(p1->x - p0->x) / (float)abs(p1->y - p0->y);
    if (p2->y - p0->y != 0)
        inv_slope_2 = (float)(p2->x - p0->x) / (float)abs(p2->y - p0->y);
    if (p1->y - p0->y != 0) {
        for (int y = p0->y; y <= p1->y; y++) {
            int x_start = p1->x + (y - p1->y) * inv_slope_1;
            int x_end = p0->x + (y - p0->y) * inv_slope_2;
            if (x_end < x_start) {
                // swap if x_start is to the right of x_end
                int_swap(&x_start, &x_end);
            }
            for (int x = x_start; x < x_end; x++) {
                draw_triangle_pixel(x, y, point_a, point_b, point_c, color);
            }
        }
    }
    // render bottom part (flat-top)
    inv_slope_1 = 0;
    inv_slope_2 = 0;
    if (p2->y - p1->y != 0)
        inv_slope_1 = (float)(p2->x - p1->x) / (float)abs(p2->y - p1->y);
    if (p2->y - p0->y != 0)
        inv_slope_2 = (float)(p2->x - p0->x) / (float)abs(p2->y - p0->y);
    if (p2->y - p1->y != 0) {
        for (int y = p1->y; y <= p2->y; y++) {
            int x_start = p1->x + (y - p1->y) * inv_slope_1;
            int x_end = p0->x + (y - p0->y) * inv_slope_2;
            if (x_end < x_start) {
                // swap if x_start is to the right of x_end
                int_swap(&x_start, &x_end);
            }
            for (int x = x_start; x < x_end; x++) {
                draw_triangle_pixel(x, y, point_a, point_b, point_c, color);
            }
        }
    }
}

// draw textured triangle based on a texture array of colors.
// split the original triangle in two, half flat-bottom and half flat-top.
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------\
//    \_        \
//       \_      \
//          \_    \
//             \_  \
//                \ \
//                  \\
//                    v2
void draw_textured_triangle(textured_triangle_point_t *p0,
                            textured_triangle_point_t *p1,
                            textured_triangle_point_t *p2,
                            const texture_t *texture) {
    // sort vertices by y-coordinate ascending
    sort_triangle_points(textured_triangle_point_t, p0, p1, p2)
        // flip V to account for inverted UV-coordinates (V grows downwards)
        p0->v = 1.0f - p0->v;
    p1->v = 1.0f - p1->v;
    p2->v = 1.0f - p2->v;
    // create vector points and texture coords after sorting
    to_vec3f(point_a, p0) to_vec3f(point_b, p1) to_vec3f(point_c, p2)
        // create texture coords from UV's
        texture_uv_t a_uv = {p0->u, p0->v};
    texture_uv_t b_uv = {p1->u, p1->v};
    texture_uv_t c_uv = {p2->u, p2->v};
    // render upper part (flat-bottom)
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;
    if (p1->y - p0->y != 0)
        inv_slope_1 = (float)(p1->x - p0->x) / (float)abs(p1->y - p0->y);
    if (p2->y - p0->y != 0)
        inv_slope_2 = (float)(p2->x - p0->x) / (float)abs(p2->y - p0->y);
    if (p1->y - p0->y != 0) {
        for (int y = p0->y; y <= p1->y; y++) {
            int x_start = p1->x + (y - p1->y) * inv_slope_1;
            int x_end = p0->x + (y - p0->y) * inv_slope_2;
            if (x_end < x_start) {
                // swap if x_start is to the right of x_end
                int_swap(&x_start, &x_end);
            }
            for (int x = x_start; x < x_end; x++) {
                draw_textured_triangle_pixel(x, y, point_a, point_b, point_c,
                                             a_uv, b_uv, c_uv, texture);
            }
        }
    }
    // render bottom part (flat-top)
    inv_slope_1 = 0;
    inv_slope_2 = 0;
    if (p2->y - p1->y != 0)
        inv_slope_1 = (float)(p2->x - p1->x) / (float)abs(p2->y - p1->y);
    if (p2->y - p0->y != 0)
        inv_slope_2 = (float)(p2->x - p0->x) / (float)abs(p2->y - p0->y);
    if (p2->y - p1->y != 0) {
        for (int y = p1->y; y <= p2->y; y++) {
            int x_start = p1->x + (y - p1->y) * inv_slope_1;
            int x_end = p0->x + (y - p0->y) * inv_slope_2;
            if (x_end < x_start) {
                // swap if x_start is to the right of x_end
                int_swap(&x_start, &x_end);
            }
            for (int x = x_start; x < x_end; x++) {
                draw_textured_triangle_pixel(x, y, point_a, point_b, point_c,
                                             a_uv, b_uv, c_uv, texture);
            }
        }
    }
}

