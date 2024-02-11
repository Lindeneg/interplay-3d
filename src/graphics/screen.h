#ifndef INTERPLAY_GRAPHICS_SCREEN_H_
#define INTERPLAY_GRAPHICS_SCREEN_H_

#include <stdbool.h>

#include "../core/text.h"
#include "../globals.h"
#include "../math/vec.h"

bool screen_init_sdl(void);

bool screen_set_size(vec2i_t size);

vec2i_t screen_get_size(void);

vec2i_t screen_get_display_size(void);

vec2f_t screen_get_size_ratio(void);

SDL_Renderer *screen_get_renderer(void);

void screen_put_back_buffer(int x, int y, color_t color);

void screen_put_z_buffer(int x, int y, float value);

void screen_put_text(text_texture_t *text_texture);

float screen_z_buffer_at(int x, int y);

void screen_clear_renderer(void);

void screen_swap_buffers(void);

void screen_render_present(void);

void screen_clear_back_buffer(void);

void screen_clear_z_buffer(void);

void screen_destroy(void);

#endif  // INTERPLAY_GRAPHICS_SCREEN_H_
