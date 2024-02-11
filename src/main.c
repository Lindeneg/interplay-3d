#include <stdlib.h>

#include "./core/context.h"
#include "./core/manager.h"
#include "./graphics/screen.h"

int main(int argc, char *argv[]) {
    // parse config
    if (!context_parse_config_file()) {
        return EXIT_FAILURE;
    }
    // init SDL core
    if (!screen_init_sdl()) {
        return EXIT_FAILURE;
    }
    // set screen buffer size
    // TODO allow to resize (without restart) via UI (and also via config)
    if (!screen_set_size((vec2i_t){800, 600})) {
        return EXIT_FAILURE;
    }
    // init TTF support
    if (!text_init_ttf(screen_get_renderer())) {
        return EXIT_FAILURE;
    }
    // init entity manager
    manager_init();
    // start main loop
    manager_start_loop();
    // clean up when done
    manager_destroy();
    text_destroy_ttf();
    screen_destroy();
    return EXIT_SUCCESS;
}

