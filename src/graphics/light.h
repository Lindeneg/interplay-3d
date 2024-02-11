#ifndef INTERPLAY_GRAPHICS_LIGHT_H_
#define INTERPLAY_GRAPHICS_LIGHT_H_

#include "../globals.h"
#include "../math/vec.h"

typedef struct light {
    vec3f_t direction;
} light_t;

typedef struct spotlight {
    vec3f_t position, direction;
} spotlight_t;

//-global-variable
extern light_t light;

void light_init(vec3f_t direction);

color_t light_apply_intensity(color_t color, float factor);

#endif  // INTERPLAY_GRAPHICS_LIGHT_H_
