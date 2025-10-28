

#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <vector>
#include <vulkan/vulkan.h>


struct Vulkan_Context;

struct Command_Buffer_Context
{
    VkCommandPool command_pool;
    std::vector<VkCommandBuffer> command_buffer;
};

/*COMMAND POOL*/
void command_pool_allocate(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context);
void command_pool_free(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context);


/*COMMAND BUFFER*/
void command_buffer_allocate(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, uint32_t frames_in_flight);
void command_buffer_free(Vulkan_Context& vulkan_context);
VkCommandBuffer command_buffer_begin_single_use(Vulkan_Context& vulkan_context, VkCommandPool& command_pool);
void command_buffer_end_single_use(Vulkan_Context& vulkan_context, VkCommandPool& command_pool, VkCommandBuffer commandBuffer);



#endif //COMMAND_BUFFER_H
