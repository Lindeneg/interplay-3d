#ifndef INTERPLAY_GLOBALS_H_
#define INTERPLAY_GLOBALS_H_

#include <stdint.h>

#define global_internal static

#define local_persist static

#define PIXEL_FORMAT SDL_PIXELFORMAT_ARGB8888

#define REL_EPSILON 1e-8

#define ABS_EPSILON 1e-12

#define PI 3.14159265359

#define FPS 60

#define FRAME_TARGET (1000 / FPS)

typedef uint32_t color_t;
typedef uint32_t color_buffer_t;

#endif  // INTERPLAY_GLOBALS_H_
