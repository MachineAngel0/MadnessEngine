#ifndef UI_TYPES_H
#define UI_TYPES_H


typedef enum UI_Property_Flags
{
    UI_FLAG_NONE = BITFLAG(0),
    UI_FLAG_BACKGROUND = BITFLAG(1),
    UI_FLAG_CLICKABLE = BITFLAG(2),
    UI_FLAG_IMAGE = BITFLAG(3),
    UI_FLAG_TEXT = BITFLAG(4),
    UI_FLAG_OUTLINE = BITFLAG(5),
    UI_FLAG_SCROLL_VIEW = BITFLAG(6),
    UI_FLAG_COLOR = BITFLAG(7), // this is kinda implied all the time
    UI_FLAG_DRAGGABLE = BITFLAG(8), //NOTE: child elements are not draggable, only a root parent is draggable
    UI_FLAG_ROUND_CORNER = BITFLAG(9),
    UI_FLAG_CIRCLE = BITFLAG(10),
    UI_FLAG_SCROLL_FLOAT = BITFLAG(11),
    UI_FLAG_TEXT_INPUT = BITFLAG(12),
    UI_FLAG_PRESSED = BITFLAG(13), //for handling states in which the item should have a different color if its in the selected state
    UI_FLAG_SCISSOR_START = BITFLAG(14),
    UI_FLAG_SCISSOR_END = BITFLAG(15),
    // UI_FLAG_ = BITFLAG(16),
    // UI_FLAG_ = BITFLAG(32),
} UI_Property_Flags;


typedef enum UI_Interaction_Event
{
    UI_EVENT_HOVER = BITFLAG(0),
    UI_EVENT_CLICK = BITFLAG(1),
    UI_EVENT_DRAG = BITFLAG(2),
    UI_EVENT_SCROLL = BITFLAG(3),
    UI_EVENT_TEXT_INPUT = BITFLAG(4),
    UI_EVENT_KEYBOARD = BITFLAG(5),
    UI_EVENT_CONTROLLER = BITFLAG(6),
    UI_EVENT_FLOAT_CHANGE = BITFLAG(7),
    // UI_EVENT_ = BITFLAG(32),
} UI_Interaction_Event;

typedef enum UI_Alignment
{
    //you can use x and y alignments to get something like bottom right
    //can be used as a nine slice
    UI_ALIGNMENT_LEFT,
    UI_ALIGNMENT_CENTER,
    UI_ALIGNMENT_RIGHT,
} UI_Alignment;

typedef enum UI_Draw_Command
{
    UI_DRAW_TYPE_QUAD,
    // UI_DRAW_TYPE_IMAGE, //maybe
    UI_DRAW_TYPE_TEXT,
    UI_DRAW_TYPE_SCISSOR_START,
    UI_DRAW_TYPE_SCISSOR_END,
    //anything else can be for specific shaders that I might want later, like for the game ui
}UI_Draw_Command;

typedef struct UI_Node_Draw_Data
{
    UI_Property_Flags ui_flags;

    // screen size and pos, not normalized
    vec2 pos;
    vec2 size;
    float rotation; // degrees, but gets converted to radians at draw time

    //rounded
    float rounded_radius; // 0-1 range

    //outline
    vec3 outline_color;
    float outline_thickness; // 0-1 :: ideally should be something small like 0.05-0.1


    //for circles
    float thickness; // size of the circle is determined by the pos and size

    //draw data
    //consider here what actually needs to be done for something to rendered, instead of passing in the entire config
    u32 texture_handle;
    vec2 uv_offset;
    vec2 uv_size;

    //colors
    vec3 color;
    vec3 background_color;

    // scissor data
    // vec2 scissor_pos;
    // vec2 scissor_size;
} UI_Node_Draw_Data;


typedef struct UI_Render_Packet
{
    UI_Node_Draw_Data* ui_material_data;
    u64 ui_material_data_count;
    u64 ui_material_bytes;
    UI_Draw_Command* draw_command;
    u64 draw_command_count;
} UI_Render_Packet;








#endif//UI_TYPES_H

