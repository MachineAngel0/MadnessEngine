#include "UI.h"

#include "vk_buffer.h"


UI_System* ui_system_init(renderer* renderer)
{
    UI_System* ui_state = arena_alloc(&renderer->arena, sizeof(UI_System));
    memset(ui_state, 0, (sizeof(UI_System)));

    //TEMPORARY
    ui_state->frame_arena = &renderer->frame_arena;

    ui_state->active.ID = -1;
    ui_state->active.layer = -1;
    ui_state->hot.ID = -1;
    ui_state->hot.layer = -1;


    ui_state->id_generation_number = -1;

    ui_state->mouse_pos_x = -1.0f;
    ui_state->mouse_pos_y = -1.0f;

    ui_state->mouse_down = 0;
    ui_state->mouse_released = 0;

    //TODO: replace with an in param
    ui_state->screen_size = (vec2){800.0f, 600.0f};


    // text_system_init(ui_state->text_system);

    u32 ui_buffer_sizes = MB(1);

    ui_state->ui_quad_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_VERTEX, ui_buffer_sizes);
    ui_state->ui_quad_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                 BUFFER_TYPE_INDEX, ui_buffer_sizes);
    ui_state->text_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_VERTEX,
                                                               ui_buffer_sizes);
    ui_state->text_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                              ui_buffer_sizes);

    return ui_state;
}

void ui_begin(UI_System* ui_state, i32 screen_size_x, i32 screen_size_y)
{
    //clear draw info and reset the hot id

    //std::cout << "MOUSE STATE:" << ui_state.mouse_down << '\n';

    ui_state->screen_size.x = screen_size_x;
    ui_state->screen_size.y = screen_size_y;

    //TODO: clear vertex and index data for UI and TEXT
    // clear_vertex_info(ui_state->draw_info.vertex_info);
    // ui_state->draw_info.UI_Objects.clear();

    ui_state->hot.ID = -1;
    ui_state->hot.layer = -1;


    //TODO: UPDATE TEXT
    // text_update(ui_state->text_system);
}

void ui_end(UI_System* ui_system)
{
    //check if mouse is released, if so reset the active id
    //also update the mouse state

    //printf("HOT ID: %d, HOT LAYER: %d\n", ui_state->hot.ID, ui_state->hot.layer);
    //printf("ACTIVE ID: %d, ACTIVE LAYER: %d\n", ui_state->active.ID, ui_state->active.layer);

    if (input_is_mouse_button_released(MOUSE_BUTTON_LEFT))
    {
        //std::cout << "mouse released\n";
        ui_system->active.ID = -1;
    }


    //update mouse state
    ui_system->mouse_down = input_is_mouse_button_pressed(MOUSE_BUTTON_LEFT);
    //update mouse pos
    update_ui_mouse_pos(ui_system);
}

void ui_system_upload_draw_data(renderer* renderer, UI_System* ui_system)
{
    /* TODO: another time
    ui_system->ui_quad_vertex_buffer_handle;
    ui_system->ui_quad_index_buffer_handle;
    ui_system->text_vertex_buffer_handle;
    ui_system->text_index_buffer_handle;
    */

    vulkan_buffer_reset_offset(renderer, ui_system->ui_quad_vertex_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_system->ui_quad_index_buffer_handle);
    // vulkan_buffer_reset_offset(renderer, ui_system->text_vertex_buffer_handle);
    // vulkan_buffer_reset_offset(renderer, ui_system->text_index_buffer_handle);

    //TODO: generate drawindirect commands

    vulkan_buffer_cpu_data_copy_from_offset_handle(renderer, &ui_system->ui_quad_vertex_buffer_handle,
                                                   ui_system->draw_info.quad_vertex, sizeof(Quad_Vertex) * 100);

    vulkan_buffer_cpu_data_copy_from_offset_handle(renderer, &ui_system->ui_quad_index_buffer_handle,
                                                   ui_system->draw_info.quad_vertex, sizeof(u16) * 100);
}

void update_ui_mouse_pos(UI_System* ui_system)
{
    input_get_mouse_pos(&ui_system->mouse_pos_x, &ui_system->mouse_pos_y);
}


Quad_Vertex* UI_create_quad(vec2 pos, vec2 size, vec3 color)
{
    WARN(false, "UI_CREATE_QUAD LIKELY WILL CRASH")

    Quad_Vertex out_vertex[] = {
        {.pos = {pos.x, pos.y}, .color = color},
        {.pos = {pos.x, pos.y + size.y}, .color = color},
        {.pos = {pos.x + size.x, pos.y + size.y}, .color = color},
        {.pos = {pos.x + size.x, pos.y}, .color = color},
    };


    return out_vertex;
}

Quad_Vertex* UI_create_quad_screen_percentage(UI_System* ui_system, vec2 pos, vec2 size, vec3 color)
{
    WARN("UI_CREATE_QUAD_SCEEN_PERCENTAGE LIKELY WILL CRASH")

    Quad_Vertex* out_vertex = arena_alloc(ui_system->frame_arena, sizeof(Quad_Vertex) * 4);

    out_vertex[0] = (Quad_Vertex){.pos = {pos.x - size.x, pos.y - size.y}, .color = color};
    out_vertex[1] = (Quad_Vertex){.pos = {pos.x - size.x, pos.y + size.y}, .color = color};
    out_vertex[2] = (Quad_Vertex){.pos = {pos.x + size.x, pos.y + size.y}, .color = color};
    out_vertex[3] = (Quad_Vertex){.pos = {pos.x + size.x, pos.y - size.y}, .color = color};

    return out_vertex;
}


bool is_ui_hot(UI_System* ui_state, int id)
{
    return ui_state->hot.ID && ui_state->hot.ID == id;
}

bool is_ui_active(UI_System* ui_state, int id)
{
    return ui_state->active.ID && ui_state->active.ID == id;
}


bool region_hit(UI_System* ui_system, vec2 pos, vec2 size)
{
    //check if we are inside a ui_object

    // bottom left
    if (pos.x - size.x > ui_system->mouse_pos_x) return false;
    if (pos.y - size.y > ui_system->mouse_pos_y) return false;

    // top left
    if (pos.x - size.x > ui_system->mouse_pos_x) return false;
    if (pos.y + size.y < ui_system->mouse_pos_y) return false;

    // top right
    if (pos.x + size.x < ui_system->mouse_pos_x)return false;
    if (pos.y + size.y < ui_system->mouse_pos_y) return false;

    // bottom right
    if (pos.x + size.x < ui_system->mouse_pos_x) return false;
    if (pos.y - size.y > ui_system->mouse_pos_y) return false;

    return true;
}

bool region_hit_new(UI_System* ui_state, vec2 pos, vec2 size)
{
    //check if we are inside a ui_object

    //printf("DEBUG REGION HIT: MOUSE: %f, %f POS: %f, %f SIZE: %f, %f\n", ui_state->mouse_pos.x, ui_state->mouse_pos.y, pos.x, pos.y, size.x, size.y);

    //top left
    if (pos.x > ui_state->mouse_pos_x) return false;
    if (pos.y > ui_state->mouse_pos_y) return false;


    // bottom left
    if (pos.x > ui_state->mouse_pos_x) return false;
    if (pos.y + size.y < ui_state->mouse_pos_y) return false;


    //top right
    if (pos.x + size.x < ui_state->mouse_pos_x) return false;
    if (pos.y > ui_state->mouse_pos_y) return false;

    // bottom right
    if (pos.x + size.x < ui_state->mouse_pos_x) return false;
    if (pos.y + size.y < ui_state->mouse_pos_y) return false;


    return true;
}


/*
bool button(UI_STATE& ui_state, int id, int x, int y)
{
    if (region_hit())
    {
        ui = hot;
        if (acitve == 0 and mouse down)
            activeitem = id
    }
}*/

//check if we can use the button
bool use_button(UI_System* ui_state, UI_ID id, vec2 pos, vec2 size)
{
    //checking if we released the mouse button, are active, and we are inside the hit region
    if (ui_state->mouse_down == 0 &&
        ui_state->active.ID == id.ID &&
        region_hit(ui_state, pos, size))
        return true;

    return false;
}

bool use_button_new(UI_System* ui_state, UI_ID id, vec2 pos, vec2 size)
{
    //checking if we released the mouse button, are active, and we are inside the hit region
    if (ui_state->mouse_down == 0 &&
        ui_state->active.ID == id.ID &&
        region_hit_new(ui_state, pos, size))
        return true;

    return false;
}

int generate_id(UI_System* ui_state)
{
    return ui_state->id_generation_number++;
}

void set_hot(UI_System* ui_state, UI_ID id)
{
    if (ui_state->hot.layer <= id.layer)
    {
        ui_state->hot.ID = id.ID;
        ui_state->hot.layer = id.layer;
        //printf("ID: %d, is hot\n", id);
    }
}

void set_active(UI_System* ui_state, UI_ID id)
{
    if (ui_state->active.layer <= id.layer)
    {
        ui_state->active.ID = id.ID;
        ui_state->active.layer = id.layer;
        //printf("ID: %d, is active\n", id);
    }
}

bool can_be_active(UI_System* ui_state)
{
    return ui_state->active.ID == -1 && ui_state->mouse_down;
}

bool is_active(UI_System* ui_state, UI_ID id)
{
    return ui_state->active.ID == id.ID;
}

bool is_hot(UI_System* ui_state, UI_ID id)
{
    return ui_state->hot.ID == id.ID;
}


bool do_button(UI_System* ui_system, UI_ID id, vec2 pos, vec2 screen_percentage,
               vec3 color, vec3 hovered_color, vec3 pressed_color)
{
    /*validation*/

    //TODO: make these asserts
    if (screen_percentage.x > 100 || screen_percentage.x < 0 || screen_percentage.y > 100 || screen_percentage.y < 0)
    {
        WARN("SCREEN PERCENTAGE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
        return false;
    }
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        WARN("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
        return false;
    }

    /*POS AND SIZE CALCULATIONS*/

    vec2 converted_pos = vec2_div_scalar(pos, 100.0f);
    vec2 converted_size = vec2_div_scalar(screen_percentage, 100.0f);

    vec2 final_pos = {
        ui_system->screen_size.x * converted_pos.x,
        ui_system->screen_size.y * converted_pos.y
    };
    vec2 final_size = {
        (ui_system->screen_size.x * converted_size.x) / 2,
        (ui_system->screen_size.y * converted_size.y) / 2
    };


    /*SET UI STATE*/

    int mesh_id = ui_system->id_generation_number++;

    //check if button is hot and active
    if (region_hit(ui_system, final_pos, final_size))
    {
        set_hot(ui_system, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_system))
        {
            set_active(ui_system, id);
        }
    }
    /*
    else
    {
        printf("is not hot\n");
    }*/


    /*SET DRAW COLOR Based on the state*/
    Quad_Vertex* new_quad = arena_alloc(ui_system->frame_arena, sizeof(Quad_Vertex) * 4);

    //set color
    //active state
    if (is_active(ui_system, id))
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_system, id))
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, color);
    }


    /*SET DRAW INFO*/
    // create indices (two triangles per quad)
    uint16_t quad_indices[6];
    memcpy(quad_indices, default_quad_indices, sizeof(uint16_t) * 6);


    // Add vertices
    memcpy(ui_system->draw_info.quad_vertex + ui_system->draw_info.quad_vertex_byte_offset, new_quad,
           sizeof(Quad_Vertex) * 4);
    memcpy(ui_system->draw_info.indices + ui_system->draw_info.index_byte_offset, quad_indices, sizeof(uint16_t) * 6);

    //increase by count
    ui_system->draw_info.quad_vertex_byte_offset += sizeof(Quad_Vertex) * 4;
    ui_system->draw_info.quad_vertex_count += 4;
    //increase by bytes
    ui_system->draw_info.index_byte_offset += sizeof(uint16_t) * 6;
    ui_system->draw_info.index_count += 6;

    //check if we clicked the button
    if (use_button(ui_system, id, final_pos, final_size)) return true;

    return false;
}


bool do_button_new(UI_System* ui_system, UI_ID id, vec2 pos, vec2 size,
                   UI_Alignment alignment, vec3 color,
                   vec3 hovered_color, vec3 pressed_color)
{
    /*validation*/
    if (size.x > 100 || size.x < 0 || size.y > 100 || size.y < 0)
    {
        printf("SIZE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }

    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }


    /*POS AND SIZE CALCULATIONS*/

    vec2 converted_pos = vec2_div_scalar(pos, 100.0f);
    vec2 converted_size = vec2_div_scalar(size, 100.0f);

    //TODO: this realistically can be moved out to the ui_state struct
    //TODO: THIS IS COMPLETELY WRONG and does not work how i think it does
    float quarter_size_x = ui_system->screen_size.x * 0.25;
    float quarter_size_y = ui_system->screen_size.y * 0.25;
    float half_size_x = ui_system->screen_size.x * 0.5;
    float half_size_y = ui_system->screen_size.y * 0.5;
    vec2 final_pos;

    switch (alignment)
    {
    case UI_ALIGNMENT_CENTER:
        final_pos = (vec2){
            .x = {ui_system->screen_size.x * converted_pos.x - quarter_size_x},
            .y = {ui_system->screen_size.y * converted_pos.y - quarter_size_y},
        };
        break;
    case UI_ALIGNMENT_LEFT:
        final_pos = (vec2){
            .x = {ui_system->screen_size.x * converted_pos.x},
            .y = {ui_system->screen_size.y * converted_pos.y},
        };
        break;
    case UI_ALIGNMENT_RIGHT:
        final_pos = (vec2){
            .x = {ui_system->screen_size.x * converted_pos.x + half_size_x},
            .y = {ui_system->screen_size.y * converted_pos.y},
        };
        break;
    case UI_ALIGNMENT_MAX:
        MASSERT(false);
        break;
    }


    vec2 final_size = {
        (ui_system->screen_size.x * converted_size.x),
        (ui_system->screen_size.y * converted_size.y),
    };

    /*SET UI STATE*/

    int mesh_id = ui_system->id_generation_number++;

    //check if button is hot and active
    if (region_hit_new(ui_system, final_pos, final_size))
    {
        set_hot(ui_system, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_system))
        {
            set_active(ui_system, id);
        }
    }
    /*
    else
    {
        printf("is not hot\n");
    }*/


    //SET DRAW COLOR Based on the state
    Quad_Vertex* new_quad = arena_alloc(ui_system->frame_arena, sizeof(Quad_Vertex));

    //set color
    //active state
    if (is_active(ui_system, id))
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_system, id))
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, color);
    }


    /*SET DRAW INFO*/
    // create indices (two triangles per quad)
    uint16_t quad_indices[6];
    memcpy(quad_indices, default_quad_indices, sizeof(uint16_t) * 6);


    // Add vertices
    ui_system->draw_info.quad_vertex[ui_system->draw_info.quad_vertex_byte_offset] = *new_quad;
    memcpy(ui_system->draw_info.indices + ui_system->draw_info.index_byte_offset, quad_indices, sizeof(uint16_t) * 6);

    //increase by count
    ui_system->draw_info.quad_vertex_byte_offset += 1;
    //increase by bytes
    ui_system->draw_info.index_byte_offset += sizeof(uint16_t) * 6;

    //check if we clicked the button
    if (use_button_new(ui_system, id, final_pos, final_size)) return true;

    return false;
}

/*
bool do_button_new_text(UI_System* ui_state, UI_ID id, glm::vec2 pos, glm::vec2 size, String text,
                        glm::vec2 text_padding = {0.0f, 0.0f}, glm::vec3 color = {1.0f, 1.0f, 1.0f},
                        glm::vec3 hovered_color = {1.0f, 1.0f, 1.0f}, glm::vec3
                        pressed_color = {1.0f, 1.0f, 1.0f})
{
    //validation
    if (size.x > 100 || size.x < 0 || size.y > 100 || size.y < 0)
    {
        printf("SIZE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }

    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }


    // POS AND SIZE CALCULATIONS

    vec2 converted_pos = pos / 100.0f;
    vec2 converted_size = size / 100.0f;

    vec2 final_pos = {
        ui_state->screen_size.x * converted_pos.x,
        ui_state->screen_size.y * converted_pos.y,
    };

    vec2 final_size = {
        (ui_state->screen_size.x * converted_size.x),
        (ui_state->screen_size.y * converted_size.y),
    };

    do_text(ui_state, text, pos + text_padding, vec3{1.0, 1.0, 0.0});


    //SET UI STATE

    int mesh_id = ui_state->draw_info.vertex_info.mesh_id++;

    //check if button is hot and active
    if (region_hit_new(ui_state, final_pos, final_size))
    {
        set_hot(ui_state, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_state))
        {
            set_active(ui_state, id);
        }
    }

    //else
    //{
     //   printf("is not hot\n");
    //}


    //SET DRAW COLOR Based on the state
    std::vector<Vertex> new_quad;

    //set color
    //active state
    if (is_active(ui_state, id))
    {
        new_quad = UI_create_quad(final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_state, id))
    {
        new_quad = UI_create_quad(final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad(final_pos, final_size, color);
    }


    // SET DRAW INFO

    uint16_t base_index =
    <uint16_t > (ui_state->draw_info.vertex_info.dynamic_vertices.size());

    // create indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        <uint16_t > (base_index + 0),
        <uint16_t > (base_index + 1),
        <uint16_t > (base_index + 2),
        <uint16_t > (base_index + 2),
        <uint16_t > (base_index + 3),
        <uint16_t > (base_index + 0)
    };

    // Add vertices
    ui_state->draw_info.vertex_info.dynamic_vertices.insert(ui_state->draw_info.vertex_info.dynamic_vertices.end(),
                                                            new_quad.begin(),
                                                            new_quad.end());
    // Add indices
    ui_state->draw_info.vertex_info.dynamic_indices.insert(ui_state->draw_info.vertex_info.dynamic_indices.end(),
                                                           quad_indices.begin(), quad_indices.end());

    //check if we clicked the button
    if (use_button_new(ui_state, id, final_pos, final_size)) return true;

    return false;
}

bool do_button_text(UI_System* ui_state, UI_ID id, String text, glm::vec2 pos, glm::vec2 screen_percentage,
                    glm::vec3 color = {1.0f, 1.0f, 1.0f}, glm::vec3 hovered_color = {1.0f, 1.0f, 1.0f}, glm::vec3
                    pressed_color = {1.0f, 1.0f, 1.0f})
{
    //validation

    if (screen_percentage.x > 100 || screen_percentage.x < 0 || screen_percentage.y > 100 || screen_percentage.y < 0)
    {
        printf("SCREEN PERCENTAGE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }

    //POS AND SIZE CALCULATIONS

    glm::vec2 converted_pos = pos / 100.0f;
    glm::vec2 converted_size = screen_percentage / 100.0f;

    glm::vec2 final_pos = {
        ui_state->push_constants.screenSize.x * converted_pos.x,
        ui_state->push_constants.screenSize.y * converted_pos.y
    };
    glm::vec2 final_size = {
        (ui_state->push_constants.screenSize.x * converted_size.x) / 2,
        (ui_state->push_constants.screenSize.y * converted_size.y) / 2
    };

    //do_text(ui_state, text, final_pos, {1.0f,1.0f,1.0f});
    do_text_screen_percentage(ui_state, text, pos, screen_percentage, {0.0f, 0.0f, 1.0f}, 48);


    //SET UI STATE

    int mesh_id = ui_state->draw_info.vertex_info.mesh_id++;

    //check if button is hot and active
    if (region_hit(ui_state, final_pos, final_size))
    {
        set_hot(ui_state, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_state))
        {
            set_active(ui_state, id);
        }
    }

    //else
    //{
    //    printf("is not hot\n");
    //}


    //SET DRAW COLOR Based on the state
    std::vector<Vertex> new_quad;

    //set color
    //active state
    if (is_active(ui_state, id))
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_state, id))
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, color);
    }


    //SET DRAW INFO

    uint16_t base_index =
    <uint16_t > (ui_state->draw_info.vertex_info.dynamic_vertices.size());

    // create indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        <uint16_t > (base_index + 0),
        <uint16_t > (base_index + 1),
        <uint16_t > (base_index + 2),
        <uint16_t > (base_index + 2),
        <uint16_t > (base_index + 3),
        <uint16_t > (base_index + 0)
    };

    // Add vertices
    ui_state->draw_info.vertex_info.dynamic_vertices.insert(ui_state->draw_info.vertex_info.dynamic_vertices.end(),
                                                            new_quad.begin(),
                                                            new_quad.end());
    // Add indices
    ui_state->draw_info.vertex_info.dynamic_indices.insert(ui_state->draw_info.vertex_info.dynamic_indices.end(),
                                                           quad_indices.begin(), quad_indices.end());

    //check if we clicked the button
    if (use_button(ui_state, id, final_pos, final_size)) return true;

    return false;
}
*/
