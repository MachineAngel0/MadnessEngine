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
    UI_FLAG_PRESSED = BITFLAG(13),
    //for handling states in which the item should have a different color if its in the selected state
    UI_FLAG_SCISSOR_START = BITFLAG(14),
    UI_FLAG_SCISSOR_END = BITFLAG(15),
    // UI_FLAG_ = BITFLAG(16),
    // UI_FLAG_ = BITFLAG(32),
} UI_Property_Flags;

typedef enum Insanity_UI_Node_Type
{
    Insanity_UI_Node_Type_Rect,
    Insanity_UI_Node_Type_Text,
    Insanity_UI_Node_Type_Scissor_Start,
    Insanity_UI_Node_Type_Scissor_End,
} Insanity_UI_Node_Type;


typedef enum UI_Alignment
{
    UI_ALIGNMENT_LEFT, // also top
    UI_ALIGNMENT_CENTER,
    UI_ALIGNMENT_RIGHT,// also bottom
} UI_Alignment;



typedef struct UI_Render_Node
{
    UI_Property_Flags ui_flags;

    // screen size and pos, not normalized
    vec2s pos;
    vec2s size;
    float rotation; // degrees, but gets converted to radians at draw time

    //rounded
    float rounded_radius; // 0-1 range

    //outline
    vec3s outline_color;
    float outline_thickness; // 0-1 :: ideally should be something small like 0.05-0.1


    //for circles
    float thickness; // size of the circle is determined by the pos and size36254

    //draw data
    //consider here what actually needs to be done for something to rendered, instead of passing in the entire config
    u32 texture_handle;
    vec2s uv_offset;
    vec2s uv_size;

    //colors
    vec3s color;
    vec3s background_color;

    // scissor data
    // vec2 scissor_pos;
    // vec2 scissor_size;
} UI_Render_Node;


typedef enum UI_Draw_Command_Type
{
    UI_DRAW_TYPE_DRAW, // everything is in one shader with lots of instancing
    UI_DRAW_TYPE_SCISSOR_START,
    UI_DRAW_TYPE_SCISSOR_END,
} UI_Draw_Command_Type;

typedef struct UI_Draw_Command
{
    UI_Draw_Command_Type type;
    u32 offset;
    u32 count;
    vec2s scissor_pos;
    vec2s scissor_size;
} UI_Draw_Command;


typedef struct UI_Render_Packet
{
    UI_Render_Node* ui_material_data;
    u64 ui_material_data_count;
    u64 ui_material_bytes;
    UI_Draw_Command* draw_command;
    u32 draw_command_count;
} UI_Render_Packet;


typedef struct Insanity_UI_Render_Packet
{
    UI_Draw_Command* draw_command;
    u32 draw_command_count;

    UI_Render_Node* material_data;
    u64 material_data_count;
    u64 material_bytes;
} Insanity_UI_Render_Packet;

void ui_add_draw_command(UI_Draw_Command* draw_command_array, u32 draw_command_array_count,
                         UI_Draw_Command_Type draw_type, vec2s scissor_pos, vec2s scissor_size)
{
    if (draw_command_array[draw_command_array_count].type == draw_type)
    {
        draw_command_array[draw_command_array_count].count++;
        draw_command_array[draw_command_array_count].scissor_pos = scissor_pos;
        draw_command_array[draw_command_array_count].scissor_size = scissor_size;
    }
    else
    {
        // we don't push empty commands
        if (draw_command_array[draw_command_array_count].count > 0)
        {
            draw_command_array_count++;
            draw_command_array[draw_command_array_count].count = 1; // one since we are adding a new draw
            draw_command_array[draw_command_array_count].offset = draw_command_array[draw_command_array_count - 1].
                offset + draw_command_array[draw_command_array_count - 1].count;
            draw_command_array[draw_command_array_count].type = draw_type;
            draw_command_array[draw_command_array_count].scissor_pos = scissor_pos;
            draw_command_array[draw_command_array_count].scissor_size = scissor_size;
        }
    }
}


#endif//UI_TYPES_H
