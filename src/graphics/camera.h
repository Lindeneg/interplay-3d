#ifndef INTERPLAY_GRAPHICS_CAMERA_H_
#define INTERPLAY_GRAPHICS_CAMERA_H_

#include <SDL_events.h>

#include "../math/vec.h"

typedef struct camera {
    vec3f_t position, direction, forward_velocity;
    float yaw, pitch;
} camera_t;

//-global-variable
extern camera_t camera;

void camera_init(vec3f_t position, vec3f_t direction);

void camera_process_input_event(SDL_Event *event, float dt);

vec3f_t camera_get_look_at_target(void);

#endif  // INTERPLAY_GRAPHICS_CAMERA_H_
