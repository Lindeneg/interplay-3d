#include "camera.h"

#include "../math/mat4.h"

camera_t camera = {.position = {0, 0, 0},
                   .direction = {0, 0, 0},
                   .forward_velocity = {0, 0, 0},
                   .yaw = 0,
                   .pitch = 0};

void camera_init(vec3f_t position, vec3f_t direction) {
    camera.position = position;
    camera.direction = direction;
}

vec3f_t camera_get_look_at_target(void) {
    // initialize target looking at positive z-axis
    vec3f_t target = {0, 0, 1};
    mat4f_t camera_yaw_rotation = mat4f_make_rotation_y(camera.yaw);
    mat4f_t camera_pitch_rotation = mat4f_make_rotation_x(camera.pitch);
    // create camera rotation matrix based on yaw and pitch
    mat4f_t camera_rotation = mat4f_identity();
    camera_rotation = mat4f_mul_mat4f(camera_pitch_rotation, camera_rotation);
    camera_rotation = mat4f_mul_mat4f(camera_yaw_rotation, camera_rotation);
    // update camera direction based on rotation
    vec4f_t camera_direction =
        mat4f_mul_vec4f(camera_rotation, vec4f_from_vec3f(target));
    camera.direction = vec3f_from_vec4f(camera_direction);
    // offset camera position in direction where camera is pointing at
    target = vec3f_add_new(camera.position, camera.direction);
    return target;
}

void camera_process_input_event(SDL_Event *event, float dt) {
    switch (event->type) {
        case SDL_KEYDOWN: {
            if (event->key.keysym.sym == SDLK_w) {
                camera.forward_velocity =
                    vec3f_multiply_new(camera.direction, 5.0f * dt);
                vec3f_add(&camera.position, &camera.forward_velocity);
                break;
            }
            if (event->key.keysym.sym == SDLK_s) {
                camera.forward_velocity =
                    vec3f_multiply_new(camera.direction, -5.0f * dt);
                vec3f_add(&camera.position, &camera.forward_velocity);
                break;
            }
            if (event->key.keysym.sym == SDLK_RIGHT) {
                camera.yaw += 1.0f * dt;
                break;
            }
            if (event->key.keysym.sym == SDLK_LEFT) {
                camera.yaw -= 1.0f * dt;
                break;
            }
            if (event->key.keysym.sym == SDLK_UP) {
                camera.pitch -= 1.0f * dt;
                break;
            }
            if (event->key.keysym.sym == SDLK_DOWN) {
                camera.pitch += 1.0f * dt;
                break;
            }
            break;
        }
    }
}
