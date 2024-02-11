#ifndef INTERPLAY_MATH_MAT4_H_
#define INTERPLAY_MATH_MAT4_H_

#include "./vec.h"

typedef struct mat4f {
    float m[4][4];
} mat4f_t;

mat4f_t mat4f_identity(void);

mat4f_t mat4f_make_scale(float sx, float sy, float sz);

mat4f_t mat4f_make_translation(float tx, float ty, float tz);

mat4f_t mat4f_make_rotation_x(float angle);

mat4f_t mat4f_make_rotation_y(float angle);

mat4f_t mat4f_make_rotation_z(float angle);

vec4f_t mat4f_mul_vec4f(mat4f_t m, vec4f_t v);

mat4f_t mat4f_mul_mat4f(mat4f_t a, mat4f_t b);

mat4f_t mat4f_look_at(vec3f_t eye, vec3f_t target, vec3f_t up);

mat4f_t mat4f_make_perspective(float fov, float aspect, float z_near,
                               float z_far);

#endif  // INTERPLAY_MATH_MAT4_H_
