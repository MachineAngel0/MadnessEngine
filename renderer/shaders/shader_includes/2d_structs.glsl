#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require


layout(push_constant, scalar) uniform PushConstant_2D{
    uint ubo_buffer_idx;
    uint instance_buffer_idx;
}PC_2D;

#define BITFLAG(x) (1 << (x))
#define SPRITE_FLAG_NONE BITFLAG(1)
#define SPRITE_FLAG_TEXTURE BITFLAG(2)
#define SPRITE_FLAG_COLOR BITFLAG(3)
#define SPRITE_FLAG_TEXT BITFLAG(4)
#define SPRITE_FLAG_CIRCLE BITFLAG(5)
#define SPRITE_FLAG_ROUND_CORNER BITFLAG(6)
#define SPRITE_FLAG_OUTLINE BITFLAG(7)


#define UI_TYPE_NONE BITFLAG(0)
#define UI_TYPE_BACKGROUND BITFLAG(1)
#define UI_TYPE_CLICKABLE BITFLAG(2)
#define UI_TYPE_IMAGE BITFLAG(3)
#define UI_TYPE_TEXT BITFLAG(4)
#define UI_TYPE_OUTLINE BITFLAG(5)
#define UI_TYPE_SCROLL BITFLAG(6)
#define UI_TYPE_COLOR BITFLAG(7)
#define UI_TYPE_DRAGGABLE BITFLAG(8)
#define UI_TYPE_ROUND_CORNER BITFLAG(9)
#define UI_TYPE_CIRCLE BITFLAG(10)

struct Sprite_Data {
    uint flags;

    vec2 pos;
    vec2 size;
    float rotation;

    float thickness;


    vec2 uv_offset;
    vec2 uv_size;

    vec3 color;
    uint texture_index;
    uint _padding0;
};

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


// binding 0 stores our textures
// binding 1 stores our params and indexes into the descriptor
layout (set = 1, binding = 0) uniform sampler2D texture_samples[];
layout (set = 1, binding = 0) uniform texture2D textures[];
layout (set = 1, binding = 0) uniform texture2DMS texturesMS[];
layout (set = 1, binding = 0) uniform textureCube textureCubes[];
layout (set = 1, binding = 0) uniform texture2DArray textureArrays[];
//layout (set = 1, binding = 1) uniform sampler samplers[];

// binding 2 stores all our buffers
layout(set = 2, binding = 0, scalar) readonly buffer SPRITE_INSTANCE_BUFFER{
    Sprite_Data sprite_instance_data[];
}Sprite_Instance_Buffer[];

layout(set = 2, binding = 0, scalar) readonly buffer UI_INSTANCE_BUFFER{
    UI_Data ui_instance_data[];
}UI_Instance_Buffer[];