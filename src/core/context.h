#ifndef INTERPLAY_CORE_CONTEXT_H_
#define INTERPLAY_CORE_CONTEXT_H_

#include <stdbool.h>

#include "../core/linked-list.h"
#include "../graphics/triangle.h"
#include "../math/mat4.h"

typedef struct loop_timing {
    float update, render, fps;
} loop_timing_t;

typedef struct perspective {
    float aspect_x, aspect_y, fov;
    float z_near, z_far;
    mat4f_t projection;
    mat4f_t view;
} perspective_t;

typedef struct config {
    char assets_folder[128];
    char ttf_file[128];
} config_t;

typedef struct mesh_data {
    vec3f_t *vertices;
    triangle_face_t *faces;
    char *name;
    int max_triangle_count;
} mesh_data_t;

//-global-variable
extern config_t config;
//-global-variable
extern loop_timing_t loop_timing;
//-global-variable
extern perspective_t perspective;
//-global-variable
extern linked_list_t entities;
//-global-variable
extern linked_list_t meshes;
//-global-variable
extern linked_list_t textures;

void context_init(int width, int height, float fov_in_degrees, float z_near,
                  float z_far);

void context_resize(int width, int height, float fov_in_degrees, float z_near,
                    float z_far);

void context_destroy(void);

bool context_parse_config_file(void);

#endif  // INTERPLAY_CORE_CONTEXT_H_
