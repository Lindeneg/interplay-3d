#include "mesh.h"

#include <stdio.h>
#include <string.h>

#include "../core/array.h"
#include "../core/mem.h"
#include "../graphics/clipping.h"
#include "../graphics/light.h"
#include "../graphics/renderer.h"
#include "../graphics/screen.h"
#include "../log.h"
#include "../utils.h"

#define MAX_MESH_TRIANGLES (2 << 13)
#define MIN_MESH_TRIANGLES (2 << 5)
#define OBJ_LINE_SIZE (2 << 10)

global_internal mat4f_t world_matrix;

// process graphics pipeline for mesh triangles
global_internal void update_mesh(entity_t *entity, float dt) {
    vec2i_t window_size = screen_get_size();
    float screen_width = (float)window_size.x;
    float screen_height = (float)window_size.y;
    mesh_t *mesh = (mesh_t *)entity->data;
    // reset triangle count
    array_clear(mesh->triangles);
    if (mesh->cfg.do_rotation) {
        entity->rotation.x += 0.6f * dt;
        entity->rotation.y += 0.6f * dt;
        entity->rotation.z += 0.6f * dt;
    }
    // create scale, rotation, and translation matrices
    mat4f_t scale_matrix =
        mat4f_make_scale(entity->scale.x, entity->scale.y, entity->scale.z);
    mat4f_t translation_matrix = mat4f_make_translation(
        entity->translation.x, entity->translation.y, entity->translation.z);
    mat4f_t rotation_matrix_x = mat4f_make_rotation_x(entity->rotation.x);
    mat4f_t rotation_matrix_y = mat4f_make_rotation_y(entity->rotation.y);
    mat4f_t rotation_matrix_z = mat4f_make_rotation_z(entity->rotation.z);
    // loop all mesh triangle faces
    int num_faces = array_size(mesh->m->faces);
    for (int face_index = 0; face_index < num_faces; face_index++) {
        triangle_face_t mesh_face = mesh->m->faces[face_index];
        vec3f_t face_vertices[3];
        // assign face vertices
        face_vertices[0] = mesh->m->vertices[mesh_face.a];
        face_vertices[1] = mesh->m->vertices[mesh_face.b];
        face_vertices[2] = mesh->m->vertices[mesh_face.c];
        // array to hold vertices after transformation
        vec4f_t transformed_vertices[3];
        // loop all three vertices of this current face
        // to apply transformation updates
        for (int v = 0; v < 3; v++) {
            vec4f_t transformed_vertex = vec4f_from_vec3f(face_vertices[v]);
            // create a world matrix combining scale, rotation, and translation
            world_matrix = mat4f_identity();
            // order matters: 1)scale, 2)rotate, 3)translate:
            // [T]*[R]*[S]*v
            world_matrix = mat4f_mul_mat4f(scale_matrix, world_matrix);
            world_matrix = mat4f_mul_mat4f(rotation_matrix_z, world_matrix);
            world_matrix = mat4f_mul_mat4f(rotation_matrix_y, world_matrix);
            world_matrix = mat4f_mul_mat4f(rotation_matrix_x, world_matrix);
            world_matrix = mat4f_mul_mat4f(translation_matrix, world_matrix);
            // multiply the world matrix by original vector
            transformed_vertex =
                mat4f_mul_vec4f(world_matrix, transformed_vertex);
            // multiply view matrix by vector to transform to camera space
            transformed_vertex =
                mat4f_mul_vec4f(perspective.view, transformed_vertex);
            // save transformed vertex
            transformed_vertices[v] = transformed_vertex;
        }
        // calculate triangle face normal
        vec3f_t face_normal = get_triangle_normal(transformed_vertices);
        // check to see if we should do backface culling
        if (mesh->cfg.do_backface_culling) {
            // find vector between vertex A in triangle and camera origin
            vec3f_t camera_ray = vec3f_subtract_new(
                vec3f_zero(), vec3f_from_vec4f(transformed_vertices[0]));
            // calculate how aligned camera ray is with face normal (using dot
            // product)
            float dot_normal_camera = vec3f_dot(face_normal, camera_ray);
            // bypass triangles that are looking away from the camera
            if (is_lesserf(dot_normal_camera, 0)) {
                continue;
            }
        }
        // create polygon from original transformed triangle
        polygon_t polygon = polygon_from_triangle(
            vec3f_from_vec4f(transformed_vertices[0]),
            vec3f_from_vec4f(transformed_vertices[1]),
            vec3f_from_vec4f(transformed_vertices[2]), mesh_face.a_uv,
            mesh_face.b_uv, mesh_face.c_uv);
        clip_polygon(&polygon);
        // break clipped polygon apart back into list of triangles
        triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int num_triangles_after_clipping = 0;
        triangles_from_polygon(&polygon, triangles_after_clipping,
                               &num_triangles_after_clipping);
        // loop all assembled triangles after clipping
        for (int triangle_index = 0;
             triangle_index < num_triangles_after_clipping; triangle_index++) {
            triangle_t triangle_after_clipping =
                triangles_after_clipping[triangle_index];
            vec4f_t projected_points[3];
            // loop all three vertices to perform projection and conversion to
            // screen space
            for (int v = 0; v < 3; v++) {
                // project current vertex using a perspective projection matrix
                projected_points[v] = mat4f_mul_vec4f(
                    perspective.projection, triangle_after_clipping.points[v]);
                // perform perspective divide
                if (!is_equalf(projected_points[v].w, 0)) {
                    projected_points[v].x /= projected_points[v].w;
                    projected_points[v].y /= projected_points[v].w;
                    projected_points[v].z /= projected_points[v].w;
                }
                // flip vertically since the y values of the 3D mesh grow
                // bottom->up and in screen space y values grow top->down
                projected_points[v].y *= -1;
                // scale into view
                projected_points[v].x *= (screen_width / 2.0f);
                projected_points[v].y *= (screen_height / 2.0f);
                // translate projected points to middle of the screen
                projected_points[v].x += (screen_width / 2.0f);
                projected_points[v].y += (screen_height / 2.0f);
            }
            color_t triangle_color = mesh_face.color;
            if (mesh->cfg.show_light) {
                // calculate shade intensity based on how aligned the
                // normal is with flipped light direction ray
                float light_intensity_factor =
                    -vec3f_dot(face_normal, light.direction);
                // calculate triangle color based on light angle
                triangle_color = light_apply_intensity(mesh_face.color,
                                                       light_intensity_factor);
            }
            // create final projected triangle that will be rendered in screen
            // space
            triangle_t triangle_to_render = {
                .points = {{projected_points[0].x, projected_points[0].y,
                            projected_points[0].z, projected_points[0].w},
                           {projected_points[1].x, projected_points[1].y,
                            projected_points[1].z, projected_points[1].w},
                           {projected_points[2].x, projected_points[2].y,
                            projected_points[2].z, projected_points[2].w}},
                .tex_coords = {{triangle_after_clipping.tex_coords[0].u,
                                triangle_after_clipping.tex_coords[0].v},
                               {triangle_after_clipping.tex_coords[1].u,
                                triangle_after_clipping.tex_coords[1].v},
                               {triangle_after_clipping.tex_coords[2].u,
                                triangle_after_clipping.tex_coords[2].v}},
                .color = triangle_color,
                .texture = mesh->texture};
            if (array_size(mesh->triangles) < mesh->m->max_triangle_count) {
                array_push(mesh->triangles, triangle_to_render);
            }
        }
    }
}

global_internal void render_mesh(const entity_t *entity) {
    mesh_t *mesh = (mesh_t *)entity->data;
    size_t triangle_count = array_size(mesh->triangles);
    for (int i = 0; i < triangle_count; i++) {
        triangle_t triangle = mesh->triangles[i];
        if (mesh->cfg.show_filled) {
            filled_triangle_point_t vertex_a = {
                (int)triangle.points[0].x, (int)triangle.points[0].y,
                triangle.points[0].z, triangle.points[0].w};
            filled_triangle_point_t vertex_b = {
                (int)triangle.points[1].x, (int)triangle.points[1].y,
                triangle.points[1].z, triangle.points[1].w};
            filled_triangle_point_t vertex_c = {
                (int)triangle.points[2].x, (int)triangle.points[2].y,
                triangle.points[2].z, triangle.points[2].w};
            draw_filled_triangle(&vertex_a, &vertex_b, &vertex_c,
                                 triangle.color);
        }
        if (mesh->cfg.show_texture) {
            // TODO support texture shading
            textured_triangle_point_t vertex_a = {
                (int)triangle.points[0].x, (int)triangle.points[0].y,
                triangle.points[0].z,      triangle.points[0].w,
                triangle.tex_coords[0].u,  triangle.tex_coords[0].v};
            textured_triangle_point_t vertex_b = {
                (int)triangle.points[1].x, (int)triangle.points[1].y,
                triangle.points[1].z,      triangle.points[1].w,
                triangle.tex_coords[1].u,  triangle.tex_coords[1].v};
            textured_triangle_point_t vertex_c = {
                (int)triangle.points[2].x, (int)triangle.points[2].y,
                triangle.points[2].z,      triangle.points[2].w,
                triangle.tex_coords[2].u,  triangle.tex_coords[2].v};
            draw_textured_triangle(&vertex_a, &vertex_b, &vertex_c,
                                   triangle.texture);
        }
        if (mesh->cfg.show_wireframe) {
            draw_triangle(
                vec2i_new((int)triangle.points[0].x, (int)triangle.points[0].y),
                vec2i_new((int)triangle.points[1].x, (int)triangle.points[1].y),
                vec2i_new((int)triangle.points[2].x, (int)triangle.points[2].y),
                0xFFFFFFFF);
        }
        if (mesh->cfg.show_vertices) {
            vec2i_t size = {3, 3};
            draw_filled_rect(
                vec2i_new((int)triangle.points[0].x, (int)triangle.points[0].y),
                size, 0xFFFF0000);
            draw_filled_rect(
                vec2i_new((int)triangle.points[1].x, (int)triangle.points[1].y),
                size, 0xFFFF0000);
            draw_filled_rect(
                vec2i_new((int)triangle.points[2].x, (int)triangle.points[2].y),
                size, 0xFFFF0000);
        }
    }
}

global_internal void destroy_mesh(entity_t *entity) {
    mesh_t *mesh = (mesh_t *)entity->data;
    array_free(mesh->triangles);
    free_mem(mesh);
    free_mem(entity);
}

global_internal mesh_data_t *load_obj_file(const char *obj_file, char *name) {
    FILE *file;
    char path[256];
    sprintf(path, "%s/%s", config.assets_folder, obj_file);
    fopen_s(&file, path, "r");
    if (!file) {
        log_error("failed to open file %s", obj_file);
        return NULL;
    }
    mesh_data_t *mesh_data = (mesh_data_t *)reserve_mem(sizeof(mesh_data_t));
    mesh_data->name = name;
    mesh_data->vertices = NULL;
    mesh_data->faces = NULL;
    char line[OBJ_LINE_SIZE];
    texture_uv_t *texture_coords = NULL;
    while (fgets(line, OBJ_LINE_SIZE, file)) {
        // vertex information
        if (strncmp(line, "v ", 2) == 0) {
            vec3f_t vertex;
            sscanf_s(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh_data->vertices, vertex)
        }
        if (strncmp(line, "vt ", 3) == 0) {
            texture_uv_t texcoord;
            sscanf_s(line, "vt %f %f", &texcoord.u, &texcoord.v);
            array_push(texture_coords, texcoord)
        }
        // face information
        if (texture_coords && strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &vertex_indices[0],
                     &texture_indices[0], &normal_indices[0],
                     &vertex_indices[1], &texture_indices[1],
                     &normal_indices[1], &vertex_indices[2],
                     &texture_indices[2], &normal_indices[2]);
            triangle_face_t face = {
                .a = vertex_indices[0] - 1,
                .b = vertex_indices[1] - 1,
                .c = vertex_indices[2] - 1,
                .a_uv = texture_coords[texture_indices[0] - 1],
                .b_uv = texture_coords[texture_indices[1] - 1],
                .c_uv = texture_coords[texture_indices[2] - 1],
                .color = 0xFFFFFFFF};
            array_push(mesh_data->faces, face)
        }
    }
    array_free(texture_coords);
    fclose(file);
    mesh_data->max_triangle_count =
        min(max(MIN_MESH_TRIANGLES,
                array_size(mesh_data->faces) + MAX_NUM_POLY_TRIANGLES),
            MAX_MESH_TRIANGLES);
    linked_list_append(&meshes, mesh_data);
    return mesh_data;
}

entity_t *mesh_create_entity(const char *obj_file, const char *png_file) {
    local_persist const char *supported_extension = "obj";
    char *mesh_name = extract_filename(obj_file, supported_extension);
    if (!mesh_name) {
        log_error("failed to extract filename from %s", obj_file);
        return NULL;
    }
    mesh_data_t *prev_loaded_mesh_data = NULL;
    linked_list_search_name(mesh_data_t, prev_loaded_mesh_data, meshes,
                            mesh_name);
    if (!prev_loaded_mesh_data &&
        !can_load_file(config.assets_folder, obj_file)) {
        log_error("failed to load file %s", obj_file);
        return NULL;
    }
    entity_t *entity = reserve_mem(sizeof(entity_t));
    mesh_t *mesh = reserve_mem(sizeof(mesh_t));
    mesh->cfg.do_backface_culling = true;
    mesh->cfg.show_light = false;
    mesh->cfg.do_rotation = false;
    mesh->cfg.show_filled = false;
    mesh->cfg.show_texture = false;
    mesh->cfg.show_vertices = false;
    mesh->cfg.show_wireframe = true;
    mesh->texture = NULL;
    mesh->triangles = NULL;
    if (prev_loaded_mesh_data) {
        log_debug("using previously loaded mesh values for %s", mesh_name);
        mesh->m = prev_loaded_mesh_data;
    } else {
        mesh_data_t *mesh_data = load_obj_file(obj_file, mesh_name);
        if (!mesh_data) {
            log_error("failed to parse file %s", obj_file);
            free_mem(mesh_name);
            free_mem(mesh);
            free_mem(entity);
            return NULL;
        }
        mesh->m = mesh_data;
    }
    if (strlen(png_file) > 0) {
        mesh->texture = texture_load(png_file);
    }
    entity->type = ENTITY_MESH;
    entity->active = false;
    entity->translation = vec3f_new(0, 0, 15);
    entity->rotation = vec3f_new(0, 0, 0);
    entity->scale = vec3f_new(1, 1, 1);
    entity->update_cb = update_mesh;
    entity->render_cb = render_mesh;
    entity->destroy_cb = destroy_mesh;
    entity->data = mesh;
    entity->id = linked_list_append(&entities, entity);
    return entity;
}

void mesh_destroy_entity(entity_t *entity) { destroy_mesh(entity); }

