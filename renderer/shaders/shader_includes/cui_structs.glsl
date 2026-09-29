#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "macros.glsl"

/* // TODO: define the render flags at some point
#define MESH_PIPELINE_COLOR  BITFLAG(1)
#define MESH_PIPELINE_NORMAL  BITFLAG(2)
#define MESH_PIPELINE_EMISSIVE  BITFLAG(3)
#define MESH_PIPELINE_ROUGHNESS  BITFLAG(4)
#define MESH_PIPELINE_METALLIC  BITFLAG(5)
#define MESH_PIPELINE_AO  BITFLAG(6)
*/

//render item
struct CUI_Data
{
    uint render_type;

    vec2 pos;
    vec2 size;
    float rotation;

//rounded rect
    float rounded_corners; // maybe a vec4 at some point

//text
    uint texture_idx;
    vec2 uv_offset;
    vec2 uv_size;



//circle
    float radius;


    vec4 color;
    float outline_thickness;
    vec4 outline_color;

// vec4s gradient[4]; // this would replace all the color param

};

layout(buffer_reference, scalar) readonly buffer CUI_Data_Buffer
{
    CUI_Data cui_data[];
};


layout(push_constant, scalar) uniform PushConstant_CUI
{
    CUI_Data_Buffer cui_ssbo;
    uint padding1;
    uint padding2;
} PC_CUI;


