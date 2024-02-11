#include "screen.h"

#include <SDL_image.h>
#include <SDL_render.h>
#include <assert.h>

#include "../core/context.h"
#include "../core/mem.h"
#include "../log.h"

global_internal bool sdl_initialized = false;
global_internal int width = 0;
global_internal int height = 0;
global_internal color_t clear_color = 0;
global_internal float *z_buffer = NULL;
global_internal color_buffer_t *back_buffer = NULL;
global_internal SDL_Texture *front_buffer = NULL;
global_internal SDL_Renderer *renderer = NULL;
global_internal SDL_Window *window = NULL;
global_internal SDL_DisplayMode display_mode;

global_internal bool setup_sdl_window(void) {
    if (window == NULL) {
        window = SDL_CreateWindow("Interplay 3D", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, display_mode.w,
                                  display_mode.h, SDL_WINDOW_BORDERLESS);
        if (!window) {
            log_error("failed to create SDL window %s", SDL_GetError());
            return false;
        }
    }
    if (renderer == NULL) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            log_error("failed to create SDL renderer %s", SDL_GetError());
            SDL_DestroyWindow(window);
            window = NULL;
            return false;
        }
    }
    if (front_buffer != NULL) {
        SDL_DestroyTexture(front_buffer);
    }
    front_buffer = SDL_CreateTexture(
        renderer, PIXEL_FORMAT, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!front_buffer) {
        log_error("failed to create SDL texture %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        renderer = NULL;
        window = NULL;
        return false;
    }
    return true;
}

// initializes SDL core and SDL image
bool screen_init_sdl(void) {
    if (sdl_initialized) {
        log_warning("trying to initialize already-initialized SDL");
        return false;
    }
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
        log_error("failed to initialize SDL %s", SDL_GetError());
        return false;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        log_error("failed to initialize SDL Image %s", SDL_GetError());
        return false;
    }
    SDL_GetCurrentDisplayMode(0, &display_mode);
    sdl_initialized = true;
    log_verbose("initialized SDL video, timer and events");
    return true;
}

// sets screen size and (re)allocates buffers accordingly
bool screen_set_size(vec2i_t size) {
    // check if size has actually changed
    if (size.x != 0 && size.y != 0 && size.x == width && size.y == height) {
        log_warning("trying to resize screen to same size already set");
        return false;
    }
    // sdl must be initialized at this point
    if (!sdl_initialized) {
        log_error("please initialize SDL before setting screen size");
        return false;
    }
    // use fullscreen if no size is set
    if (size.x == 0 && size.y == 0) {
        size.x = display_mode.w;
        size.y = display_mode.h;
    }
    // set new screen dimensions
    width = size.x;
    height = size.y;
    // setup (or reinitialize) window
    if (!setup_sdl_window()) {
        return false;
    }
    // check if we already have buffers allocated
    if (z_buffer != NULL) {
        free_mem(z_buffer);
        z_buffer = NULL;
    }
    if (back_buffer != NULL) {
        free_mem(back_buffer);
        back_buffer = NULL;
    }
    // allocate memory for new buffers
    back_buffer = (uint32_t *)reserve_mem(sizeof(uint32_t) * width * height);
    z_buffer = (float *)reserve_mem(sizeof(float) * width * height);
    // set clear color and clear back buffer with it
    clear_color = 0xFF000000;
    // update config values
    config.width = width;
    config.height = height;
    screen_clear_back_buffer();
    screen_clear_z_buffer();
    log_debug("screen: (%d,%d) buffer: (%d,%d)", display_mode.w, display_mode.h,
              width, height);
    return true;
}

vec2i_t screen_get_size(void) { return (vec2i_t){width, height}; }

vec2f_t screen_get_size_ratio(void) {
    return vec2f_new((float)width / (float)display_mode.w,
                     (float)height / (float)display_mode.h);
}

SDL_Renderer *screen_get_renderer(void) {
    assert(renderer != NULL && "renderer not initialized");
    return renderer;
}

void screen_put_back_buffer(int x, int y, color_t color) {
    if (back_buffer == NULL) return;
    if (x >= 0 && x < width && y >= 0 && y < height) {
        back_buffer[(width * y) + x] = color;
    }
}

void screen_put_z_buffer(int x, int y, float value) {
    if (z_buffer == NULL) return;
    if (x >= 0 && x < width && y >= 0 && y < height) {
        z_buffer[(width * y) + x] = value;
    }
}

void screen_put_text(text_texture_t *text_texture) {
    SDL_Rect renderQuad = {text_texture->position.x, text_texture->position.y,
                           text_texture->width, text_texture->height};
    SDL_RenderCopyEx(renderer, text_texture->texture, NULL, &renderQuad, 0.0,
                     NULL, SDL_FLIP_NONE);
}

float screen_z_buffer_at(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 1.0f;
    }
    return z_buffer[(width * y) + x];
}

void screen_clear_renderer(void) {
    if (renderer == NULL) return;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void screen_swap_buffers(void) {
    if (front_buffer == NULL || back_buffer == NULL || renderer == NULL) return;
    SDL_UpdateTexture(front_buffer, NULL, back_buffer,
                      (int)(width) * (int)(sizeof(uint32_t)));
    SDL_RenderCopyEx(renderer, front_buffer, NULL, NULL, 0, NULL, 0);
}

void screen_render_present(void) {
    if (renderer == NULL) return;
    SDL_RenderPresent(renderer);
}

void screen_clear_back_buffer(void) {
    for (int i = 0; i < width * height; i++) {
        back_buffer[i] = clear_color;
    }
}

void screen_clear_z_buffer(void) {
    for (int i = 0; i < width * height; i++) {
        z_buffer[i] = 1.0f;
    }
}

void screen_destroy(void) {
    free_mem(back_buffer);
    back_buffer = NULL;
    free_mem(z_buffer);
    z_buffer = NULL;
    SDL_DestroyTexture(front_buffer);
    front_buffer = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
    sdl_initialized = false;
}

