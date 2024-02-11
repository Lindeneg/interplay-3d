#ifndef INTERPLAY_CORE_TEXTURE_H_
#define INTERPLAY_CORE_TEXTURE_H_

#include <SDL_surface.h>

typedef struct texture {
    int id;
    char *name;
    SDL_Surface *data;
} texture_t;

typedef struct texture_uv {
    float u, v;
} texture_uv_t;

texture_uv_t texture_coords_clone(const texture_uv_t *t);

texture_t *texture_load(const char *filename);

void texture_free(texture_t *texture);

#endif  // INTERPLAY_CORE_TEXTURE_H_
