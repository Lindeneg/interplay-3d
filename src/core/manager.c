#include "manager.h"

#include <SDL_events.h>
#include <SDL_timer.h>
#include <time.h>

#include "../entities/entity.h"
#include "../globals.h"
#include "../graphics/camera.h"
#include "../graphics/light.h"
#include "../graphics/screen.h"
#include "../log.h"
#include "../ui.h"
#include "../utils.h"
#include "./context.h"

#define TIME_FN_CALL(fn, var)                                     \
    {                                                             \
        clock_t start = clock();                                  \
        fn();                                                     \
        clock_t end = clock();                                    \
        var += ((end - start) / (float)CLOCKS_PER_SEC) * 1000.0f; \
    }

global_internal SDL_Event event;
global_internal bool is_running = false;
global_internal float g_dt;
global_internal int frame_counter = 0;
global_internal Uint32 previous_frame_time = 0;
global_internal Uint32 last_fps_time = 0;

global_internal void process_input() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                    break;
                }
            }
            default: {
                break;
            }
        }
        camera_process_input_event(&event, g_dt);
        ui_process_input_event(&event);
    }
}

global_internal void update_entities(linked_list_t *list, float dt) {
    linked_list_node_t *current = list->head;
    while (current != NULL) {
        entity_t *entity = current->data;
        if (entity->active) {
            entity->update_cb(entity, dt);
        }
        current = current->next;
    }
}

global_internal void render_entities(linked_list_t *list) {
    linked_list_node_t *current = list->head;
    while (current != NULL) {
        entity_t *entity = current->data;
        if (entity->active) {
            entity->render_cb(entity);
        }
        current = current->next;
    }
}

global_internal void process_update() {
    local_persist vec3f_t up_direction = {0.0f, 1.0f, 0.0f};
    // wait until target frame time is reached
    int time_to_wait = FRAME_TARGET - (SDL_GetTicks() - previous_frame_time);
    // only delay if too fast
    if (time_to_wait > 0 && time_to_wait < FRAME_TARGET) {
        SDL_Delay(time_to_wait);
    }
    // calculate delta time
    g_dt = ((float)SDL_GetTicks() - (float)previous_frame_time) / 1000.0f;
    // clamp value (if running in debugger dt will be messed up)
    g_dt = (is_greaterf(g_dt, 0.05f)) ? 0.05f : g_dt;
    // update previous frame time
    previous_frame_time = SDL_GetTicks();
    // create camera view matrix
    perspective.view = mat4f_look_at(camera.position,
                                     camera_get_look_at_target(), up_direction);
    // update entities
    update_entities(&entities, g_dt);
}

global_internal void process_render() {
    render_entities(&entities);
    screen_clear_renderer();
    screen_swap_buffers();
    screen_clear_back_buffer();
    screen_clear_z_buffer();
    ui_process_render();
    screen_render_present();
}

void manager_init(void) {
    vec2i_t screen_size = screen_get_size();
    context_init(screen_size.x, screen_size.y, 60, 1.0f, 60.0f);
    camera_init(vec3f_zero(), vec3f_new(0.0f, 0.0f, 1.0f));
    light_init(vec3f_new(0.0f, 0.0f, 1.0f));
    if (!ui_init()) {
        log_error("failed to initialize UI");
    }
}

void manager_start_loop(void) {
    local_persist float update_perf_time = 0.0f;
    local_persist float render_perf_time = 0.0f;
    is_running = true;
    while (is_running) {
        process_input();
        TIME_FN_CALL(process_update, update_perf_time)
        TIME_FN_CALL(process_render, render_perf_time)
        frame_counter++;
        Uint32 current_time = SDL_GetTicks();
        Uint32 elapsed_time = current_time - last_fps_time;
        // update timers every second
        if (elapsed_time >= 1000) {
            // update values in timing struct
            loop_timing.update = update_perf_time / (float)frame_counter;
            loop_timing.render = render_perf_time / (float)frame_counter;
            loop_timing.fps =
                (float)frame_counter / ((float)elapsed_time / 1000.0f);
            // update last fps time
            last_fps_time = current_time;
            // reset counters
            update_perf_time = 0.0f;
            render_perf_time = 0.0f;
            frame_counter = 0;
        }
    }
}

void manager_destroy(void) {
    ui_destroy();
    context_destroy();
}
