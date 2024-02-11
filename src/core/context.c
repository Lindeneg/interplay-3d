#include "context.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../entities/entity.h"
#include "../graphics/clipping.h"
#include "../log.h"
#include "../utils.h"
#include "./array.h"
#include "./mem.h"

config_t config = {
    .assets_folder = '\0', .ttf_file = '\0', .width = 0, .height = 0};
loop_timing_t loop_timing = {.update = 0, .render = 0, .fps = 0};
perspective_t perspective = {.aspect_x = 0.0f,
                             .aspect_y = 0.0f,
                             .fov = 0.0f,
                             .projection = {{0}},
                             .view = {{0}}};
linked_list_t entities;
linked_list_t meshes;
linked_list_t textures;

void context_init(int width, int height, float fov_in_degrees, float z_near,
                  float z_far) {
    entities = linked_list_new();
    meshes = linked_list_new();
    textures = linked_list_new();
    context_resize(width, height, fov_in_degrees, z_near, z_far);
}

void context_resize(int width, int height, float fov_in_degrees, float z_near,
                    float z_far) {
    perspective.aspect_x = (float)width / (float)height;
    perspective.aspect_y = (float)height / (float)width;
    perspective.fov = to_radians(fov_in_degrees);
    perspective.z_near = z_near;
    perspective.z_far = z_far;
    float fov_x =
        atanf(tanf(perspective.fov / 2.0f) * perspective.aspect_x) * 2.0f;
    perspective.projection =
        mat4f_make_perspective(perspective.fov, perspective.aspect_y,
                               perspective.z_near, perspective.z_far);
    init_frustum_planes(fov_x, perspective.fov, perspective.z_near,
                        perspective.z_far);
}

void context_destroy(void) {
    linked_list_node_t *current = entities.head;
    while (current != NULL) {
        entity_t *entity = current->data;
        entity->destroy_cb(entity);
        current = current->next;
    }
    current = meshes.head;
    while (current != NULL) {
        mesh_data_t *mesh_data = current->data;
        array_free(mesh_data->vertices);
        array_free(mesh_data->faces);
        free_mem(mesh_data->name);
        free_mem(mesh_data);
        current = current->next;
    }
    current = textures.head;
    while (current != NULL) {
        texture_t *texture = current->data;
        SDL_FreeSurface(texture->data);
        free_mem(texture->name);
        free_mem(texture);
        current = current->next;
    }
    linked_list_clear(&entities);
    linked_list_clear(&meshes);
    linked_list_clear(&textures);
}

bool context_parse_config_file(char cfg_file[PATH_LENGTH]) {
    FILE *file;
    fopen_s(&file, cfg_file, "r");
    if (!file) {
        log_error("failed to open config %s", cfg_file);
        return false;
    }
    char line[512];
    while (fgets(line, 512, file)) {
        if (strncmp(&line[0], "#", 1) == 0) continue;
        if (strncmp(line, "log-level=", 1) == 0) {
            int log_level;
            sscanf_s(line, "log-level=%d", &log_level);
            log_set_level(log_level);
        } else if (strncmp(line, "assets_folder=", 1) == 0) {
            sscanf_s(line, "assets_folder=%s", config.assets_folder,
                     PATH_LENGTH);
        } else if (strncmp(line, "ttf_file=", 1) == 0) {
            sscanf_s(line, "ttf_file=%s", config.ttf_file, PATH_LENGTH);
        } else if (strncmp(line, "width=", 1) == 0) {
            sscanf_s(line, "width=%d", &config.width);
        } else if (strncmp(line, "height=", 1) == 0) {
            sscanf_s(line, "height=%d", &config.height);
        }
    }
    fclose(file);
    if (config.assets_folder[0] == '\0') {
        log_error("failed to find required assets folder path");
        return false;
    }
    if (config.ttf_file[0] == '\0') {
        log_error("failed to find required ttf file path");
        return false;
    }
    log_debug("found log_level %d", log_get_level());
    log_debug("found assets_folder %s", config.assets_folder);
    log_debug("found ttf_file %s", config.ttf_file);
    log_debug("found (width,height) (%d,%d)", config.width, config.height);
    return true;
}

