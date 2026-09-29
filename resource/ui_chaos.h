#ifndef CHAOS_UI_H
#define CHAOS_UI_H

#include "input.h"
#include "resource_types.h"
#include "ui_chaos_structs_enums.h"

typedef struct Chaos_UI
{
    Allocator* allocator; // rn mainly just for loading fonts, would be better as a pool arena
    Allocator* scratch_allocator; //small amount of memory
    Frame_Allocator* frame_allocator;

    Input_System* input_system; // does not own memory

    //NOTE: this could be a context pointer with function ptr for getting what you need, if you wanted to make this a library
    Asset_System* asset_system; // does not own memory,

    vec2s screen_size; // this gets queried every frame in the begin effect

    CUI_Default_Style default_style;

    //for invalid states, pass this back instead of crashing
    CUI_Node dummy_node;

    //this should be an array at some point
    Texture_Handle default_font_handle;
    float default_font_size;
    float editor_font_size;
    float text_outline;
    // Font fonts[100];


    ARRAY_TYPE(CUI_Node)* ui_nodes;

    //Renderer

    //draw commands
    Chaos_UI_Render_Packet render_packet;
} Chaos_UI;

static Chaos_UI* chaos_ui;


//API
bool chaos_ui_init(Memory_System* memory_system, Input_System* input_system,
                   Asset_System* asset_system);
bool chaos_ui_deinit(void);

//pass in the size every frame, in the event the size changes
void chaos_ui_begin(s32 screen_size_x, s32 screen_size_y);


//Note: needs to be called right before the renderers update method, to generate the appropriate render data
Chaos_UI_Render_Packet chaos_ui_end(void);
void chaos_ui_resolve_interaction(void);

void chaos_ui_test(float dt, float elapsed_time);


//buildling blocks
CUI_Node* cui_node(const char* name);

CUI_Node* cui_text(const char* name, CUI_Text_Wrap text_wrap);
CUI_Node* cui_image(const char* name, u32 image_index);

CUI_Node* cui_scissor_start(const char* name, vec2s pos, vec2s size);
CUI_Node* cui_scissor_end(const char* name);


void cui_add_child(CUI_Node* parent, CUI_Node* child);


//auto layout
void chaos_ui_resolve_layout(CUI_Node* root);

void cui_layout_fit_sizing_widths(CUI_Node* root);
void cui_layout_fit_sizing_heights(CUI_Node* root);

void cui_layout_grow_shrink_sizing_width(CUI_Node* root);
void cui_layout_grow_shrink_sizing_height(CUI_Node* root);

void cui_layout_wrap_text(CUI_Node* root);

void cui_layout_position(CUI_Node* root);
void cui_layout_view_offsets(CUI_Node* root);





// manual layout things


//widgets
void cui_scroll_begin(const char* name, CUI_Scroll_Flags flags)
{

}
void cui_scroll_end(void)
{

}


void cui_button()
{

    CUI_Node* cui_node(const char* name);


}




#endif //CHAOS_UI_H
