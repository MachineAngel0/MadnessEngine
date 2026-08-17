#ifndef VULKAN_COMMAND_BUFFER_H
#define VULKAN_COMMAND_BUFFER_H

#include "vulkan_struct_types.h"

//just sketching it out for now
enum upload_intent { transfer_to_graphics, transfer_to_compute };

typedef struct Vulkan_Render_Queue
{
    Vulkan_Queue_Type type;

    VkCommandPool pool;

    Vulkan_Command_Buffer command_buffer[MAX_VULKAN_COMMAND_BUFFERS];
    const VkCommandBufferSubmitInfo* pCommandBufferInfos;
    uint32_t commandBufferInfoCount;
    //example: if the transfer and graphics are the same then, we simple dont do the transfer queue operations
    //might not be a bad idea to have something like this, helps synchronization

    const VkSemaphoreSubmitInfo* pWaitSemaphoreInfos;
    uint32_t waitSemaphoreInfoCount;
    const VkSemaphoreSubmitInfo* pSignalSemaphoreInfos;
    uint32_t signalSemaphoreInfoCount;

    //above creates the submit info,
    //but it makes sense that the command buffers hold onto the info,
    //and queue just gather them up
    // VkSubmitInfo2 submit_info;
} Vulkan_Render_Queue;

void vulkan_queue_flush_and_submit(Vulkan_Render_Queue* render_queue);


Vulkan_Command_Buffer_System* vulkan_command_buffer_system_init(Renderer* renderer);
bool vulkan_command_buffer_system_deinit(Renderer* renderer, Vulkan_Command_Buffer_System* cb_system);

/**
 * @note use like so:
 * Vulkan_Command_Buffer cb = NULL;
* vulkan_command_buffer_system_get_cb(..., ..., &cb);
 */

//we hand out the buffer and let the user manage it
bool vulkan_command_buffer_system_get_cb(Vulkan_Command_Buffer_System* system, Vulkan_Queue_Type type,
                                         Vulkan_Command_Buffer** out_cb);
bool vulkan_command_buffer_system_get_and_begin_cb(Vulkan_Command_Buffer_System* system,
                                                   Vulkan_Queue_Type type,
                                                   Vulkan_Command_Buffer** out_cb);

//ideally suppose to check semaphore to make buffers reusable
bool vulkan_command_buffer_system_update(Vulkan_Command_Buffer_System* system);

bool vulkan_command_buffer_allocate(
    Vulkan_Context* context,
    Vulkan_Command_Buffer* out_command_buffer,
    Vulkan_Command_Buffer_Level cb_level,
    VkCommandPool pool);

void vulkan_command_buffer_reset(Vulkan_Command_Buffer* command_buffer);

void vulkan_command_buffer_free(
    Vulkan_Context* context,
    Vulkan_Command_Buffer* command_buffer,
    VkCommandPool pool);

void vulkan_command_buffer_begin(Vulkan_Command_Buffer* command_buffer);

void vulkan_command_buffer_end(Vulkan_Command_Buffer* command_buffer);

void vulkan_command_buffer_submit_binary_semaphore(Vulkan_Context* context, Vulkan_Command_Buffer* command_buffer,
                                                   VkQueue queue, VkSemaphoreSubmitInfo* wait_semaphore,
                                                   VkSemaphoreSubmitInfo* signal_semaphore);

void vulkan_command_buffer_submit_generic(Vulkan_Context* context, Vulkan_Command_Buffer* command_buffer,
                                          VkQueue queue, VkSemaphoreSubmitInfo* wait_semaphore,
                                          VkSemaphoreSubmitInfo* signal_semaphore);

VkCommandBufferSubmitInfo vulkan_command_buffer_get_submit_info(Vulkan_Command_Buffer* command_buffer);

void vulkan_command_buffer_end_and_submit(Vulkan_Command_Buffer* command_buffer);


//TODO: rn will break compile, have to load in the funciton pointers
void vulkan_command_buffer_begin_debug_label(Renderer* renderer, Vulkan_Command_Buffer* command_buffer,
                                             const char* name);
void vulkan_command_buffer_end_debug_label(Renderer* renderer, Vulkan_Command_Buffer* command_buffer);


///////// OLD /////////

// used by the renderer, the rest are helper functions
void vulkan_renderer_command_buffers_create(Vulkan_Context* vk_context);
void vulkan_renderer_command_buffer_destroy(Vulkan_Context* vk_context);


void vulkan_command_buffer_begin_old(
    Vulkan_Command_Buffer* command_buffer,
    bool is_single_use,
    bool is_renderpass_continue,
    bool is_simultaneous_use);


/**
 * Allocates and begins recording to out_command_buffer.
 */
void vulkan_command_buffer_allocate_and_begin_single_use(
    Vulkan_Context* context,
    VkCommandPool pool,
    Vulkan_Command_Buffer* out_command_buffer);

/**
 * Ends recording, submits to and waits for queue operation and frees the provided command buffer.
 */
void vulkan_command_buffer_end_and_submit_and_free_single_use(Vulkan_Context* context, VkCommandPool pool,
                                                              Vulkan_Command_Buffer* command_buffer, VkQueue queue);


void vulkan_command_buffer_submit(Vulkan_Context* context, Vulkan_Command_Buffer* command_buffer, VkQueue queue);


//TODO:

/*COMMAND POOL*/
// void command_pool_allocate(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context);
// void command_pool_free(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context);


#endif
