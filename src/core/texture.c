#include "texture.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <stdio.h>
#include <string.h>

#include "../globals.h"
#include "../log.h"
#include "../utils.h"
#include "./context.h"
#include "./mem.h"

global_internal bool texture_load_file(texture_t *texture,
                                       const char *filename) {
    char path[256];
    sprintf(path, "%s/%s", config.assets_folder, filename);
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        log_error("failed to load texture %s", filename);
        return false;
    }
    SDL_Surface *formatted_surface =
        SDL_ConvertSurfaceFormat(surface, PIXEL_FORMAT, 0);
    SDL_FreeSurface(surface);
    if (!formatted_surface) {
        log_error("failed to format texture %s", filename);
        return false;
    }
    texture->data = formatted_surface;
    return true;
}

texture_uv_t texture_coords_clone(const texture_uv_t *t) {
    return (texture_uv_t){t->u, t->v};
}

texture_t *texture_load(const char *filename) {
    local_persist const char *supported_extension = "png";
    char *texture_name = extract_filename(filename, supported_extension);
    if (!texture_name) {
        log_error("failed to extract filename from %s", filename);
        return NULL;
    }
    texture_t *prev_loaded_texture = NULL;
    linked_list_search_name(texture_t, prev_loaded_texture, textures,
                            texture_name);
    if (prev_loaded_texture) {
        log_debug("using previously loaded texture values for %s",
                  texture_name);
        free_mem(texture_name);
        return prev_loaded_texture;
    }
    texture_t *texture = (texture_t *)reserve_mem(sizeof(texture_t));
    if (!texture_load_file(texture, filename)) {
        free_mem(texture);
        free_mem(texture_name);
        return NULL;
    }
    texture->name = texture_name;
    texture->id = linked_list_append(&textures, texture);
    return texture;
}

void texture_free(texture_t *texture) {
    SDL_FreeSurface(texture->data);
    free_mem(texture->name);
    free_mem(texture);
}

