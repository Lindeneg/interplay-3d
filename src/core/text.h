#ifndef INTERPLAY_CORE_TEXT_H_
#define INTERPLAY_CORE_TEXT_H_

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "../globals.h"
#include "../math/vec.h"

typedef struct text_texture {
    int width, height;
    color_t color;
    SDL_Texture *texture;
    vec2i_t position;
} text_texture_t;

bool text_init_ttf(SDL_Renderer *renderer);

text_texture_t text_create_texture();

void text_update_texture(text_texture_t *text_texture, const char *text,
                         color_t color);

void text_destroy_texture(text_texture_t *text_texture);

void text_destroy_ttf(void);

#endif  // INTERPLAY_CORE_TEXT_H_
