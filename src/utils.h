#ifndef INTERPLAY_UTILS_H_
#define INTERPLAY_UTILS_H_

#include <SDL_pixels.h>
#include <microui.h>
#include <stdbool.h>

#include "./globals.h"

bool is_equalf(float a, float b);

bool is_greater_or_equalf(float a, float b);

bool is_less_or_equalf(float a, float b);

bool is_greaterf(float a, float b);

bool is_lesserf(float a, float b);

SDL_Color uint32_to_sdl_color(color_t color);

color_t sdl_color_to_uint32(SDL_Color color);

SDL_Color mu_color_to_sdl_color(mu_Color color);

float flerp(float a, float b, float t);

float to_radians(float degrees);

bool can_load_file(const char *base_path, const char *filename);

char *extract_filename(const char *filepath, const char *required_extension);

#endif  // INTERPLAY_UTILS_H_
