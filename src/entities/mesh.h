#ifndef INTERPLAY_ENTITIES_MESH_H_
#define INTERPLAY_ENTITIES_MESH_H_

#include <stdbool.h>

#include "../core/context.h"
#include "../graphics/triangle.h"
#include "./entity.h"

typedef struct mesh_config {
    bool do_rotation;
    bool do_backface_culling;
    bool show_light;
    bool show_vertices;
    bool show_wireframe;
    bool show_filled;
    bool show_texture;
} mesh_config_t;

typedef struct mesh {
    triangle_t *triangles;
    const mesh_data_t *m;
    const texture_t *texture;
    mesh_config_t cfg;
} mesh_t;

entity_t *mesh_create_entity(const char *obj_file, const char *png_file);

void mesh_destroy_entity(entity_t *entity);

#endif  // INTERPLAY_ENTITIES_MESH_H_
