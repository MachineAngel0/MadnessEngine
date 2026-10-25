#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "macros.glsl"



#define UI_FLAG_NONE BITFLAG(0)
#define UI_FLAG_BACKGROUND BITFLAG(1)
#define UI_FLAG_CLICKABLE BITFLAG(2)
#define UI_FLAG_IMAGE BITFLAG(3)
#define UI_FLAG_TEXT BITFLAG(4)
#define UI_FLAG_OUTLINE BITFLAG(5)
#define UI_FLAG_SCROLL_VIEW BITFLAG(6)
#define UI_FLAG_COLOR BITFLAG(7)
#define UI_FLAG_DRAGGABLE BITFLAG(8)
#define UI_FLAG_ROUND_CORNER BITFLAG(9)
#define UI_FLAG_CIRCLE BITFLAG(10)
#define UI_FLAG_SCROLL_FLOAT BITFLAG(11)
#define UI_FLAG_TEXT_INPUT BITFLAG(12)






struct UI_Data
{
    uint flags;

// screen size and pos, not normalized
    vec2 pos;
    vec2 size;
    float rotation;// degrees, but gets converted to radians at draw time

//rounded
    float rounded_radius;// 0-1 range

//outline
    vec3 outline_color;
    float outline_thickness;// 0-1 :: ideally should be something small like 0.05-0.1


//for circles
    float thickness;// size of the circle is determined by the pos and size

//texture
    uint texture_handle;
    vec2 uv_offset;
    vec2 uv_size;


    vec3 color;

    vec3 background_color;
};



layout(buffer_reference, scalar) readonly buffer UI_Instance_Buffer{
    UI_Data ui_data[];
};


layout(push_constant, scalar) uniform PushConstant_2D{
    UI_Instance_Buffer material_buffer;
    uint padding1;
    uint padding2;
}PC_2D;


// binding 0 stores our textures
// binding 1 stores our params and indexes into the descriptor
layout (set = 1, binding = 0) uniform sampler2D texture_samples[];
layout (set = 1, binding = 0) uniform texture2D textures[];
layout (set = 1, binding = 0) uniform texture2DMS texturesMS[];
layout (set = 1, binding = 0) uniform textureCube textureCubes[];
layout (set = 1, binding = 0) uniform texture2DArray textureArrays[];
//layout (set = 1, binding = 1) uniform sampler samplers[];

