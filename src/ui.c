#include "ui.h"

#include <microui.h>
#include <stdio.h>
#include <string.h>

#include "./core/array.h"
#include "./core/context.h"
#include "./core/mem.h"
#include "./core/text.h"
#include "./entities/entity.h"
#include "./entities/mesh.h"
#include "./graphics/camera.h"
#include "./graphics/light.h"
#include "./graphics/renderer.h"
#include "./graphics/screen.h"
#include "./log.h"
#include "./utils.h"

#define CHECKBOX(name, var, target)     \
    if (mu_checkbox(ctx, name, &var)) { \
        target = (bool)var;             \
    }

#define D_CHECKBOX(n0, v0, t0, n1, v1, t1)       \
    mu_layout_row(ctx, 2, (int[]){125, 125}, 0); \
    CHECKBOX(n0, v0, t0)                         \
    CHECKBOX(n1, v1, t1)

//-global-variable
mu_Context *ctx = NULL;

global_internal float rotation_range[2];
global_internal float translation_range[2];
global_internal float scaling_range[2];

global_internal char mesh_buf[128];
global_internal char texture_buf[128];
global_internal int selected_entity_id = -1;
global_internal bool camera_selected = false;
global_internal bool light_selected = false;
global_internal text_texture_t text_texture;

// https://github.com/rxi/microui/blob/master/demo/main.c#L207-L211
global_internal const char button_map[256] = {
    [SDL_BUTTON_LEFT & 0xff] = MU_MOUSE_LEFT,
    [SDL_BUTTON_RIGHT & 0xff] = MU_MOUSE_RIGHT,
    [SDL_BUTTON_MIDDLE & 0xff] = MU_MOUSE_MIDDLE,
};
// https://github.com/rxi/microui/blob/master/demo/main.c#L213-L222
global_internal const char key_map[256] = {
    [SDLK_LSHIFT & 0xff] = MU_KEY_SHIFT,
    [SDLK_RSHIFT & 0xff] = MU_KEY_SHIFT,
    [SDLK_LCTRL & 0xff] = MU_KEY_CTRL,
    [SDLK_RCTRL & 0xff] = MU_KEY_CTRL,
    [SDLK_LALT & 0xff] = MU_KEY_ALT,
    [SDLK_RALT & 0xff] = MU_KEY_ALT,
    [SDLK_RETURN & 0xff] = MU_KEY_RETURN,
    [SDLK_BACKSPACE & 0xff] = MU_KEY_BACKSPACE,
};

global_internal int get_text_width(mu_Font font, const char *text, int len) {
    if (len > 0) return len;
    if (strcmp(text, "") == 0) return 0;
    text_update_texture(&text_texture, text, 0xFF000000);
    int width = text_texture.width;
    text_destroy_texture(&text_texture);
    return width;
}

global_internal int get_text_height(mu_Font font) { return 18; }

global_internal uint32_t convert_mu_color_to_uint32(mu_Color color) {
    return sdl_color_to_uint32(mu_color_to_sdl_color(color));
}

global_internal int float_slider(float *value, float low, float high) {
    local_persist float tmp;
    mu_push_id(ctx, &value, sizeof(value));
    tmp = *value;
    int res = mu_slider_ex(ctx, &tmp, low, high, 0, "%.3f", MU_OPT_ALIGNCENTER);
    *value = tmp;
    mu_pop_id(ctx);
    return res;
}

global_internal void slider_row(const char *text, float *ptr, float low,
                                float high) {
    mu_layout_row(ctx, 2, (int[]){100, -1}, 0);
    mu_label(ctx, text);
    float_slider(ptr, low, high);
}

global_internal void build_camera_control_ui() {
    vec2i_t screen_size = screen_get_size();
    vec2f_t screen_ratio = screen_get_size_ratio();
    if (mu_begin_window_ex(
            ctx, "camera_t Position",
            mu_rect(0, (screen_size.y / screen_ratio.y) - 150, 250, 150),
            MU_OPT_NOCLOSE | MU_OPT_NORESIZE)) {
        mu_layout_row(ctx, 3, (int[]){77, 77, 77}, 0);
        mu_label(ctx, "X");
        mu_label(ctx, "Y");
        mu_label(ctx, "Z");
        mu_layout_row(ctx, 3, (int[]){77, 77, 77}, 0);
        float_slider(&camera.position.x, translation_range[0],
                     translation_range[1]);
        float_slider(&camera.position.y, translation_range[0],
                     translation_range[1]);
        float_slider(&camera.position.z, translation_range[0],
                     translation_range[1]);
        mu_layout_row(ctx, 2, (int[]){118, 118}, 0);
        mu_label(ctx, "YAW");
        mu_label(ctx, "PITCH");
        mu_layout_row(ctx, 2, (int[]){118, 118}, 0);
        float_slider(&camera.yaw, rotation_range[0], rotation_range[1]);
        float_slider(&camera.pitch, rotation_range[0], rotation_range[1]);
        mu_end_window(ctx);
    }
}

global_internal void build_light_control_ui() {
    vec2i_t screen_size = screen_get_size();
    vec2f_t screen_ratio = screen_get_size_ratio();
    if (mu_begin_window_ex(
            ctx, "light_t Direction",
            mu_rect(0, (screen_size.y / screen_ratio.y) - (150 + 120), 250,
                    120),
            MU_OPT_NOCLOSE | MU_OPT_NORESIZE)) {
        mu_layout_row(ctx, 3, (int[]){77, 77, 77}, 0);
        mu_label(ctx, "X");
        mu_label(ctx, "Y");
        mu_label(ctx, "Z");
        mu_layout_row(ctx, 3, (int[]){77, 77, 77}, 0);
        float_slider(&light.direction.x, translation_range[0],
                     translation_range[1]);
        float_slider(&light.direction.y, translation_range[0],
                     translation_range[1]);
        float_slider(&light.direction.z, translation_range[0],
                     translation_range[1]);
        mu_end_window(ctx);
    }
}

global_internal void build_entity_control_ui() {
    local_persist int cfg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    vec2i_t screen_size = screen_get_size();
    vec2f_t screen_ratio = screen_get_size_ratio();
    char buf[256];
    if (mu_begin_window_ex(
            ctx, "Entity",
            mu_rect((int)(screen_size.x / screen_ratio.x) - 280, 0, 280, 500),
            MU_OPT_NOCLOSE | MU_OPT_NORESIZE)) {
        entity_t *entity =
            (entity_t *)linked_list_get(&entities, selected_entity_id)->data;
        mesh_t *mesh = (mesh_t *)entity->data;
        // mesh and texture names
        mu_layout_row(ctx, 2, (int[]){125, 125}, 0);
        sprintf(buf, "Mesh: %s.obj", mesh->m->name);
        mu_text(ctx, buf);
        if (mesh->texture) {
            sprintf(buf, "Texture: %s.png", mesh->texture->name);
        } else {
            sprintf(buf, "Texture: None");
        }
        mu_text(ctx, buf);
        // id and triangle count
        mu_layout_row(ctx, 2, (int[]){125, 125}, 0);
        sprintf(buf, "ID: %d", entity->id);
        mu_text(ctx, buf);
        sprintf(buf, "Triangles: %d", array_size(mesh->triangles));
        mu_text(ctx, buf);
        // vertices and faces count
        mu_layout_row(ctx, 2, (int[]){125, 125}, 0);
        sprintf(buf, "Vertices: %d", array_size(mesh->m->vertices));
        mu_text(ctx, buf);
        sprintf(buf, "Faces: %d", array_size(mesh->m->faces));
        mu_text(ctx, buf);
        /* CONFIG SECTION */
        cfg[0] = (int)entity->active;
        cfg[1] = (int)mesh->cfg.show_light;
        cfg[2] = (int)mesh->cfg.do_backface_culling;
        cfg[3] = (int)mesh->cfg.do_rotation;
        cfg[4] = (int)mesh->cfg.show_texture;
        cfg[5] = (int)mesh->cfg.show_wireframe;
        cfg[6] = (int)mesh->cfg.show_vertices;
        cfg[7] = (int)mesh->cfg.show_filled;
        // config on/off states
        D_CHECKBOX("ACTIVE", cfg[0], entity->active, "SHADING", cfg[1],
                   mesh->cfg.show_light)
        D_CHECKBOX("CULLING", cfg[2], mesh->cfg.do_backface_culling, "ROTATE",
                   cfg[3], mesh->cfg.do_rotation)
        D_CHECKBOX("TEXTURE", cfg[4], mesh->cfg.show_texture, "WIRED", cfg[5],
                   mesh->cfg.show_wireframe)
        D_CHECKBOX("VERTICES", cfg[6], mesh->cfg.show_vertices, "FILLED",
                   cfg[7], mesh->cfg.show_filled)
        /* TRANSFORM SECTION */
        mu_label(ctx, "TRANSLATION");
        slider_row("X", &entity->translation.x, translation_range[0],
                   translation_range[1]);
        slider_row("Y", &entity->translation.y, translation_range[0],
                   translation_range[1]);
        slider_row("Z", &entity->translation.z, translation_range[0],
                   translation_range[1]);
        mu_label(ctx, "ROTATION");
        if (mesh->cfg.do_rotation) {
            if (is_greater_or_equalf(entity->rotation.x, rotation_range[1])) {
                entity->rotation.x = rotation_range[0];
            }
            if (is_greater_or_equalf(entity->rotation.y, rotation_range[1])) {
                entity->rotation.y = rotation_range[0];
            }
            if (is_greater_or_equalf(entity->rotation.z, rotation_range[1])) {
                entity->rotation.z = rotation_range[0];
            }
        }
        slider_row("X", &entity->rotation.x, rotation_range[0],
                   rotation_range[1]);
        slider_row("Y", &entity->rotation.y, rotation_range[0],
                   rotation_range[1]);
        slider_row("Z", &entity->rotation.z, rotation_range[0],
                   rotation_range[1]);
        mu_label(ctx, "SCALE");
        slider_row("X", &entity->scale.x, scaling_range[0], scaling_range[1]);
        slider_row("Y", &entity->scale.y, scaling_range[0], scaling_range[1]);
        slider_row("Z", &entity->scale.z, scaling_range[0], scaling_range[1]);
        mu_end_window(ctx);
    }
}

global_internal void build_stat_ui() {
    char buf[128];
    sprintf(buf, "Interplay Stats (%dx%d)", config.width, config.height);
    if (mu_begin_window_ex(ctx, buf, mu_rect(0, 0, 200, 120),
                           MU_OPT_NOCLOSE | MU_OPT_NORESIZE)) {
        sprintf(buf, "Update: %.3fms", loop_timing.update);
        mu_text(ctx, buf);
        sprintf(buf, "Render: %.3fms", loop_timing.render);
        mu_text(ctx, buf);
        sprintf(buf, "FPS: %.3f", loop_timing.fps);
        mu_text(ctx, buf);
        mu_end_window(ctx);
    }
}

global_internal void build_import_ui() {
    if (mu_begin_window_ex(ctx, "Import", mu_rect(0, 120, 200, 180),
                           MU_OPT_NOCLOSE | MU_OPT_NORESIZE)) {
        char buf[128];
        sprintf(buf, "Relative to %s", config.assets_folder);
        mu_text(ctx, buf);
        mu_layout_row(ctx, 1, (int[]){-1}, 0);
        mu_label(ctx, "Mesh File (.obj)");
        int mesh_submitted = 0;
        int texture_submitted = 0;
        mu_layout_row(ctx, 1, (int[]){-1}, 0);
        if (mu_textbox(ctx, mesh_buf, sizeof(mesh_buf)) & MU_RES_SUBMIT) {
            mu_set_focus(ctx, ctx->last_id);
            mesh_submitted = 1;
        }
        mu_layout_row(ctx, 1, (int[]){-1}, 0);
        mu_label(ctx, "Texture File (.png)");
        mu_layout_row(ctx, 1, (int[]){-1}, 0);
        if (mu_textbox(ctx, texture_buf, sizeof(texture_buf)) & MU_RES_SUBMIT) {
            mu_set_focus(ctx, ctx->last_id);
            texture_submitted = 1;
        }
        mu_layout_row(ctx, 1, (int[]){-1}, 0);
        if (mu_button(ctx, "Submit")) {
            mesh_submitted = 1;
            texture_submitted = 1;
        }
        if (mesh_submitted && texture_submitted) {
            if (!mesh_create_entity(mesh_buf, texture_buf)) {
                // TODO probably show an error popup or something
            }
            mesh_buf[0] = '\0';
            texture_buf[0] = '\0';
        }
        mu_end_window(ctx);
    }
}

global_internal void build_control_ui() {
    if (mu_begin_window_ex(ctx, "Control", mu_rect(0, 300, 200, 250),
                           MU_OPT_NOCLOSE | MU_OPT_NORESIZE)) {
        char buf[256];
        mu_text(ctx, "Core Control");
        mu_layout_row(ctx, 2, (int[]){92, 92}, 0);
        const char *cam_text = camera_selected ? "camera*" : "camera";
        const char *light_text = light_selected ? "light*" : "light";
        if (mu_button(ctx, cam_text)) {
            camera_selected = !camera_selected;
        }
        if (mu_button(ctx, light_text)) {
            // TODO enable light control
            // light_selected = !light_selected;
        }
        mu_text(ctx, "Entity Control");
        linked_list_node_t *current = entities.head;
        int i = 0;
        while (current != NULL) {
            if (i % 2 == 0) {
                mu_layout_row(ctx, 2, (int[]){90, 90}, 0);
            }
            entity_t *mesh_entity = (entity_t *)current->data;
            if (mesh_entity->type == ENTITY_MESH) {
                mesh_t *mesh = (mesh_t *)mesh_entity->data;
                const char *btn_format = selected_entity_id == mesh_entity->id
                                             ? "(%d)%s*"
                                             : "(%d)%s";
                sprintf(buf, btn_format, mesh_entity->id, mesh->m->name);
                if (mu_button(ctx, buf)) {
                    if (selected_entity_id == mesh_entity->id) {
                        selected_entity_id = -1;
                    } else {
                        selected_entity_id = mesh_entity->id;
                    }
                }
            }
            i++;
            current = current->next;
        }
        mu_end_window(ctx);
    }
}

global_internal void draw_ui_rect(mu_Rect rect, mu_Color color) {
    vec2f_t screen_ratio = screen_get_size_ratio();
    float scaled_width = (float)rect.w * screen_ratio.x;
    float scaled_height = (float)rect.h * screen_ratio.y;
    int x = (int)(((rect.x * screen_ratio.x)) + (scaled_width / 2.0f));
    int y = (int)((rect.y * screen_ratio.y) + (scaled_height / 2.0f));
    draw_outlined_rect(vec2i_new(x, y),
                       vec2i_new((int)scaled_width, (int)scaled_height),
                       convert_mu_color_to_uint32(color));
}

global_internal void draw_ui_text(char str[1], mu_Vec2 pos, mu_Color color) {
    if (str[0] == '\0') return;
    text_texture.position.x = pos.x;
    text_texture.position.y = pos.y;
    text_update_texture(&text_texture, str, convert_mu_color_to_uint32(color));
    screen_put_text(&text_texture);
    text_destroy_texture(&text_texture);
}

bool ui_init(void) {
    if (ctx) {
        log_warning("cannot initialize already initialized UI");
        return false;
    }
    ctx = reserve_mem(sizeof(mu_Context));
    if (!ctx) return false;
    // init micro ui
    mu_init(ctx);
    // set required callbacks
    ctx->text_width = get_text_width;
    ctx->text_height = get_text_height;
    // create texture for text rendering
    text_texture = text_create_texture();
    // set transform constraints
    rotation_range[0] = -to_radians(360);
    rotation_range[1] = to_radians(360);
    translation_range[0] = -(perspective.z_far + 20);
    translation_range[1] = perspective.z_far + 20;
    scaling_range[0] = 0.1f;
    scaling_range[1] = 5.0f;
    return true;
}

void ui_process_input_event(SDL_Event *event) {
    switch (event->type) {
        case SDL_MOUSEMOTION:
            mu_input_mousemove(ctx, event->motion.x, event->motion.y);
            break;
        case SDL_MOUSEWHEEL:
            mu_input_scroll(ctx, 0, event->wheel.y * -30);
            break;
        case SDL_TEXTINPUT:
            mu_input_text(ctx, event->text.text);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            int b = button_map[event->button.button & 0xff];
            if (b && event->type == SDL_MOUSEBUTTONDOWN) {
                mu_input_mousedown(ctx, event->button.x, event->button.y, b);
            }
            if (b && event->type == SDL_MOUSEBUTTONUP) {
                mu_input_mouseup(ctx, event->button.x, event->button.y, b);
            }
            break;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            int c = key_map[event->key.keysym.sym & 0xff];
            if (c && event->type == SDL_KEYDOWN) {
                mu_input_keydown(ctx, c);
            }
            if (c && event->type == SDL_KEYUP) {
                mu_input_keyup(ctx, c);
            }
            break;
        }
    }
}

void ui_process_render(void) {
    // build UI
    mu_begin(ctx);
    build_stat_ui();
    build_import_ui();
    build_control_ui();
    if (camera_selected) {
        build_camera_control_ui();
    }
    if (light_selected) {
        build_light_control_ui();
    }
    if (selected_entity_id > -1) {
        build_entity_control_ui();
    }
    mu_end(ctx);
    // handle micro ui commands
    mu_Command *cmd = NULL;
    while (mu_next_command(ctx, &cmd)) {
        switch (cmd->type) {
            case MU_COMMAND_TEXT:
                draw_ui_text(cmd->text.str, cmd->text.pos, cmd->text.color);
                break;
            case MU_COMMAND_RECT:
                draw_ui_rect(cmd->rect.rect, cmd->rect.color);
                break;
            case MU_COMMAND_ICON:
                if (cmd->icon.id == MU_ICON_CHECK) {
                    // what are you doing man
                    draw_ui_text(
                        "X",
                        mu_vec2(cmd->icon.rect.x + 7, cmd->icon.rect.y + 2),
                        cmd->icon.color);
                }
                break;
        }
    }
}

void ui_destroy() {
    text_destroy_texture(&text_texture);
    free_mem(ctx);
    ctx = NULL;
}

