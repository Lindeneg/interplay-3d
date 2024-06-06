#include "text.h"

#include <SDL2/SDL_ttf.h>

#include "../log.h"
#include "../utils.h"
#include "./context.h"

// just one font supported
global_internal TTF_Font *font = NULL;
// non-owning ptr to screen renderer
global_internal SDL_Renderer *renderer = NULL;

void text_init_texture(text_texture_t *text_texture, const char *text,
                       color_t color) {
    if (text_texture->texture) {
        log_error("failed to create text, text_texture already created");
        return;
    }
    if (!font || !renderer) {
        log_error("failed to create text, TTF is not initialized");
        return;
    }
    SDL_Surface *text_surface =
        TTF_RenderText_Solid(font, text, uint32_to_sdl_color(color));
    if (!text_surface) {
        log_error("failed to create TTF SDL surface %s", TTF_GetError());
        return;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (!texture) {
        log_error("failed to create TTF SDL texture %s", SDL_GetError());
        return;
    }
    // set members
    text_texture->texture = texture;
    text_texture->color = color;
    text_texture->width = text_surface->w;
    text_texture->height = text_surface->h;
    // free temp used surface
    SDL_FreeSurface(text_surface);
}

// initialize TTF and obtain ptr to renderer
bool text_init_ttf(SDL_Renderer *screen_renderer) {
    local_persist int font_size = 12;
    if (TTF_Init() != 0) {
        log_error("failed to initialize SDL TTF %s", SDL_GetError());
        return false;
    }
    font = TTF_OpenFont(config.ttf_file, font_size);
    if (!font) {
        log_error("failed to load TTF %s", SDL_GetError());
        return false;
    }
    renderer = screen_renderer;
    log_verbose("initialized TTF file %s with size %d", config.ttf_file,
                font_size);
    return true;
}

text_texture_t text_create_texture() {
    return (text_texture_t){.texture = NULL,
                            .position = {0, 0},
                            .color = 0,
                            .width = 0,
                            .height = 0};
}

void text_update_texture(text_texture_t *text_texture, const char *text,
                         color_t color) {
    if (!font || !renderer) {
        log_warning("failed to update text, TTF is not initialized");
        return;
    }
    // remove already set texture
    text_destroy_texture(text_texture);
    // update texture with new configuration
    text_init_texture(text_texture, text, color);
}

void text_destroy_texture(text_texture_t *text_texture) {
    if (text_texture->texture) {
        SDL_DestroyTexture(text_texture->texture);
        text_texture->texture = NULL;
    }
}

void text_destroy_ttf(void) {
    if (font == NULL) return;
    TTF_CloseFont(font);
    font = NULL;
    renderer = NULL;
    TTF_Quit();
}

