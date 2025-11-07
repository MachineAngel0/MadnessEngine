

#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <vulkan/vulkan.h>
#include "vulkan_types.h"



typedef struct Command_Buffer_Context
{
    VkCommandPool command_pool;
    VkCommandBuffer* command_buffer; // darray
}Command_Buffer_Context;


//TODO: refactor

/*COMMAND POOL*/
// void command_pool_allocate(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context);
// void command_pool_free(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context);


/*COMMAND BUFFER*/
// void command_buffer_allocate(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context, uint32_t frames_in_flight);
// void command_buffer_free(vulkan_context* vulkan_context);
// VkCommandBuffer command_buffer_begin_single_use(vulkan_context* vulkan_context, VkCommandPool* command_pool);
// void command_buffer_end_single_use(vulkan_context* vulkan_context, VkCommandPool* command_pool, VkCommandBuffer commandBuffer);



#endif
