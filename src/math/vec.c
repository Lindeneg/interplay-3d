#include "vec.h"

#include <math.h>

/* Implementation: vec2i_t */

vec2i_t vec2i_zero(void) { return (vec2i_t){0, 0}; }

vec2i_t vec2i_new(int x, int y) { return (vec2i_t){x, y}; }

/* Implementation: vec2f_t */

vec2f_t vec2f_zero(void) { return (vec2f_t){0.0f, 0.0f}; }

vec2f_t vec2f_new(float x, float y) { return (vec2f_t){x, y}; }

float vec2f_magnitude(const vec2f_t *vec) {
    return sqrtf((vec->x * vec->x) + (vec->y * vec->y));
}

vec2f_t vec2f_copy(const vec2f_t *vec) { return vec2f_new(vec->x, vec->y); }

void vec2f_add(vec2f_t *vec, const vec2f_t *other) {
    vec->x += other->x;
    vec->y += other->y;
}

vec2f_t vec2f_add_new(vec2f_t vec, vec2f_t other) {
    return vec2f_new(vec.x + other.x, vec.y + other.y);
}

void vec2f_subtract(vec2f_t *vec, const vec2f_t *other) {
    vec->x -= other->x;
    vec->y -= other->y;
}

vec2f_t vec2f_subtract_new(vec2f_t vec, vec2f_t other) {
    return vec2f_new(vec.x - other.x, vec.y - other.y);
}

void vec2f_multiply(vec2f_t *vec, float scale) {
    vec->x *= scale;
    vec->y *= scale;
}

vec2f_t vec2f_multiply_new(vec2f_t vec, float scale) {
    return vec2f_new(vec.x * scale, vec.y * scale);
}

void vec2f_divide(vec2f_t *vec, float scale) {
    vec->x /= scale;
    vec->y /= scale;
}

vec2f_t vec2f_divide_new(vec2f_t vec, float scale) {
    return vec2f_new(vec.x / scale, vec.y / scale);
}

void vec2f_normalize(vec2f_t *vec) {
    float magnitude = vec2f_magnitude(vec);
    vec->x /= magnitude;
    vec->y /= magnitude;
}

float vec2f_dot(vec2f_t a, vec2f_t b) { return (a.x * b.x) + (a.y * b.y); }

/* Implementation: vec3f_t */

vec3f_t vec3f_zero(void) { return (vec3f_t){0.0f, 0.0f, 0.0f}; }

vec3f_t vec3f_new(float x, float y, float z) { return (vec3f_t){x, y, z}; }

vec3f_t vec3f_copy(const vec3f_t *vec) {
    return vec3f_new(vec->x, vec->y, vec->z);
}

float vec3f_magnitude(const vec3f_t *vec) {
    return sqrtf((vec->x * vec->x) + (vec->y * vec->y) + (vec->z * vec->z));
}

void vec3f_add(vec3f_t *vec, const vec3f_t *other) {
    vec->x += other->x;
    vec->y += other->y;
    vec->z += other->z;
}

vec3f_t vec3f_add_new(vec3f_t vec, vec3f_t other) {
    return vec3f_new(vec.x + other.x, vec.y + other.y, vec.z + other.z);
}

void vec3f_subtract(vec3f_t *vec, const vec3f_t *other) {
    vec->x -= other->x;
    vec->y -= other->y;
    vec->z -= other->z;
}

vec3f_t vec3f_subtract_new(vec3f_t vec, vec3f_t other) {
    return vec3f_new(vec.x - other.x, vec.y - other.y, vec.z - other.z);
}

void vec3f_multiply(vec3f_t *vec, float scale) {
    vec->x *= scale;
    vec->y *= scale;
    vec->z *= scale;
}

vec3f_t vec3f_multiply_new(vec3f_t vec, float scale) {
    return vec3f_new(vec.x * scale, vec.y * scale, vec.z * scale);
}

void vec3f_divide(vec3f_t *vec, float scale) {
    vec->x /= scale;
    vec->y /= scale;
    vec->z /= scale;
}

vec3f_t vec3f_divide_new(vec3f_t vec, float scale) {
    return vec3f_new(vec.x / scale, vec.y / scale, vec.z / scale);
}

vec3f_t vec3f_cross(vec3f_t vec, vec3f_t other) {
    return (vec3f_t){.x = (vec.y * other.z) - (vec.z * other.y),
                     .y = (vec.z * other.x) - (vec.x * other.z),
                     .z = (vec.x * other.y) - (vec.y * other.x)};
}

float vec3f_dot(vec3f_t vec, vec3f_t other) {
    return (vec.x * other.x) + (vec.y * other.y) + (vec.z * other.z);
}

void vec3f_normalize(vec3f_t *vec) {
    float magnitude = vec3f_magnitude(vec);
    vec->x /= magnitude;
    vec->y /= magnitude;
    vec->z /= magnitude;
}

/* Conversion implementation */

vec2f_t vec2f_from_vec4f(vec4f_t vec) { return vec2f_new(vec.x, vec.y); }

vec3f_t vec3f_from_vec4f(vec4f_t vec) { return vec3f_new(vec.x, vec.y, vec.z); }

vec4f_t vec4f_from_vec3f(vec3f_t vec) {
    return (vec4f_t){vec.x, vec.y, vec.z, 1.0f};
}

