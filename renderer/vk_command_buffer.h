#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "vulkan_struct_types.h"

// used by the renderer, the rest are helper functions
void vulkan_renderer_command_buffers_create(Vulkan_Context* vk_context);
void vulkan_renderer_command_buffer_destroy(Vulkan_Context* vk_context);


void vulkan_command_buffer_allocate(
    Vulkan_Context* context,
    VkCommandPool pool,
    Vulkan_Command_Buffer_Level cb_level,
    Vulkan_Command_Buffer* out_command_buffer);

void vulkan_command_buffer_reset(Vulkan_Command_Buffer* command_buffer);
void vulkan_command_buffer_free(
    Vulkan_Context* context,
    VkCommandPool pool,
    Vulkan_Command_Buffer* command_buffer);


void vulkan_command_buffer_begin(
    Vulkan_Command_Buffer* command_buffer,
    bool is_single_use,
    bool is_renderpass_continue,
    bool is_simultaneous_use);


void vulkan_command_buffer_end(Vulkan_Command_Buffer* command_buffer);


/**
 * Allocates and begins recording to out_command_buffer.
 */
void vulkan_command_buffer_allocate_and_begin_single_use(
    Vulkan_Context* context,
    VkCommandPool pool,
    Vulkan_Command_Buffer* out_command_buffer);

/**
 * Ends recording, submits to queue, does not free memory
 */
void vulkan_command_buffer_end_and_submit_single_use(
    Vulkan_Command_Buffer* command_buffer,
    VkQueue queue, VkSubmitInfo2* submit_info);

/**
 * Ends recording, submits to and waits for queue operation and frees the provided command buffer.
 */
void vulkan_command_buffer_end_and_submit_and_free_single_use(Vulkan_Context* context, VkCommandPool pool,
                                          Vulkan_Command_Buffer* command_buffer, VkQueue queue);


void vulkan_command_buffer_submit(Vulkan_Context* context, Vulkan_Command_Buffer* command_buffer, VkQueue queue);

void vulkan_command_buffer_submit_new(Vulkan_Context* context, Vulkan_Command_Buffer* command_buffer,
                                      VkQueue queue, VkSemaphoreSubmitInfo* wait_semaphore,
                                      VkSemaphoreSubmitInfo* signal_semaphore);

//TODO: rn will break compile, have to load in the funciton pointers
void vulkan_command_buffer_begin_debug_label(Renderer* renderer, Vulkan_Command_Buffer* command_buffer, const char* name);
void vulkan_command_buffer_end_debug_label(Renderer* renderer, Vulkan_Command_Buffer* command_buffer);


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
