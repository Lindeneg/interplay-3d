#if defined(_WIN32) || defined(WIN32)
#define _CRT_SECURE_NO_WARNINGS
#define SDL_MAIN_HANDLED
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./core/context.h"
#include "./core/manager.h"
#include "./graphics/screen.h"

int main(int argc, char *argv[]) {
    char config_file[PATH_LENGTH] = "./config.interplay";
    if (argc > 1 && strncmp(argv[1], "config=", 1) == 0) {
        sscanf(argv[1], "config=%s", config_file);
    }
    // parse config
    if (!context_parse_config_file(config_file)) {
        return EXIT_FAILURE;
    }
    // init SDL core
    if (!screen_init_sdl()) {
        return EXIT_FAILURE;
    }
    // set screen buffer size
    // TODO allow to resize (without restart) via UI
    if (!screen_set_size((vec2i_t){config.width, config.height})) {
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

