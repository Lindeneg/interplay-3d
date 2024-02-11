#ifndef INTERPLAY_ENTITIES_ENTITY_H_
#define INTERPLAY_ENTITIES_ENTITY_H_

#include <stdbool.h>

#include "../math/vec.h"

typedef enum { ENTITY_MESH } EntityType;

typedef struct entity {
    int id;
    EntityType type;
    bool active;
    void *data;

    // TODO:
    // remove fn pointers and use _Generic macro instead
    // that calls correct EntityType update/render/destroy
    // will save 8x3 bytes per entity
    void (*update_cb)(struct entity *entity, float dt);

    void (*render_cb)(const struct entity *entity);

    void (*destroy_cb)(struct entity *entity);

    vec3f_t translation, rotation, scale;
} entity_t;

#endif  // INTERPLAY_ENTITIES_ENTITY_H_
