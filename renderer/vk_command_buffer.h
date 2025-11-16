#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <stdbool.h>
#include <vulkan/vulkan.h>
#include "vulkan_types.h"


typedef struct Command_Buffer_Context
{
    VkCommandPool command_pool;
    VkCommandBuffer* command_buffer; // darray
} Command_Buffer_Context;


// used by the renderer, the rest are helper functions
void vulkan_renderer_command_buffers_create(vulkan_context* vk_context);
void vulkan_renderer_command_buffer_destroy(vulkan_context* vk_context);
;


void vulkan_command_buffer_allocate(
    vulkan_context* context,
    VkCommandPool pool,
    bool is_primary,
    vulkan_command_buffer* out_command_buffer);


void vulkan_command_buffer_free(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* command_buffer);


void vulkan_command_buffer_begin(
    vulkan_command_buffer* command_buffer,
    bool is_single_use,
    bool is_renderpass_continue,
    bool is_simultaneous_use);


void vulkan_command_buffer_end(vulkan_command_buffer* command_buffer);


/**
 * Allocates and begins recording to out_command_buffer.
 */
void vulkan_command_buffer_allocate_and_begin_single_use(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* out_command_buffer);

/**
 * Ends recording, submits to and waits for queue operation and frees the provided command buffer.
 */
void vulkan_command_buffer_end_single_use(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* command_buffer,
    VkQueue queue);


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
