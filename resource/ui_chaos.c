#include "ui_chaos.h"

#include "logger.h"
#include "math_lib.h"
#include "profiler.h"


bool chaos_ui_init(Memory_System* memory_system, Input_System* input_system,
                      Asset_System* asset_system)
{
    chaos_ui = memory_system_alloc(memory_system, sizeof(chaos_UI), MEMORY_SUBSYSTEM_UI);
    MASSERT(chaos_ui);

    u64 ui_arena_mem_size = MB(16);
    u64 ui_frame_arena_mem_size = MB(16);


    chaos_ui->allocator = memory_system_allocator_create(memory_system, ui_arena_mem_size, MEMORY_SUBSYSTEM_UI);
    chaos_ui->frame_allocator = memory_system_allocator_create(memory_system, ui_frame_arena_mem_size,
                                                                  MEMORY_SUBSYSTEM_UI);

    chaos_ui->input_system = input_system;
    chaos_ui->asset_system = asset_system;


    chaos_ui->default_font_size = INSANITY_DEFAULT_FONT_SIZE;
    chaos_ui->editor_font_size = INSANITY_EDITOR_FONT_SIZE;
    chaos_ui->text_outline = INSANITY_TEXT_OUTLINE;







    chaos_ui->dummy_node = (Chaos_Auto_Node){0};


    INFO("CHAOS UI CREATED");
    return true;
}

bool chaos_ui_deinit(void)
{
    return true;
}

void chaos_ui_begin(s32 screen_size_x, s32 screen_size_y)
{
    MASSERT(chaos_ui);

    PROFILE_ZONE(chaos_ui_begin)

    //clear node/draw info
    allocator_clear(chaos_ui->frame_allocator);

    chaos_ui->ui_auto_nodes = array_create(Chaos_Auto_Node, INSANITY_UI_MAX_NODE_COUNT, chaos_ui->frame_allocator);




    chaos_ui->screen_size.x = screen_size_x;
    chaos_ui->screen_size.y = screen_size_y;





    PROFILE_ZONE_END(chaos_ui_begin)
}


Insanity_UI_Render_Packet chaos_ui_end(void)
{
    MASSERT(chaos_ui);
    PROFILE_ZONE(chaos_ui_end)




    //OPTIMIZE: figure it out
    u32 render_node_count = 0;

    for (u32 i = 0; i < chaos_ui->ui_auto_nodes->num_items; i++)
    {
        Chaos_Auto_Node* node = &array_get(chaos_ui->ui_auto_nodes, i, Chaos_Auto_Node);
        switch (node->type)
        {
        case Insanity_UI_Node_Type_Rect:
            render_node_count++;
            break;
        case Insanity_UI_Node_Type_Text:
            render_node_count += node->text->length;
            break;
        case Insanity_UI_Node_Type_Scissor_Start:
            render_node_count++;
            break;
        case Insanity_UI_Node_Type_Scissor_End:
            render_node_count++;
            break;
        }
    }


    chaos_ui->render_node_array = allocator_alloc(insanity_ui->frame_allocator,
                                                     render_node_count * sizeof(UI_Render_Node));
    chaos_ui->render_node_array_count = render_node_count;


    chaos_ui->draw_command_array = allocator_alloc(insanity_ui->frame_allocator,
                                                      render_node_count * sizeof(UI_Draw_Command));
    chaos_ui->draw_command_count = render_node_count;
    chaos_ui->current_draw_command_count = 0;

    u32 render_node_instance = 0;

    for (u32 i = 0; i < chaos_ui->ui_auto_nodes->num_items; i++)
    {
        Chaos_Auto_Node* node_data = &array_get(chaos_ui->ui_auto_nodes, i, Chaos_Auto_Node);

        switch (node_data->type)
        {
        case Insanity_UI_Node_Type_Rect:

            UI_Render_Node* render_node = &chaos_ui->render_node_array[render_node_instance];

            render_node->ui_flags = node_data->flags;
            render_node->pos = glms_vec2_div(node_data->pos, chaos_ui->screen_size);
            render_node->size = glms_vec2_div(node_data->size, chaos_ui->screen_size);
            render_node->rotation = deg_to_rad(node_data->rotation);

            render_node->thickness = node_data->thickness;

            render_node->rounded_radius = node_data->rounded_radius;

            render_node->color = node_data->color;

            render_node->texture_handle = node_data->texture_handle;
            render_node->uv_offset = node_data->uv_offset;
            render_node->uv_size = node_data->uv_size;

            render_node->outline_color = node_data->outline_color;
            render_node->outline_thickness = node_data->outline_thickness;

            ui_add_draw_command(chaos_ui->draw_command_array, chaos_ui->current_draw_command_count,
                                UI_DRAW_TYPE_DRAW, glms_vec2_zero(), glms_vec2_zero());

            render_node_instance++;

            break;
        case Insanity_UI_Node_Type_Text:
            //expand string
            //generate the actual text now that we have the proper position
            vec2s text_current_pos = node_data->pos;
            f32 font_scalar = chaos_ui->editor_font_size / chaos_ui->default_font_size;
            Madness_Font font_data;
            texture_system_get_font(chaos_ui->asset_system->texture_system, chaos_ui->default_font_handle,
                                    &font_data);

            for (u64 text_idx = 0; text_idx < node_data->text->length; text_idx++)
            {
                const char c = node_data->text->chars[text_idx];

                if (c < 32 || c >= 128) continue; // skip unsupported characters

                Glyph* g = &font_data.glyphs[c - 32];

                f32 x_position = text_current_pos.x + ((float)g->xoff * font_scalar);
                f32 y_position = text_current_pos.y + ((float)g->yoff * font_scalar);

                f32 x_width = ((f32)g->width * font_scalar);
                f32 y_height = ((f32)g->height * font_scalar);

                UI_Render_Node* text_render_node = &chaos_ui->render_node_array[render_node_instance];
                text_render_node->pos = (vec2s){
                    /*node_data->pos.x +*/ x_position, /*node_data->pos.y +*/ y_position
                };
                text_render_node->pos = glms_vec2_div(text_render_node->pos, chaos_ui->screen_size);

                text_render_node->size = (vec2s){x_width, y_height};
                text_render_node->size = glms_vec2_div(text_render_node->size, chaos_ui->screen_size);

                text_render_node->rotation = deg_to_rad(node_data->rotation);
                text_render_node->uv_offset = (vec2s){g->u0, g->v0};
                text_render_node->uv_size = (vec2s){g->u1 - g->u0, g->v1 - g->v0};
                text_render_node->color = node_data->color;
                text_render_node->texture_handle = chaos_ui->default_font_handle.handle;
                text_render_node->ui_flags = node_data->flags;
                text_render_node->ui_flags |= UI_FLAG_TEXT;

                // render_node->outline_thickness = insanity_ui->text_outline;
                // render_node->outline_color = insanity_ui->text_outline_color;
                // render_node->thickness = node_data->thickness;

                text_current_pos.x += (g->advance) * font_scalar; // move offset forward
                ui_add_draw_command(chaos_ui->draw_command_array, chaos_ui->current_draw_command_count,
                                    UI_DRAW_TYPE_DRAW, glms_vec2_zero(), glms_vec2_zero());
                render_node_instance++;
            }

            break;
        case Insanity_UI_Node_Type_Scissor_Start:
            //no need to fill out the render nodes, they are zeroed anyway
            ui_add_draw_command(chaos_ui->draw_command_array, chaos_ui->current_draw_command_count,
                                UI_DRAW_TYPE_SCISSOR_START, glms_vec2_zero(), glms_vec2_zero());
            render_node_instance++;
            break;
        case Insanity_UI_Node_Type_Scissor_End:
            ui_add_draw_command(chaos_ui->draw_command_array, chaos_ui->current_draw_command_count,
                                UI_DRAW_TYPE_SCISSOR_START, glms_vec2_zero(), glms_vec2_zero());
            render_node_instance++;
            break;
        }
    }


    PROFILE_ZONE_END(insanity_ui_end)

    return (Insanity_UI_Render_Packet){
        .material_data = chaos_ui->render_node_array,
        .material_data_count = chaos_ui->render_node_array_count,
        .material_bytes = chaos_ui->render_node_array_count * sizeof(UI_Render_Node),
        .draw_command = chaos_ui->draw_command_array,
        .draw_command_count = chaos_ui->draw_command_count,
    };
}


void chaos_ui_test(float dt, float elapsed_time)
{
    PROFILE_ZONE(chaos_ui_test)


    Chaos_Auto_Node* root = chaos_ui_auto_node("root");
    root->size = (vec2s){1000, 500};
    // root->sizing_type_x = UI_Sizing_Fit;
    // root->sizing_type_y = UI_Sizing_Fit;
    root->layout_direction = Insanity_UI_Layout_Horizontal;
    // root->layout_direction = Insanity_UI_Layout_Vertical;
    // root->size = (vec2s){200, 200};
    root->color = COLOR_RED;
    root->padding.left = 32.f;
    root->padding.right = 32.f;
    // root->padding.top = 32.f;
    // root->padding.bottom = 32.f;
    root->child_padding = 32.f;
    Chaos_Auto_Node* child1 = insanity_ui_auto_node("child1");
    child1->size = (vec2s){100, 100};
    child1->color = COLOR_BLUE;
    child1->y_alignment = UI_ALIGNMENT_Y_CENTER;
    Chaos_Auto_Node* child2 = insanity_ui_auto_node("child2");
    // child2->size = (vec2s){100, 100};
    child2->sizing_type_x = UI_Sizing_Grow;
    child2->sizing_type_y = UI_Sizing_Grow;
    child2->color = COLOR_VIOLET;
    Chaos_Auto_Node* child3 = insanity_ui_auto_node("child3");
    child3->size = (vec2s){100, 100};
    child3->color = COLOR_ORANGE;
    Chaos_Auto_Node* child4 = insanity_ui_auto_node("child4");
    child4->sizing_type_x = UI_Sizing_Grow;
    child4->sizing_type_y = UI_Sizing_Grow;
    child4->color = COLOR_YELLOW;
    Chaos_Auto_Node* child3_child1 = insanity_ui_auto_node("child3_child1");
    // child3_child1->sizing_type_x = UI_Sizing_Grow;
    child3_child1->sizing_type_y = UI_Sizing_Grow;
    child3_child1->size = (vec2s){50, 50};
    child3_child1->color = COLOR_GREEN;


    insanity_ui_add_child(root, child1);
    insanity_ui_add_child(root, child2);
    insanity_ui_add_child(root, child3);
    insanity_ui_add_child(root, child4);

    insanity_ui_add_child(child3, child3_child1);


    insanity_ui_resolve_layout(root);


    Chaos_Auto_Node* button = insanity_ui_auto_node("button");
    button->size = (vec2s){100, 100};
    button->color = COLOR_YELLOW;
    button->x_alignment = UI_ALIGNMENT_X_CENTER;
    button->y_alignment = UI_ALIGNMENT_Y_CENTER;
    Chaos_Auto_Node* text = insanity_ui_auto_node("button_text");
    text->size = (vec2s){50, 50};
    text->x_alignment = UI_ALIGNMENT_X_CENTER;
    text->y_alignment = UI_ALIGNMENT_Y_CENTER;
    text->color = COLOR_VIOLET;

    insanity_ui_add_child(button, text);
    insanity_ui_resolve_layout(button);

    PROFILE_ZONE_END(chaos_ui_test)
}
