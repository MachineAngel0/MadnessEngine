#ifndef VULKAN_ENUM_TYPES_H
#define VULKAN_ENUM_TYPES_H


#define AVAILABLE_TEXTURES 100


typedef enum Vulkan_Render_Pass_State
{
    READY,
    RECORDING,
    IN_RENDER_PASS,
    RECORDING_ENDED,
    SUBMITTED,
    NOT_ALLOCATED
} Vulkan_Render_Pass_State;

typedef enum Vulkan_Command_Buffer_State
{
    VULKAN_COMMAND_BUFFER_STATE_NOT_ALLOCATED,
    VULKAN_COMMAND_BUFFER_STATE_USABLE, //just allocated or reset
    VULKAN_COMMAND_BUFFER_STATE_BEGIN,
    VULKAN_COMMAND_BUFFER_STATE_END,
    VULKAN_COMMAND_BUFFER_STATE_SUBMITTED, // waiting on sync to reset/free
} Vulkan_Command_Buffer_State;

typedef enum Vulkan_Command_Buffer_Lifetime
{
    VULKAN_COMMAND_BUFFER_LIFETIME_PERSISTENT,

    //TODO: i should generally avoid this and just reuse them from a pool/freelist, not to be confused with a command pool
    VULKAN_COMMAND_BUFFER_LIFETIME_SINGLE_USE,

    //OPTIMIZE: this is more of an optimization but its not neccessary or even guaranteed to improve speeed
    // im refering to the command buffers allocated from a pool with the transient bit flag
    // there command buffers which are intended to be short lived and recycled often
    // VULKAN_COMMAND_BUFFER_LIFETIME_TRANSIENT,
} Vulkan_Command_Buffer_Lifetime;




typedef enum Vulkan_Command_Buffer_Level
{
    VULKAN_COMMAND_BUFFER_LEVEL_PRIMARY,
    VULKAN_COMMAND_BUFFER_LEVEL_SECONDARY,
} Vulkan_Command_Buffer_Level;


typedef enum Vulkan_Queue_Type
{
    //TODO: technically they can be on the same queue, so probably a bitflag at some point
    VULKAN_QUEUE_TYPE_GRAPHICS,
    VULKAN_QUEUE_TYPE_TRANSFER,
    VULKAN_QUEUE_TYPE_COMPUTE,
} Vulkan_Queue_Type;


typedef enum Vulkan_Buffer_Type
{
    BUFFER_TYPE_VERTEX, // meant to only be used with vkCmdBindVertexBuffers
    BUFFER_TYPE_INDEX, // meant to only be used as part of a vkCmdBindIndexBuffer or  vkCmdBindIndexBuffer2

    //create this if the storage is bieng used large data, buffers in descriptor sets, or for bda poitners
    BUFFER_TYPE_STORAGE_GPU,

    // meant to used as part of a vkCmdDrawIndirect, vkCmdDrawIndexedIndirect, vkCmdDrawMeshTasksIndirectNV, vkCmdDrawMeshTasksIndirectCountNV, vkCmdDrawMeshTasksIndirectEXT, vkCmdDrawMeshTasksIndirectCountEXT,
    BUFFER_TYPE_INDIRECT,
    BUFFER_TYPE_INDIRECT_HOST_VISIBLE, // for small buffers, mostly for things like ui draws

    //idk what these are used for yet, but they will probably be useful later
    //UNIFORM_TEXEL,
    //STORAGE_TEXEL,

    //NOTE: TYPES BELOW ARE BUFFERS MEANT TO BE USED UPDATABLE WITHOUT A STAING BUFFER

    //create this if the storage is used for  or if the data is relatively small and updates every frame, or static/rarely gets updated
    BUFFER_TYPE_STORAGE_CPU,
    BUFFER_TYPE_STAGING,
    BUFFER_TYPE_UNIFORM,
} Vulkan_Buffer_Type;


typedef enum Vulkan_Mesh_Data_Type
{
    VERTEX,
    INDEX,
    TANGENT,
    VERTEX_COLOR,
    NORMAL,
    UV,
    WEIGHT,
    JOINT,
} Vulkan_Mesh_Data_Type;


typedef enum Render_Mode
{
    RENDER_MODE_NONE,
    RENDER_MODE_NORMAL,
    RENDER_MODE_LIGHTING,
    // wireframe
    RENDER_MODE_MAX,
} Render_Mode;


#endif //VULKAN_ENUM_TYPES_H
