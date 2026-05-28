#ifndef VULKAN_ENUM_TYPES_H
#define VULKAN_ENUM_TYPES_H


#define AVAILABLE_TEXTURES 100


typedef enum vulkan_render_pass_state
{
    READY,
    RECORDING,
    IN_RENDER_PASS,
    RECORDING_ENDED,
    SUBMITTED,
    NOT_ALLOCATED
} vulkan_render_pass_state;


typedef enum vulkan_command_buffer_type
{
    COMMAND_BUFFER_TYPE_GRAPHICS,
    COMMAND_BUFFER_TYPE_TRANSFER,
    COMMAND_BUFFER_TYPE_COMPUTE,
} vulkan_command_buffer_type;


typedef enum Vulkan_Buffer_Type
{
    BUFFER_TYPE_INVALID,

    BUFFER_TYPE_VERTEX, // meant to only be used with vkCmdBindVertexBuffers
    BUFFER_TYPE_INDEX, // meant to only be used as part of a vkCmdBindIndexBuffer or  vkCmdBindIndexBuffer2

    //create this if the storage is bieng used large data
    BUFFER_TYPE_CPU_STORAGE, // meant to be used in a descriptor set
    // meant to used as part of a vkCmdDrawIndirect, vkCmdDrawIndexedIndirect, vkCmdDrawMeshTasksIndirectNV, vkCmdDrawMeshTasksIndirectCountNV, vkCmdDrawMeshTasksIndirectEXT, vkCmdDrawMeshTasksIndirectCountEXT,
    BUFFER_TYPE_INDIRECT,
    BUFFER_TYPE_INDIRECT_HOST_VISIBLE, // for small buffers, mostly for things like ui draws

    //idk what these are used for yet, but they will probably be useful later
    //UNIFORM_TEXEL,
    //STORAGE_TEXEL,

    //NOTE: TYPES BELOW ARE BUFFERS MEANT TO BE USED UPDATABLE WITHOUT A STAING BUFFER

    //create this if the storage is used for compute or if the data is relatively small and updates every frame, or static/rarely gets updated
    BUFFER_TYPE_GPU_STORAGE,
    BUFFER_TYPE_STAGING,
    BUFFER_TYPE_UNIFORM,
} Vulkan_Buffer_Type;


typedef enum Render_Mode
{
    RENDER_MODE_NONE,
    RENDER_MODE_NORMAL,
    RENDER_MODE_LIGHTING,
    RENDER_MODE_MAX,
} Render_Mode;


#endif //VULKAN_ENUM_TYPES_H
