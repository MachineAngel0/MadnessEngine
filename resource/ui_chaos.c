#include "ui_chaos.h"

#include "logger.h"
#include "math_lib.h"
#include "profiler.h"
#include "ui_chaos_structs_enums.h"


bool chaos_ui_init(Memory_System* memory_system, Input_System* input_system,
                   Asset_System* asset_system)
{
    chaos_ui = memory_system_alloc(memory_system, sizeof(Chaos_UI), MEMORY_SUBSYSTEM_UI);
    MASSERT(chaos_ui);

    u64 ui_arena_mem_size = MB(16);
    u64 ui_frame_arena_mem_size = MB(16);


    chaos_ui->allocator = memory_system_allocator_create(memory_system, ui_arena_mem_size, MEMORY_SUBSYSTEM_UI);
    chaos_ui->frame_allocator = memory_system_allocator_create(memory_system, ui_frame_arena_mem_size,
                                                               MEMORY_SUBSYSTEM_UI);

    chaos_ui->input_system = input_system;
    chaos_ui->asset_system = asset_system;


    chaos_ui->default_font_size = CHAOS_DEFAULT_FONT_SIZE;
    chaos_ui->editor_font_size = CHAOS_EDITOR_FONT_SIZE;
    chaos_ui->text_outline = CHAOS_TEXT_OUTLINE;


    chaos_ui->dummy_node = (CUI_Node){0};


    chaos_ui->default_style = (CUI_Default_Style){
        .layout_color = COLOR4_PURPLE_PALETTE_DARK, .layout_accent_color = COLOR4_PURPLE_PALETTE_PURPLE,
        .text_color = COLOR4_PURPLE_PALETTE_LIGHT, .textbox_color = COLOR4_PURPLE_PALETTE_DARK2,
        .custom_widget_color = COLOR4_PURPLE_PALETTE_PURPLE_LIGHT,
        .color = COLOR4_PURPLE_PALETTE_PURPLE_STRONG, .hovered_color = COLOR4_PURPLE_PALETTE_PURPLE_LIGHT2,
        .pressed_color = COLOR4_PURPLE_PALETTE_DARK2,
        .outline_color = COLOR4_PURPLE_PALETTE_PURPLE_LIGHT,
        .permanent_active = COLOR4_HOT_PINK,
        .header_color = (vec4s){0.425f, 0.05f, 0.456f, 1.0f},
        .pop_up_color = (vec4s){0.110f, 0.120f, 0.162f, 1.0f},
        .scrollbox_color = COLOR4_HOT_PINK,
        .error_color = COLOR4_GREEN,
    };

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

    chaos_ui->ui_nodes = array_create(CUI_Node, CHAOS_UI_MAX_NODE_COUNT, chaos_ui->frame_allocator);


    chaos_ui->screen_size.x = screen_size_x;
    chaos_ui->screen_size.y = screen_size_y;


    PROFILE_ZONE_END(chaos_ui_begin)
}


Chaos_UI_Render_Packet chaos_ui_end(void)
{
    MASSERT(chaos_ui);
    PROFILE_ZONE(chaos_ui_end)


    //
    chaos_ui->render_packet.render_items = allocator_alloc(chaos_ui->frame_allocator,
                                                           sizeof(CUI_Render_Item)
                                                           * chaos_ui->ui_nodes->num_items);
    chaos_ui->render_packet.render_items_count = chaos_ui->ui_nodes->num_items;
    chaos_ui->render_packet.render_items_byte_size = sizeof(CUI_Render_Item) * chaos_ui->ui_nodes->num_items;


    for (u32 i = 0; i < chaos_ui->ui_nodes->num_items; i++)
    {
        CUI_Node* node = &array_get(chaos_ui->ui_nodes, i, CUI_Node);

        chaos_ui->render_packet.render_items[i] = (CUI_Render_Item){
            .render_type = node->render_type,
            .pos = node->pos,
            .size = node->size,
            .rotation = node->rotation,
            .rounded_corners = node->rounded_radius,
            .texture_id = node->texture_handle,
            .uv_offset = node->uv_offset,
            .uv_size = node->uv_size,
            .radius = node->rounded_radius,
            .color = node->color,
            .outline_thickness = node->outline_thickness,
            .outline_color = node->outline_color,
        };
    }


    //below doesn't work is we need to expand text
    //optimize: use a dynamic array that grows from the end
    chaos_ui->render_packet.render_command_array = allocator_alloc(chaos_ui->frame_allocator,
                                                                   sizeof(Chaos_UI_Render_Command)
                                                                   * chaos_ui->ui_nodes->num_items);
    chaos_ui->render_packet.render_command_count = 0;


    //TODO: TEMP FOR NOW
    chaos_ui->render_packet.render_command_array[0].buffer_offset = 0;
    chaos_ui->render_packet.render_command_array[0].draw_count = chaos_ui->ui_nodes->num_items;
    chaos_ui->render_packet.render_command_array[0].type = Chaos_UI_Render_Command_Type_Draw;
    chaos_ui->render_packet.render_command_count = 1;


    PROFILE_ZONE_END(chaos_ui_end)

    return chaos_ui->render_packet;
}


CUI_Node* cui_node(const char* name)
{
    CUI_Node* return_node = _array_get(chaos_ui->ui_nodes,
                                       chaos_ui->ui_nodes->num_items++);

    return_node->render_type = CUI_Render_Flags_Quad;
    return_node->name_id = name;
    return_node->hash_id = c_string_hash_u32(name);
    // return_node->ui_flags;
    return_node->color = chaos_ui->default_style.error_color;


    return return_node;
}

CUI_Node* cui_text(const char* name, CUI_Text_Wrap text_wrap)
{
    CUI_Node* return_node = _array_get(chaos_ui->ui_nodes,
                                       chaos_ui->ui_nodes->num_items++);

    return_node->render_type = CUI_Render_Flags_Text;
    return_node->name_id = name;
    return_node->hash_id = c_string_hash_u32(name);
    // return_node->ui_flags;
    return_node->color = chaos_ui->default_style.error_color;

    return_node->text = STRING_CREATE_FROM_BUFFER_ALLOCATOR(name, chaos_ui->frame_allocator);

    return return_node;
}

CUI_Node* cui_image(const char* name, u32 image_index)
{
    CUI_Node* return_node = _array_get(chaos_ui->ui_nodes,
                                       chaos_ui->ui_nodes->num_items++);

    return_node->render_type = CUI_Render_Flags_Image;
    return_node->name_id = name;
    return_node->hash_id = c_string_hash_u32(name);
    // return_node->ui_flags;
    return_node->color = chaos_ui->default_style.error_color;

    //TODO: add uv offsets and size if needed, probably as a push style
    return_node->texture_handle = image_index;

    return return_node;
}

CUI_Node* cui_scissor_start(const char* name, vec2s pos, vec2s size)
{
    CUI_Node* return_node = _array_get(chaos_ui->ui_nodes,
                                       chaos_ui->ui_nodes->num_items++);

    return_node->render_type = CUI_Render_Flags_Scissor;
    return_node->name_id = name;
    return_node->hash_id = c_string_hash_u32(name);
    // return_node->ui_flags;
    return_node->color = chaos_ui->default_style.error_color;
    return_node->pos = pos;
    return_node->size = size;

    return return_node;
}

CUI_Node* cui_scissor_end(const char* name)
{
    CUI_Node* return_node = _array_get(chaos_ui->ui_nodes,
                                       chaos_ui->ui_nodes->num_items++);

    return_node->render_type = CUI_Render_Flags_Scissor;
    return_node->name_id = name;
    return_node->hash_id = c_string_hash_u32(name);
    // return_node->ui_flags;
    return_node->color = chaos_ui->default_style.error_color;
    return_node->pos = glms_vec2_zero();
    return_node->size = chaos_ui->screen_size;

    return return_node;
}


void cui_add_child(CUI_Node* parent, CUI_Node* child)
{
    MASSERT(parent->child_count < CHAOS_MAX_UI_NODE_CHILD_COUNT);
    parent->child[parent->child_count++] = child;
    child->parent = parent;
}

void cui_layout_fit_sizing_widths(CUI_Node* root)
{
    //fit sizing widths
    //get all fixed sized items and add them to any fit sized items
    u32 pos_x_offset = root->padding.left;
    float total_gap = 0;
    if (root->child > 0)
    {
        total_gap = (root->child_count - 1) * root->child_padding;
    }
    root->size.x += root->padding.left + root->padding.right;
    for (u32 i = 0; i < root->child_count; i++)
    {
        //assuming horizontal layout
        CUI_Node* child = root->child[i];
        switch (root->layout_direction)
        {
        case CUI_Layout_Horizontal:

            // child->pos.y = root->pos.y + (f32)root->padding_top;
            pos_x_offset += child->size.x + root->child_padding;

            //size the parent
            if (root->sizing_type_x == CUI_Sizing_Fit)
            {
                root->size.x += child->size.x;
                //takes on the childs min size
                root->min_size.x += child->min_size.x;
            }

            break;
        case CUI_Layout_Vertical:
            //size the parent
            if (root->sizing_type_x == CUI_Sizing_Fit)
            {
                root->size.x = max_f(root->size.x, child->size.x + root->padding.left + root->padding.right);

                root->min_size.x = max_f(root->min_size.x, child->min_size.x);
            }


            break;
        }


        cui_layout_fit_sizing_widths(child);
    }

    // add total gap once after the loop
    if (root->layout_direction == CUI_Layout_Horizontal)
    {
        root->size.x += total_gap;
    }


    if (root->size.x > root->max_size.x && root->max_size.x > 0)
    {
        root->size.x = root->max_size.x;
    }
}

void cui_layout_fit_sizing_heights(CUI_Node* root)
{
    //fit sizing widths
    //get all fixed sized items and add them to any fit sized items
    u32 pos_y_offset = root->padding.top;
    float total_gap = 0;
    if (root->child > 0)
    {
        total_gap = (root->child_count - 1) * root->child_padding;
    }
    root->size.y += root->padding.top + root->padding.bottom;
    for (u32 i = 0; i < root->child_count; i++)
    {
        //assuming horizontal layout
        CUI_Node* child = root->child[i];
        switch (root->layout_direction)
        {
        case CUI_Layout_Horizontal:

            //size the parent
            if (root->sizing_type_y == CUI_Sizing_Fit)
            {
                root->size.y = max_f(root->size.y, child->size.y + root->padding.top + root->padding.bottom);
                //takes on the childs min size
                root->min_size.y = max_f(root->min_size.y, child->min_size.y);
            }
            break;
        case CUI_Layout_Vertical:
            // child->pos.y = root->pos.y + (f32)root->padding_top;
            pos_y_offset += child->size.y + root->child_padding;
            //size the parent
            if (root->sizing_type_y == CUI_Sizing_Fit)
            {
                root->size.y += child->size.y;


                root->min_size.y += child->min_size.y;
            }

            break;
        }


        cui_layout_fit_sizing_widths(child);
    }

    // add total gap once after the loop
    if (root->layout_direction == CUI_Layout_Vertical)
    {
        root->size.y += total_gap;
    }

    if (root->size.y > root->max_size.y && root->max_size.y > 0)
    {
        root->size.y = root->max_size.y;
    }
}

void cui_layout_grow_shrink_sizing_width(CUI_Node* root)
{
    if (root->child_count <= 0) { return; }

    //grow sizing
    float remaining_width = root->size.x;

    remaining_width -= root->padding.left + root->padding.right;

    float vertical_remaining_width = remaining_width; // if the container is vertical, we use this

    float total_gap = 0;
    if (root->child_count > 0)
    {
        total_gap = (root->child_count - 1) * root->child_padding;
    }
    remaining_width -= total_gap;

    //there can only be 32 children
    Dynamic_Array* growables_x_array = dynamic_array_create_allocator(CUI_Node*, 32, chaos_ui->frame_allocator)
    for (u32 i = 0; i < root->child_count; i++)
    {
        CUI_Node* child = root->child[i];
        remaining_width -= child->size.x;

        if (child->sizing_type_x == CUI_Sizing_Grow)
        {
            dynamic_array_push(growables_x_array, &child);
        }
    }

    switch (root->layout_direction)
    {
    case CUI_Layout_Vertical:
        //TODO: not exactly right, should take into account padding
        //set the size to width of the container
        for (u32 i = 0; i < growables_x_array->num_items; i++)
        {
            CUI_Node* child = dynamic_array_get(growables_x_array, i, CUI_Node*);
            child->size.x += vertical_remaining_width - child->size.x;
            if (child->size.x >= child->max_size.x && child->max_size.x != 0)
            {
                child->size.x = child->max_size.x;
            }
        }

        break;
    case CUI_Layout_Horizontal:


        //grow for width
        if (remaining_width > 0)
        {
            while (remaining_width > 0)
            {
                if (dynamic_array_is_empty(growables_x_array)) { break; }
                //grab the first node
                CUI_Node* node = dynamic_array_get(growables_x_array, 0, CUI_Node*);
                float smallest = node->size.x;
                float second_smallest = FLT_MAX;
                float width_to_add = remaining_width;

                for (u32 i = 0; i < growables_x_array->num_items; i++)
                {
                    CUI_Node* child = dynamic_array_get(growables_x_array, i, CUI_Node*);

                    if (child->size.x < smallest)
                    {
                        second_smallest = smallest;
                        smallest = child->size.x;
                    }
                    if (child->size.x > smallest)
                    {
                        second_smallest = min_f(second_smallest, child->size.x);
                        width_to_add = second_smallest - smallest;
                    }
                }

                width_to_add = min_f(width_to_add, remaining_width / growables_x_array->num_items);
                for (u32 i = 0; i < growables_x_array->num_items; i++)
                {
                    CUI_Node* child = dynamic_array_get(growables_x_array, i, CUI_Node*);
                    float previous_width = child->size.x;
                    if (child->size.x == smallest)
                    {
                        child->size.x += width_to_add;
                        if (child->size.x >= child->max_size.x && child->max_size.x != 0)
                        {
                            child->size.x = child->max_size.x;
                            dynamic_array_remove_swap(growables_x_array, i);
                            i--;
                        }
                        remaining_width -= (child->size.x - previous_width);
                    }
                }
            }
        }
        else
        {
            //shrink for width
            while (remaining_width < 0)
            {
                if (dynamic_array_is_empty(growables_x_array)) { break; }
                //grab the first node
                CUI_Node* node = dynamic_array_get(growables_x_array, 0, CUI_Node*);
                float largest = node->size.x;
                float second_largest = -FLT_MAX;
                float width_to_remove = remaining_width;

                for (u32 i = 0; i < growables_x_array->num_items; i++)
                {
                    CUI_Node* child = dynamic_array_get(growables_x_array, i, CUI_Node*);

                    if (child->size.x > largest)
                    {
                        second_largest = largest;
                        largest = child->size.x;
                    }
                    if (child->size.x < largest)
                    {
                        second_largest = max_f(second_largest, child->size.x);
                        width_to_remove = second_largest - largest;
                    }
                }

                width_to_remove = max_f(width_to_remove, remaining_width / growables_x_array->num_items);
                for (u32 i = 0; i < growables_x_array->num_items; i++)
                {
                    CUI_Node* child = dynamic_array_get(growables_x_array, i, CUI_Node*);
                    float previous_width = child->size.x;
                    if (child->size.x == largest)
                    {
                        child->size.x -= width_to_remove;

                        if (child->size.x <= child->min_size.x)
                        {
                            child->size.x = child->min_size.x;
                            dynamic_array_remove_swap(growables_x_array, i);
                            i--;
                        }

                        remaining_width += (child->size.x - previous_width);
                    }
                }
            }
        }
        break;
    }


    for (u32 i = 0; i < root->child_count; i++)
    {
        cui_layout_grow_shrink_sizing_width(root->child[i]);
    }
}


void cui_layout_grow_shrink_sizing_height(CUI_Node* root)
{
    Dynamic_Array* growables_y_array = dynamic_array_create_allocator(CUI_Node*, 128, chaos_ui->frame_allocator)
    for (u32 i = 0; i < root->child_count; i++)
    {
        CUI_Node* child = root->child[i];
        if (child->sizing_type_y == CUI_Sizing_Grow)
        {
            dynamic_array_push(growables_y_array, &child);
        }
    }

    float remaining_height = root->size.y;
    remaining_height -= root->padding.top + root->padding.bottom;

    float horizontal_remaining_height = remaining_height;

    float total_gap = 0;
    if (root->child_count > 0)
    {
        total_gap = (root->child_count - 1) * root->child_padding;
    }
    remaining_height -= total_gap;

    switch (root->layout_direction)
    {
    case CUI_Layout_Horizontal:
        //size for height
        for (u32 i = 0; i < growables_y_array->num_items; i++)
        {
            CUI_Node* child = dynamic_array_get(growables_y_array, i, CUI_Node*);
            child->size.y += (horizontal_remaining_height - child->size.y);
            //TODO: CLAMP THE SIZE TO THE MIN/MAX
        }

        break;
     case CUI_Layout_Vertical:
        //grow for height
        if (remaining_height > 0 && !dynamic_array_is_empty(growables_y_array))
        {
            while (remaining_height > 0)
            {
                //grab the first node
                CUI_Node* node = dynamic_array_get(growables_y_array, 0, CUI_Node*);
                float smallest = node->size.y;
                float second_smallest = FLT_MAX;
                float height_to_add = remaining_height;

                for (u32 i = 0; i < growables_y_array->num_items; i++)
                {
                    CUI_Node* child = dynamic_array_get(growables_y_array, i, CUI_Node*);

                    if (child->size.y < smallest)
                    {
                        second_smallest = smallest;
                        smallest = child->size.y;
                    }
                    if (child->size.y > smallest)
                    {
                        second_smallest = min_f(second_smallest, child->size.x);
                        height_to_add = second_smallest - smallest;
                    }
                }

                height_to_add = min_f(height_to_add, remaining_height / growables_y_array->num_items);
                for (u32 i = 0; i < growables_y_array->num_items; i++)
                {
                    CUI_Node* child = dynamic_array_get(growables_y_array, i, CUI_Node*);
                    float previous_width = child->size.y;
                    if (child->size.y == smallest)
                    {
                        child->size.y += height_to_add;
                        if (child->size.y >= child->max_size.y && child->max_size.y != 0)
                        {
                            child->size.y = child->max_size.y;
                            dynamic_array_remove_swap(growables_y_array, i);
                            i--;
                        }
                        remaining_height -= (child->size.y - previous_width);
                    }
                }
            }
        }
        else
        {
            //shrink for height
            while (remaining_height < 0 && !dynamic_array_is_empty(growables_y_array))
            {
                //grab the first node
                CUI_Node* node = dynamic_array_get(growables_y_array, 0, CUI_Node*);
                float largest = node->size.y;
                float second_largest = -FLT_MAX;
                float height_to_remove = remaining_height;

                for (u32 i = 0; i < growables_y_array->num_items; i++)
                {
                    CUI_Node* child = dynamic_array_get(growables_y_array, i, CUI_Node*);

                    if (child->size.y > largest)
                    {
                        second_largest = largest;
                        largest = child->size.y;
                    }
                    if (child->size.y < largest)
                    {
                        second_largest = max_f(second_largest, child->size.y);
                        height_to_remove = second_largest - largest;
                    }
                }

                height_to_remove = max_f(height_to_remove, remaining_height / growables_y_array->num_items);
                for (u32 i = 0; i < growables_y_array->num_items; i++)
                {
                    CUI_Node* child = dynamic_array_get(growables_y_array, i, CUI_Node*);
                    float previous_width = child->size.y;
                    if (child->size.y == largest)
                    {
                        child->size.y -= height_to_remove;

                        if (child->size.y <= child->min_size.y)
                        {
                            child->size.y = child->min_size.y;
                            dynamic_array_remove_swap(growables_y_array, i);
                            i--;
                        }

                        remaining_height += (child->size.y - previous_width);
                    }
                }
            }
        }

        break;
    }


    for (u32 i = 0; i < root->child_count; i++)
    {
        cui_layout_grow_shrink_sizing_height(root->child[i]);
    }
}


void cui_layout_wrap_text(CUI_Node* root)

{
    for (u32 i = 0; i < root->child_count; i++)
    {
        CUI_Node* child = root->child[i];


        cui_layout_wrap_text(child);
    }
}


void cui_layout_position(CUI_Node* root)
{
    //fit sizing widths
    //get all fixed sized items and add them to any fit sized items
    u32 pos_x_offset = root->padding.left;
    u32 pos_y_offset = root->padding.top;
    for (u32 i = 0; i < root->child_count; i++)
    {
        CUI_Node* child = root->child[i];
        switch (root->layout_direction)
        {
        case CUI_Layout_Horizontal:
            child->pos.x = root->pos.x + (f32)pos_x_offset;
            child->pos.y = root->pos.y + (f32)pos_y_offset;

            // child->pos.y = root->pos.y + (f32)root->padding_top;
            pos_x_offset += child->size.x + root->child_padding;


            break;
        case CUI_Layout_Vertical:
            child->pos.x = root->pos.x + (f32)pos_x_offset;
            child->pos.y = root->pos.y + (f32)pos_y_offset;

            // child->pos.y = root->pos.y + (f32)root->padding_top;
            pos_y_offset += child->size.y + root->child_padding;
            break;
        }


        cui_layout_position(child);
    }
}


void cui_layout_view_offsets(CUI_Node* root)
{
    //scroll view offsets
    for (u32 i = 0; i < root->child_count; i++)
    {
        root->child[i]->pos.y -= root->view_offset;
        cui_layout_view_offsets(root->child[i]);
    }
}




void chaos_ui_resolve_layout(CUI_Node* root)
{
    PROFILE_ZONE(chaos_ui_resolve_layout)

    //fit sizing width
    cui_layout_fit_sizing_widths(root);

    //grow/shrink width
    cui_layout_grow_shrink_sizing_width(root);

    //wrap text
    cui_layout_wrap_text(root);

    //fit sizing height
    cui_layout_fit_sizing_heights(root);

    //grow/shrink height
    cui_layout_grow_shrink_sizing_height(root);

    //position
    cui_layout_position(root);

    //alignment
    //TODO: well alignment is gone cause i deleted like a dumbass


    //layouts
    cui_layout_view_offsets(root);





    PROFILE_ZONE_END(chaos_ui_resolve_layout);
}


void chaos_ui_test(float dt, float elapsed_time)
{
    PROFILE_ZONE(chaos_ui_test)

    /*CUI_Node* root = cui_node("root");
    root->size = (vec2s){1000, 500};
    // root->sizing_type_x = UI_Sizing_Fit;
    // root->sizing_type_y = UI_Sizing_Fit;
    root->layout_direction = CUI_Layout_Horizontal;
    // root->layout_direction = CUI_Layout_Vertical;
    // root->size = (vec2s){200, 200};
    root->color = COLOR4_RED;
    root->padding.left = 32.f;
    root->padding.right = 32.f;
    // root->padding.top = 32.f;
    // root->padding.bottom = 32.f;
    root->child_padding = 32.f;


    CUI_Node* child1 = cui_node("child1");
    child1->size = (vec2s){100, 100};
    child1->color = COLOR4_BLUE;
    child1->y_alignment = CUI_ALIGNMENT_CENTER;
    CUI_Node* child2 = cui_node("child2");
    // child2->size = (vec2s){100, 100};
    child2->sizing_type_x = CUI_Sizing_Grow;
    child2->sizing_type_y = CUI_Sizing_Grow;
    child2->color = COLOR4_VIOLET;
    CUI_Node* child3 = cui_node("child3");
    child3->size = (vec2s){100, 100};
    child3->color = COLOR4_ORANGE;

    CUI_Node* child3_child1 = cui_node("child3_child1");
    // child3_child1->sizing_type_x = UI_Sizing_Grow;
    child3_child1->sizing_type_y = CUI_Sizing_Grow;
    child3_child1->size = (vec2s){50, 50};
    child3_child1->color = COLOR4_GREEN;

    CUI_Node* child4 = cui_node("child4");
    child4->sizing_type_x = CUI_Sizing_Grow;
    child4->sizing_type_y = CUI_Sizing_Grow;
    child4->color = COLOR4_YELLOW;

    cui_add_child(root, child1);
    cui_add_child(root, child2);
    cui_add_child(root, child3);
    cui_add_child(root, child4);

    cui_add_child(child3, child3_child1);


    chaos_ui_resolve_layout(root);

    CUI_Node* button = cui_node("button");
    button->size = (vec2s){100, 100};
    button->color = COLOR4_YELLOW;
    button->x_alignment = CUI_ALIGNMENT_CENTER;
    button->y_alignment = CUI_ALIGNMENT_CENTER;

    CUI_Node* text = cui_node("button_text");
    text->size = (vec2s){50, 50};
    text->x_alignment = CUI_ALIGNMENT_CENTER;
    text->y_alignment = CUI_ALIGNMENT_CENTER;
    text->color = COLOR4_VIOLET;

    cui_add_child(button, text);
    chaos_ui_resolve_layout(button);*/

    //trying to make a scroll box


    CUI_Node* scroll = cui_node("scroll");
    scroll->pos = (vec2s){500, 500};
    scroll->size = (vec2s){200, 200};
    scroll->color = COLOR4_GREY;
    scroll->padding.top = 10;
    scroll->padding.bottom = 10;
    // TODO: add a scissor region, begin and end
    // if you consider popping this from a parent stack then this would be the scissor region,
    // or even handle in the resolve loop
    CUI_Node* scroll_view = cui_node("scroll_view");
    scroll_view->layout_direction = CUI_Layout_Vertical;
    scroll_view->sizing_type_x = CUI_Sizing_Grow;
    scroll_view->sizing_type_y = CUI_Sizing_Grow;
    scroll_view->color = COLOR4_MAGENTA;
    scroll_view->child_padding = 10;
    cui_add_child(scroll, scroll_view);


    //we can only display 32 children, but what if you really did have 1000 children?
    //we want the dimensions of the scroll view, but we wont know it until we resolve the layout
    static CUI_Scroll scroll_state;
    scroll_state.viewport_height = 200;
    scroll_state.scroll_speed = 5;

    if (input_is_mouse_wheel_up())
        {scroll_state.y_scroll -= scroll_state.scroll_speed;}

    if (input_is_mouse_wheel_down())
        {scroll_state.y_scroll += scroll_state.scroll_speed;}
    scroll_state.y_scroll = clamp_f32(scroll_state.y_scroll, 0.0f, scroll_state.max_scroll);

    // u32 item_count = 1000;
    u32 item_count = 30;
    f32 item_height = 10.0f;
    f32 content_height = item_count * item_height;


    scroll_state.max_scroll =
       max_f(0.0f, content_height - scroll_state.viewport_height);

    scroll_view->size.y = scroll_state.viewport_height;

    scroll_state.y_scroll =
        clamp_f32(
            scroll_state.y_scroll,
            0.0f,
            scroll_state.max_scroll
        );


    u32 first_item =
        (u32)(scroll_state.y_scroll / item_height);

    f32 before_height = first_item * item_height;

    u32 last_item = min_i(first_item + 32, item_count);
    f32 after_height =
        (item_count - last_item) * item_height;

    scroll_view->view_offset = scroll_state.y_scroll;


    CUI_Node* spacer_before = cui_node("item");
    spacer_before->size.y = before_height;
    cui_add_child(scroll_view, spacer_before);

    for (u32 i = 0; i < 30; i++)
    {
        u32 item_index = first_item + i;

        if (item_index >= item_count)
            break;

        CUI_Node* item = cui_node("item");
        item->color = (vec4s){0.1 * i, 0, 0, 1.0};
        item->size = (vec2s){0.f, item_height};
        item->sizing_type_x = CUI_Sizing_Grow;

        if (i == 29)
        {
            item->color = COLOR4_GREEN;

        }

        cui_add_child(scroll_view, item);
    }
    CUI_Node* spacer_after = cui_node("item");
    spacer_after->size.y = after_height;
    cui_add_child(scroll_view, spacer_after);


    CUI_Node* scroll_bar = cui_node("scrollbar");
    scroll_bar->layout_direction = CUI_Layout_Vertical;
    scroll_bar->sizing_type_y = CUI_Sizing_Grow;
    // scroll_bar->padding.left = 1;
    // scroll_bar->padding.right = 1;
    scroll_bar->size.x = scroll->size.x * 0.1;

    CUI_Node* scroll_region = cui_node("scroll_region");
    scroll_region->sizing_type_x = CUI_Sizing_Grow;
    // scroll_region->size.x = 10;
    scroll_region->size.y = 10; // size to some percent of size and scroll offset
    scroll_region->color = COLOR4_ORANGE;

    cui_add_child(scroll, scroll_bar);
    cui_add_child(scroll_bar, scroll_region);

    chaos_ui_resolve_layout(scroll);



    /*
    for (u32 i = 0; i < scroll_view->child_count; i++)
    {
        scroll_view->child[i]->pos.y -= wheel * 10.f;
    }
    */




    PROFILE_ZONE_END(chaos_ui_test)
}
