#ifndef UI_H
#define UI_H

//IMMEDIATE MODE UI



typedef enum UI_Alignment
{
    UI_ALIGNMENT_CENTER,
    UI_ALIGNMENT_LEFT,
    UI_ALIGNMENT_RIGHT,
    //JUSTIFIED,
    UI_ALIGNMENT_MAX,
} UI_Alignment;

typedef struct UI_ID
{
    int ID;
    int layer;
} UI_ID;

//TODO: update to SOA
typedef struct UI_BUTTON
{
    UI_ID id;
    vec2 position;
    vec2 screen_percentage;
    vec3 color;
    vec3 hover_color;
    vec3 pressed_color;
    int _padding;
} UI_BUTTON;


typedef struct Quad_Vertex
{
    vec2 pos;
    vec3 color;
} Quad_Vertex;

typedef struct Quad_Textured_Vertex
{
    vec2 pos;
    vec2 tex_coord;
} Quad_Textured_Vertex;


Quad_Vertex default_quad_vertex[4]= {
    {{-0.5f, -0.5f},  {1.0f, 0.0f}},
    {{0.5f, -0.5f},  {0.0f, 0.0f}},
    {{0.5f, 0.5f},  {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f}}
};
uint16_t default_quad_indices[6] = {
    0, 1, 2, 2, 3, 0
};


typedef struct
{
    vec2* pos;
    vec3* color;
    //there is an interesting optimization given that all UI's are squares
    VkDrawIndexedIndirectCommand* indirect_draw_array;
} UI_Draw_data_new;

typedef struct UI_Draw_Data
{
    //the 100 is temporary
    Quad_Vertex quad_vertex[100];
    u32 quad_vertex_byte_offset;
    u32 quad_vertex_count;

    u16 indices[100];
    u32 index_byte_offset;
    u32 index_count;

    UI_BUTTON* UI_Objects; // darray or maybe even an allocator
} UI_Draw_Data;



typedef enum UI_Type
{
    color,
    texture,
} UI_Type;


typedef struct UI_System
{
    Frame_Arena* frame_arena;

    UI_ID hot;
    UI_ID active;

    int id_generation_number;

    int mouse_down;
    int mouse_released;
    i16 mouse_pos_x;
    i16 mouse_pos_y;

    vec2 screen_size;

    UI_Draw_Data draw_info;
    // Text_System text_system;

    Buffer_Handle ui_quad_vertex_buffer_handle;
    Buffer_Handle ui_quad_index_buffer_handle;
    Buffer_Handle text_vertex_buffer_handle;
    Buffer_Handle text_index_buffer_handle;

} UI_System;

/* TODO: make sense to pass in as a param for the UI at some point
typedef struct Transform_2D
{
    vec2 translation{};
    float rotation{};
    vec2 scale{1.0f, 1.0f};

    mat2 _mat2()
    {
        float s = sin(rotation);
        float c = cos(rotation);
        mat2 rotMatrix = (mat2){{scale.x, s}, {c, scale.y}};

        mat2 scaleMat = (mat2){{scale.x, 0.0f}, {0.0f, scale.y}};
        return rotMatrix * scaleMat;
    }
} Transform_2D;
*/


//NOTE: Remove the renderer from the init, these should not be coupled
//I should only have to pass the vertex/index data to the renderer for drawing
UI_System* ui_system_init(renderer* renderer);

//pass in the size every frame, in the event the size changes
void ui_begin(UI_System* ui_state, i32 screen_size_x, i32 screen_size_y);
void ui_end(UI_System* ui_system);


//VULKAN
void ui_system_upload_draw_data(renderer* renderer, UI_System* ui_system);




//for drawing
Quad_Vertex* UI_create_quad(vec2 pos, vec2 size, vec3 color);


Quad_Vertex* UI_create_quad_screen_percentage(UI_System* ui_system, vec2 pos, vec2 size, vec3 color);

bool is_ui_hot(UI_System* ui_state, int id);

bool is_ui_active(UI_System* ui_state, int id);

bool region_hit(UI_System* ui_system, vec2 pos, vec2 size);

bool region_hit_new(UI_System* ui_state, vec2 pos, vec2 size);

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


//UTILITY

void update_ui_mouse_pos(UI_System* ui_system);

//check if we can use the button
bool use_button(UI_System* ui_state, UI_ID id, vec2 pos, vec2 size);

bool use_button_new(UI_System* ui_state, UI_ID id, vec2 pos, vec2 size);

int generate_id(UI_System* ui_state);

void set_hot(UI_System* ui_state, UI_ID id);

void set_active(UI_System* ui_state, UI_ID id);

bool can_be_active(UI_System* ui_state);
bool is_active(UI_System* ui_state, UI_ID id);

bool is_hot(UI_System* ui_state, UI_ID id);


//API
bool do_button(UI_System* ui_system, UI_ID id, vec2 pos, vec2 screen_percentage,
               vec3 color, vec3 hovered_color, vec3 pressed_color);

bool do_button_new(UI_System* ui_system, UI_ID id, vec2 pos, vec2 size,
                   UI_Alignment alignment, vec3 color,
                   vec3 hovered_color, vec3 pressed_color);

#define DO_BUTTON_TEST(UI_SYSTEM, UI_ID) do_button(UI_SYSTEM, UI_ID, (vec2){0.0f,0.0f}, (vec2){10.0f,10.0f}, (vec3){1.0f, 1.0f, 1.0f}, (vec3){1.0f, 1.0f, 1.0f}, (vec3){1.0f, 1.0f, 1.0f});


/*
bool do_button(UI_System* ui_system, UI_ID id, vec2 pos, vec2 screen_percentage,
               vec3 color = {1.0f, 1.0f, 1.0f}, vec3 hovered_color = {1.0f, 1.0f, 1.0f},
               vec3 pressed_color = {1.0f, 1.0f, 1.0f});


bool do_button_new(UI_System* ui_system, UI_ID id, vec2 pos, vec2 size,
                   UI_Alignment alignment = UI_Alignment::UI_ALIGNMENT_LEFT, vec3 color = {1.0f, 1.0f, 1.0f},
                   vec3 hovered_color = {1.0f, 1.0f, 1.0f}, vec3
                   pressed_color = {1.0f, 1.0f, 1.0f});
*/

/* TODO: after everything else is sorted
bool do_button_new_text(UI_System* ui_state, UI_ID id, vec2 pos, vec2 size, String text,
                        vec2 text_padding = {0.0f, 0.0f}, vec3 color = {1.0f, 1.0f, 1.0f},
                        vec3 hovered_color = {1.0f, 1.0f, 1.0f}, vec3
                        pressed_color = {1.0f, 1.0f, 1.0f});

bool do_button_text(UI_System* ui_state, UI_ID id, String text, vec2 pos, vec2 screen_percentage,
                    vec3 color = {1.0f, 1.0f, 1.0f}, vec3 hovered_color = {1.0f, 1.0f, 1.0f}, vec3
                    pressed_color = {1.0f, 1.0f, 1.0f});
*/

#endif //UI_H
