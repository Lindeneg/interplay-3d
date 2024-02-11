#include "utils.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "./core/mem.h"
#include "./log.h"

global_internal bool equal_rel(float a, float b) {
    return (fabsf(a - b) <= (fmaxf(fabsf(a), fabsf(b)) * REL_EPSILON));
}

bool is_equalf(float a, float b) {
    return (fabsf(a - b) <= ABS_EPSILON) || equal_rel(a, b);
}

bool is_greater_or_equalf(float a, float b) {
    return (is_equalf(a, b)) || a > b;
}

bool is_less_or_equalf(float a, float b) { return (is_equalf(a, b)) || a < b; }

bool is_greaterf(float a, float b) { return is_equalf(a, b) ? false : a > b; }

bool is_lesserf(float a, float b) { return is_equalf(a, b) ? false : a < b; }

SDL_Color uint32_to_sdl_color(color_t color) {
    return (SDL_Color){.a = (color >> 24) & 0xFF,
                       .r = (color >> 16) & 0xFF,
                       .g = (color >> 8) & 0xFF,
                       .b = color & 0xFF};
}

color_t sdl_color_to_uint32(SDL_Color color) {
    return (uint32_t)((color.a << 24) + (color.r << 16) + (color.g << 8) +
                      (color.b << 0));
}

SDL_Color mu_color_to_sdl_color(mu_Color color) {
    return (SDL_Color){.a = color.a, .r = color.r, .g = color.g, .b = color.b};
}

float flerp(float a, float b, float t) { return a + t * (b - a); }

float to_radians(float degrees) { return (degrees * PI) / 180.0f; }

float to_degrees(float radians) { return (radians * 180.0f) / PI; }

bool can_load_file(const char *base_path, const char *filename) {
    FILE *file;
    char path[256];
    sprintf(path, "%s/%s", base_path, filename);
    fopen_s(&file, path, "r");
    if (!file) {
        return false;
    }
    fclose(file);
    return true;
}

char *extract_filename(const char *filepath, const char *required_extension) {
    if (!filepath || *filepath == '\0') {
        log_error("%s is not a valid filepath", filepath);
        return NULL;
    }
    const char *last_slash = strrchr(filepath, '/');
    int offset = 1;
    if (!last_slash) {
        last_slash = filepath;
        offset = 0;
    }
    const char *dot = strchr(last_slash, '.');
    if (!dot) {
        return NULL;
    }
    if (strcmp(dot + 1, required_extension) != 0) {
        log_error("%s does not have required extension %s", filepath,
                  required_extension);
        return NULL;
    }
    size_t length = dot - (last_slash + offset);
    char *filename = reserve_mem(length + 1);
    if (!filename) {
        return NULL;
    }
    strncpy_s(filename, length + 1, last_slash + offset, length);
    filename[length] = '\0';
    return filename;
}

