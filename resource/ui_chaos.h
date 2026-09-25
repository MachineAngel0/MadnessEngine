#ifndef CHAOS_UI_H
#define CHAOS_UI_H

#include "allocator.h"
#include "input.h"
#include "profiler.h"
#include "resource_types.h"
#include "str.h"
#include "ui_types.h"
#include "cglm/struct/vec2.h"



//NOTE: an auto layout ui, that i dont really like using that much, but its here in case I do want to use it


#define INSANITY_UI_DEBUG_PRINT


#define INSANITY_DEFAULT_FONT_SIZE 32.0f
#define INSANITY_EDITOR_FONT_SIZE 16.0f
#define INSANITY_TEXT_OUTLINE 0.5f

#define INSANITY_UI_MAX_NODE_COUNT 1000
#define INSANITY_UI_MAX_WINDOW_COUNT 100
#define INSANITY_MAX_UI_NODE_CHILD_COUNT 32


typedef enum UI_Sizing
{
    UI_Sizing_Fixed,
    UI_Sizing_Grow, // sizes to take up remaining space
    UI_Sizing_Fit, //sizes to content
    UI_Sizing_Percent,
} UI_Sizing;


typedef struct UI_Padding
{
    u32 left;
    u32 right;
    u32 top;
    u32 bottom;
} UI_Padding;

typedef enum UI_Text_Wrap
{
    UI_Text_Wrap_None,
    UI_Text_Wrap_Wrap,
    UI_Text_Wrap_Newline,
} UI_Text_Wrap;


typedef enum Insanity_UI_Layout_Direction
{
    Insanity_UI_Layout_Horizontal,
    Insanity_UI_Layout_Vertical,
} Insanity_UI_Layout_Direction;




typedef struct Chaos_Auto_Node
{
    const char* name_id;
    u32 hash_id;

    vec2s pos; // relative (until layout step)
    vec2s size;


    UI_Sizing sizing_type_x; // fixed, percent, auto
    UI_Sizing sizing_type_y; // fixed, percent, auto
    UI_Alignment_X x_alignment;
    UI_Alignment_Y y_alignment;
    Insanity_UI_Layout_Direction layout_direction;

    UI_Padding padding;
    f32 child_padding; // for all directions

    vec2s max_size;
    vec2s min_size;

    vec3s color;

    Insanity_UI_Node_Type type;
    UI_Property_Flags flags;

    //render
    float rotation;
    float thickness;
    float rounded_radius;
    vec3s outline_color;
    float outline_thickness;


    //text
    u16 font_size;
    u16 letter_spacing;
    // u16 line_height;
    // wrap_mode
    String* text;
    UI_Text_Wrap text_wrap;
    //texture
    u32 texture_handle;
    vec2s uv_offset;
    vec2s uv_size;

    struct Chaos_Auto_Node* parent;
    struct Chaos_Auto_Node* child[INSANITY_MAX_UI_NODE_CHILD_COUNT];
    u8 child_count;
} Chaos_Auto_Node;







typedef struct Chaos_UI
{
    Allocator* allocator; // rn mainly just for loading fonts, would be better as a pool arena
    Allocator* scratch_allocator; //small amount of memory
    Frame_Allocator* frame_allocator;

    Input_System* input_system; // does not own memory

    //NOTE: this could be a context pointer with function ptr for getting what you need, if you wanted to make this a library
    Asset_System* asset_system; // does not own memory,

    vec2s screen_size; // this gets queried every frame in the begin effect

    //for invalid states, pass this back instead of crashing
    Chaos_Auto_Node dummy_node;

    //this should be an array at some point
    Texture_Handle default_font_handle;
    float default_font_size;
    float editor_font_size;
    float text_outline;
    // Font fonts[100];



    ARRAY_TYPE(IUI_Auto_Node)* ui_auto_nodes;


    //Render
    UI_Render_Node* render_node_array;
    u64 render_node_array_count;
    UI_Draw_Command* draw_command_array;
    u64 draw_command_count;
    u64 current_draw_command_count;

} Chaos_UI;

static Chaos_UI* chaos_ui;


//API
bool chaos_ui_init(Memory_System* memory_system, Input_System* input_system,
                      Asset_System* asset_system);
bool chaos_ui_deinit(void);

//pass in the size every frame, in the event the size changes
void chaos_ui_begin(s32 screen_size_x, s32 screen_size_y);


//Note: needs to be called right before the renderers update method, to generate the appropriate render data
Insanity_UI_Render_Packet chaos_ui_end(void);
void chaos_ui_resolve_interaction(void);

void chaos_ui_test(float dt, float elapsed_time);



Chaos_Auto_Node* chaos_ui_auto_node(const char* name)
{
    Chaos_Auto_Node* return_node = (Chaos_Auto_Node*)_array_get(chaos_ui->ui_auto_nodes,
                                                            chaos_ui->ui_auto_nodes->num_items++);
    return_node->name_id = name;
    chaos_ui->hash = hash_32_continous(chaos_ui->hash, (u8*)name, strlen(name));
    return_node->hash_id = chaos_ui->hash;
    // return_node->ui_flags;
    return_node->color = chaos_ui->editor_style.error_color;

    return return_node;
}

Chaos_Auto_Node* chaos_ui_auto_text(const char* name, UI_Text_Wrap text_wrap)
{
}


void chaos_ui_add_child(Chaos_Auto_Node* parent, Chaos_Auto_Node* child)
{
    parent->child[parent->child_count++] = child;
    child->parent = parent;
}


void chaos_ui_layout_fit_sizing_widths(Chaos_Auto_Node* root)
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
        Chaos_Auto_Node* child = root->child[i];
        switch (root->layout_direction)
        {
        case Insanity_UI_Layout_Horizontal:

            // child->pos.y = root->pos.y + (f32)root->padding_top;
            pos_x_offset += child->size.x + root->child_padding;

            //size the parent
            if (root->sizing_type_x == UI_Sizing_Fit)
            {
                root->size.x += child->size.x;
                //takes on the childs min size
                root->min_size.x += child->min_size.x;
            }

            break;
        case Insanity_UI_Layout_Vertical:
            //size the parent
            if (root->sizing_type_x == UI_Sizing_Fit)
            {
                root->size.x = max_f(root->size.x, child->size.x + root->padding.left + root->padding.right);

                root->min_size.x = max_f(root->min_size.x, child->min_size.x);
            }


            break;
        }


        chaos_ui_layout_fit_sizing_widths(child);
    }

    // add total gap once after the loop
    if (root->layout_direction == Insanity_UI_Layout_Horizontal)
    {
        root->size.x += total_gap;
    }


    if (root->size.x > root->max_size.x && root->max_size.x > 0)
    {
        root->size.x = root->max_size.x;
    }
}

void chaos_ui_layout_fit_sizing_heights(Chaos_Auto_Node* root)
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
        Chaos_Auto_Node* child = root->child[i];
        switch (root->layout_direction)
        {
        case Insanity_UI_Layout_Horizontal:

            //size the parent
            if (root->sizing_type_y == UI_Sizing_Fit)
            {
                root->size.y = max_f(root->size.y, child->size.y + root->padding.top + root->padding.bottom);
                //takes on the childs min size
                root->min_size.y = max_f(root->min_size.y, child->min_size.y);
            }
            break;
        case Insanity_UI_Layout_Vertical:
            // child->pos.y = root->pos.y + (f32)root->padding_top;
            pos_y_offset += child->size.y + root->child_padding;
            //size the parent
            if (root->sizing_type_y == UI_Sizing_Fit)
            {
                root->size.y += child->size.y;


                root->min_size.y += child->min_size.y;
            }

            break;
        }


        chaos_ui_layout_fit_sizing_widths(child);
    }

    // add total gap once after the loop
    if (root->layout_direction == Insanity_UI_Layout_Vertical)
    {
        root->size.y += total_gap;
    }

    if (root->size.y > root->max_size.y && root->max_size.y > 0)
    {
        root->size.y = root->max_size.y;
    }
}


void chaos_ui_layout_grow_shrink_sizing_width(Chaos_Auto_Node* root)
{
    if (root->child_count <= 0) { return; }


    //grow sizing
    float remaining_width = root->size.x;

    remaining_width -= root->padding.left + root->padding.right;

    Dynamic_Array* growables_x_array = dynamic_array_create_allocator(Chaos_Auto_Node*, 128, chaos_ui->frame_allocator)
    for (u32 i = 0; i < root->child_count; i++)
    {
        Chaos_Auto_Node* child = root->child[i];
        remaining_width -= child->size.x;

        if (child->sizing_type_x == UI_Sizing_Grow)
        {
            dynamic_array_push(growables_x_array, &child);
        }
    }


    float total_gap = 0;
    if (root->child_count > 0)
    {
        total_gap = (root->child_count - 1) * root->child_padding;
    }
    remaining_width -= total_gap;

    //grow for width
    while (remaining_width > 0 && !dynamic_array_is_empty(growables_x_array))
    {
        //grab the first node
        Chaos_Auto_Node* node = dynamic_array_get(growables_x_array, 0, Chaos_Auto_Node*);
        float smallest = node->size.x;
        float second_smallest = INFINITY;
        float width_to_add = remaining_width;

        for (u32 i = 0; i < growables_x_array->num_items; i++)
        {
            Chaos_Auto_Node* child = dynamic_array_get(growables_x_array, i, Chaos_Auto_Node*);

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
            Chaos_Auto_Node* child = dynamic_array_get(growables_x_array, i, Chaos_Auto_Node*);
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

    //shrink for width
    while (remaining_width < 0 && !dynamic_array_is_empty(growables_x_array))
    {
        //grab the first node
        Chaos_Auto_Node* node = dynamic_array_get(growables_x_array, 0, Chaos_Auto_Node*);
        float largest = node->size.x;
        float second_largest = INFINITY;
        float width_to_add = remaining_width;

        for (u32 i = 0; i < growables_x_array->num_items; i++)
        {
            Chaos_Auto_Node* child = dynamic_array_get(growables_x_array, i, Chaos_Auto_Node*);

            if (child->size.x > largest)
            {
                second_largest = largest;
                largest = child->size.x;
            }
            if (child->size.x < largest)
            {
                second_largest = max_f(second_largest, child->size.x);
                width_to_add = second_largest - largest;
            }
        }

        width_to_add = max_f(width_to_add, remaining_width / growables_x_array->num_items);
        for (u32 i = 0; i < growables_x_array->num_items; i++)
        {
            Chaos_Auto_Node* child = dynamic_array_get(growables_x_array, i, Chaos_Auto_Node*);
            float previous_width = child->size.x;
            if (child->size.x == largest)
            {
                child->size.x += width_to_add;

                if (child->size.x <= child->min_size.x)
                {
                    child->size.x = child->min_size.x;
                    dynamic_array_remove_swap(growables_x_array, i);
                    i--;
                }

                remaining_width -= (child->size.x - previous_width);
            }
        }
    }


    for (u32 i = 0; i < root->child_count; i++)
    {
        chaos_ui_layout_grow_shrink_sizing_width(root->child[i]);
    }
}


void chaos_ui_layout_grow_shrink_sizing_height(Chaos_Auto_Node* root)
{
    Dynamic_Array* growables_y_array = dynamic_array_create_allocator(Chaos_Auto_Node*, 128, chaos_ui->frame_allocator)
    for (u32 i = 0; i < root->child_count; i++)
    {
        Chaos_Auto_Node* child = root->child[i];
        if (child->sizing_type_y == UI_Sizing_Grow)
        {
            dynamic_array_push(growables_y_array, &child);
        }
    }

    float remaining_height = root->size.y;
    remaining_height -= root->padding.top + root->padding.bottom;

    //size for height
    for (u32 i = 0; i < growables_y_array->num_items; i++)
    {
        Chaos_Auto_Node* child = dynamic_array_get(growables_y_array, i, Chaos_Auto_Node*);

        if (child->sizing_type_y == UI_Sizing_Grow)
        {
            child->size.y += (remaining_height - child->size.y);
        }
    }
}


void chaos_ui_layout_wrap_text(Chaos_Auto_Node* root)
{
    for (u32 i = 0; i < root->child_count; i++)
    {
        Chaos_Auto_Node* child = root->child[i];

        if (child->type == Insanity_UI_Node_Type_Text)
        {
            //do the thing
        }


        chaos_ui_layout_wrap_text(child);
    }
}


void chaos_ui_layout_position(Chaos_Auto_Node* root)
{
    //fit sizing widths
    //get all fixed sized items and add them to any fit sized items
    u32 pos_x_offset = root->padding.left;
    u32 pos_y_offset = root->padding.top;
    for (u32 i = 0; i < root->child_count; i++)
    {
        Chaos_Auto_Node* child = root->child[i];
        switch (root->layout_direction)
        {
        case Insanity_UI_Layout_Horizontal:
            child->pos.x = root->pos.x + (f32)pos_x_offset;
            child->pos.y = root->pos.y + (f32)pos_y_offset;

            // child->pos.y = root->pos.y + (f32)root->padding_top;
            pos_x_offset += child->size.x + root->child_padding;


            break;
        case Insanity_UI_Layout_Vertical:
            child->pos.x = root->pos.x + (f32)pos_x_offset;
            child->pos.y = root->pos.y + (f32)pos_y_offset;

            // child->pos.y = root->pos.y + (f32)root->padding_top;
            pos_y_offset += child->size.y + root->child_padding;
            break;
        }


        chaos_ui_layout_position(child);
    }
}


void chaos_ui_layout_alignment(Chaos_Auto_Node* root)
{
    //get size of parent container
    //get size of all first level child containers
    //align based on what the root wants for the x alignment
    // y alignment is per child
    float container_width = root->size.x;
    float container_height = root->size.y;

    float remaining_width = 0;


    for (u32 i = 0; i < root->child_count; i++)
    {
        Chaos_Auto_Node* child = root->child[i];
        remaining_width += child->size.x;
    }

    switch (root->x_alignment)
    {
    case UI_ALIGNMENT_X_LEFT:
        remaining_width = 0;
        break;
    case UI_ALIGNMENT_X_CENTER:
        remaining_width *= 0.5f;
        break;
    case UI_ALIGNMENT_X_RIGHT:
        break;
    }

    for (u32 i = 0; i < root->child_count; i++)
    {
        Chaos_Auto_Node* child = root->child[i];
        child->pos.x += remaining_width;

        switch (child->y_alignment)
        {
        case UI_ALIGNMENT_Y_TOP:
            //nothing
            break;
        case UI_ALIGNMENT_Y_CENTER:
            child->pos.y += (container_height - child->size.y)/2;
            break;
        case UI_ALIGNMENT_Y_BOTTOM:
            child->pos.y += container_height - child->size.y;
            break;
        }

    }


    for (u32 i = 0; i < root->child_count; i++)
    {
        chaos_ui_layout_alignment(root->child[i]);
    }
}

void chaos_ui_resolve_layout(Chaos_Auto_Node* root)
{
    PROFILE_ZONE(chaos_ui_resolve_layout)


    //fit sizing width
    chaos_ui_layout_fit_sizing_widths(root);

    //grow/shrink width
    chaos_ui_layout_grow_shrink_sizing_width(root);

    //wrap text
    chaos_ui_layout_wrap_text(root);

    //fit sizing height
    chaos_ui_layout_fit_sizing_heights(root);

    //grow/shrink height
    chaos_ui_layout_grow_shrink_sizing_height(root);

    //position
    chaos_ui_layout_position(root);

    //alignment
    chaos_ui_layout_alignment(root);


    PROFILE_ZONE_END(chaos_ui_resolve_layout);
}


#endif //CHAOS_UI_H
