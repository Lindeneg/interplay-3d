#ifndef INTERPLAY_MATH_VEC_H_
#define INTERPLAY_MATH_VEC_H_

typedef struct vec2i {
    int x, y;
} vec2i_t;

vec2i_t vec2i_zero(void);

vec2i_t vec2i_new(int x, int y);

typedef struct vec2f {
    float x, y;
} vec2f_t;

vec2f_t vec2f_zero(void);

vec2f_t vec2f_new(float x, float y);

vec2f_t vec2f_copy(const vec2f_t *vec);

float vec2f_magnitude(const vec2f_t *vec);

void vec2f_add(vec2f_t *vec, const vec2f_t *other);

vec2f_t vec2f_add_new(vec2f_t vec, vec2f_t other);

void vec2f_subtract(vec2f_t *vec, const vec2f_t *other);

vec2f_t vec2f_subtract_new(vec2f_t vec, vec2f_t other);

void vec2f_multiply(vec2f_t *vec, float scale);

vec2f_t vec2f_multiply_new(vec2f_t vec, float scale);

void vec2f_divide(vec2f_t *vec, float scale);

vec2f_t vec2f_divide_new(vec2f_t vec, float scale);

void vec2f_normalize(vec2f_t *vec);

float vec2f_dot(vec2f_t a, vec2f_t b);

typedef struct vec3f {
    float x, y, z;
} vec3f_t;

vec3f_t vec3f_zero(void);

vec3f_t vec3f_new(float x, float y, float z);

vec3f_t vec3f_copy(const vec3f_t *vec);

float vec3f_magnitude(const vec3f_t *vec);

void vec3f_add(vec3f_t *vec, const vec3f_t *other);

vec3f_t vec3f_add_new(vec3f_t vec, vec3f_t other);

void vec3f_subtract(vec3f_t *vec, const vec3f_t *other);

vec3f_t vec3f_subtract_new(vec3f_t vec, vec3f_t other);

void vec3f_multiply(vec3f_t *vec, float scale);

vec3f_t vec3f_multiply_new(vec3f_t vec, float scale);

void vec3f_divide(vec3f_t *vec, float scale);

vec3f_t vec3f_divide_new(vec3f_t vec, float scale);

vec3f_t vec3f_cross(vec3f_t vec, vec3f_t other);

float vec3f_dot(vec3f_t vec, vec3f_t other);

void vec3f_normalize(vec3f_t *vec);

typedef struct vec4f {
    float x, y, z, w;
} vec4f_t;

vec2f_t vec2f_from_vec4f(vec4f_t vec);

vec3f_t vec3f_from_vec4f(vec4f_t vec);

vec4f_t vec4f_from_vec3f(vec3f_t vec);

#endif  // INTERPLAY_MATH_VEC_H_
