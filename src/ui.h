#ifndef INTERPLAY_UI_H_
#define INTERPLAY_UI_H_

#include <SDL2/SDL_events.h>
#include <microui.h>
#include <stdbool.h>

extern mu_Context *ctx;

bool ui_init(void);

void ui_process_input_event(SDL_Event *event);

void ui_process_render(void);

void ui_destroy();

#endif  // INTERPLAY_UI_H_
