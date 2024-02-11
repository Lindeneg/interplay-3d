#include "light.h"

#include "../utils.h"

light_t light = {.direction = {0, 0, 0}};

void light_init(vec3f_t direction) { light.direction = direction; }

color_t light_apply_intensity(color_t color, float factor) {
    if (is_lesserf(factor, 0.0f)) {
        factor = 0.0f;
    } else if (is_greaterf(factor, 1.0f)) {
        factor = 1.0f;
    }
    color_t a = (color & 0xFF000000);
    color_t r = (color & 0x00FF0000) * factor;
    color_t g = (color & 0x0000FF00) * factor;
    color_t b = (color & 0x000000FF) * factor;
    return (a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF));
}

