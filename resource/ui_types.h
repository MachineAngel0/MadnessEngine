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
    // UI_FLAG_ = BITFLAG(14),
    // UI_FLAG_ = BITFLAG(15),
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

    // //scissor data
    // vec2 scissor_start;
    // vec2 scissor_end;
} UI_Node_Draw_Data;

typedef struct UI_Render_Packet
{
    UI_Node_Draw_Data* ui_data;
    u64 ui_data_count;
    u64 ui_data_bytes;
} UI_Render_Packet;

typedef enum UI_Draw_Type
{
    UI_Draw_Type_UI,
    UI_Draw_Type_TEXT,
    UI_Draw_Type_SCISSOR_START,
    UI_Draw_Type_SCISSOR_END,
}UI_Draw_Type;

typedef struct UI_Draw_Type_Data
{
    union
    {
        struct draw
        {
            u32 data_start;
            u32 data_length;
        };
        struct scissor
        {
            u32 scissor_offset;
            u32 scissor_extent;
        };
    };
}UI_Draw_Type_Data;


typedef struct UI_Draw_Command
{
    UI_Draw_Type* ui_types; // this assumes 1 draw per ui object, easiest
    u32 type_count;

    UI_Node_Draw_Data* ui_data;
    u64 ui_data_count;
    u64 ui_data_bytes;

} UI_Draw_Command;




#endif//UI_TYPES_H

