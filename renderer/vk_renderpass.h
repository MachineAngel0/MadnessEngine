#ifndef VK_RENDERPASS_H
#define VK_RENDERPASS_H

#include "vulkan_struct_types.h"

//NOTE: idk why these are flags, a renderpass can only be one type
// unless its specifically to sort where draw objects go
typedef enum Vulkan_Renderpass_Type
{
    RENDERPASS_SHADOW_MAP = BITFLAG(0),
    RENDERPASS_FORWARD_LIGHT_CULL = BITFLAG(1), //this would not be a renderpass but a calcuation/compute shader
    RENDERPASS_FORWARD_OPAQUE = BITFLAG(1),
    RENDERPASS_TRANSPARENCY = BITFLAG(2),
    RENDERPASS_POST_PROCESSING = BITFLAG(3),
    RENDERPASS_UI = BITFLAG(4),
    RENDERPASS_REFLECTION = BITFLAG(5),
} Vulkan_Renderpass_Type;

typedef enum Vulkan_Attachment_Type
{
    //there are only 3 types of attachments
    Attachment_Type_Color,
    Attachment_Type_Depth_Stencil,
    // //NOTE: this kinda has to get resolved manually at some point
    // Attachment_Type_Resolve, //aka multisampling, usually on the color channel
} Vulkan_Attachment_Type;

typedef enum Vulkan_Attachment_Load_Op
{
    LOAD_OP_LOAD = 0, //load attachment
    LOAD_OP_CLEAR = 1, // clear, used during the first time it is accessed
    LOAD_OP_DONT_CARE = 2, // discard or dont use load previous values
    LOAD_OP_NONE = 1000400000,
} Vulkan_Attachment_Load_Op;

typedef enum Vulkan_Attachment_Store_Op
{
    STORE_OP_STORE = 0, //stores attachment for later use
    STORE_OP_DONT_CARE = 1, // discards attachment info
    STORE_OP_NONE = 1000400000,
} Vulkan_Attachment_Store_Op;

typedef struct Attachment
{
    //NOTE: as some point this is going to need to be conveyed to the graphics pipeline at creation, to be used
    const char* name;
    Vulkan_Attachment_Type type;
    u32 image_width;
    u32 image_height;

    //extra texture data needed for the renderpass data
    VkImageLayout layout;
    VkFormat format;

    //Texture Data
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;


    //NOTE: for VkRenderingAttachmentInfoKHR
    //Image_View
    //image layout
    // load op
    // store op
    // Clear Value
    // Depth Stencil Value
} Attachment;

typedef struct Attachment_Handle
{
    u32 id;
} Attachment_Handle;


typedef struct RenderPass_Resources
{
    Attachment_Handle attachment_handle;
    Vulkan_Attachment_Type type;
    Vulkan_Attachment_Load_Op load_op;
    Vulkan_Attachment_Store_Op store_op;
    bool resolve_msaa;
} RenderPass_Resources;


typedef struct RenderPass
{
    /*
     * renderpass
     *      name:
     *      type:
     * input:
     *      renderpass
     *      Attachment #:
     *          name:
     *          type:
     *          load:
     *          store:
     * output:
    *       Attachment #:
     *          name:
     *          type:
     *          load:
     *          store:
     *
     */
    const char* name; // TODO: should probably replace with String type
    Vulkan_Renderpass_Type type;
    RenderPass_Resources* inputs;
    RenderPass_Resources* outputs;

    float image_width;
    float image_height;
} RenderPass;


//Render Graph???
typedef struct Render_Graph
{
    // some arbitratry number used
    Attachment attachments[32];
    u32 attachments_count;

    RenderPass render_pass[32];
    u32 renderpass_count;
} Render_Graph;


//sudo forward renderer

//shadow -> depth attachment
// oqaque -> color, depth, shadowmap
// transparent -> same as oqapue but with blending
//post -> most likely just the color
//UI -> color, load op

void vulkan_renderpass_create_new(Vulkan_Context* context);


Attachment_Handle vulkan_create_attachment(Vulkan_Context* context, Render_Graph* render_graph, Vulkan_Attachment_Type type,
                                           VkFormat format, u32 width, u32 height);

//TODO: implement this function properly when needed
void vulkan_renderpass_insert_memory_barrier(Vulkan_Context* context, Vulkan_Command_Buffer* command_buffer);

//OLD API

void vulkan_renderpass_create(
    Vulkan_Context* context,
    Vulkan_Renderpass* out_renderpass,
    vec4s screen_pos, vec4s clear_color,
    f32 depth,
    u32 stencil);


void vulkan_renderpass_destroy(Vulkan_Context* context, Vulkan_Renderpass* renderpass);


void vulkan_renderpass_begin(Renderer* renderer, Vulkan_Command_Buffer* command_buffer, u32 current_frame);
void vulkan_renderpass_end(Renderer* renderer, Vulkan_Command_Buffer* command_buffer, u32 current_frame);


void vulkan_renderpass_UI_begin(Renderer* renderer, Vulkan_Command_Buffer* command_buffer, u32 current_frame);
void vulkan_renderpass_UI_end(Renderer* renderer, Vulkan_Command_Buffer* command_buffer, u32 current_frame);


void fake_renderpass_begin(void)
{
    //insert memory barriers needed
    //cmd beging rendering -> but we need rendering attachment info and rendering info struct
}


#endif //VK_RENDERPASS_H
