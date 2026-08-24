#ifndef VULKAN_COMMAND_BUFFER_H
#define VULKAN_COMMAND_BUFFER_H

#include "vk_sync.h"
#include "vulkan_struct_types.h"


Vulkan_Queue_System* vulkan_queue_system_init(Renderer* renderer);
bool vulkan_queue_system_deinit(Renderer* renderer, Vulkan_Queue_System* queue_system);

void vulkan_queue_frame_begin(Renderer* renderer, u32 current_frame);
void vulkan_queue_frame_end(Renderer* renderer, u32 current_frame, u32 image_index);

void vulkan_queue_system_flush_queues(Renderer* renderer, Vulkan_Queue_System* queue_system);

void vulkan_queue_system_graphics_fence_wait(Renderer* renderer, u32 current_frame);



/**
 * @note use like so:
 * Vulkan_Command_Buffer cb = NULL;
* vulkan_command_buffer_system_get_cb(..., ..., &cb);
 */

//we hand out the buffer and let the user manage it

bool vulkan_queue_system_get_cb(Renderer* renderer, Vulkan_Queue_Type type,
                                Vulkan_Command_Buffer** out_cb);

/**
 * @note: this should never fail, if we grab the per frame graphics buffer
 */
bool vulkan_queue_system_get_graphics_command_buffer(Renderer* renderer, Vulkan_Command_Buffer** out_cb);
/**
 * @note: will begin the command buffer for you, will end at the end of the frame
 */
bool vulkan_queue_system_get_transfer_command_buffer(Renderer* renderer, Vulkan_Command_Buffer** out_cb);
void vulkan_queue_system_get_transfer_semaphore_value(Renderer* renderer, u64* out_semaphore_value);
VkSemaphore vulkan_queue_system_get_transfer_semaphore(Renderer* renderer);


bool vulkan_queue_add_signal_semaphore(Renderer* renderer, Vulkan_Queue_Type queue_type,
                                       VkSemaphoreSubmitInfo submit_info);
bool vulkan_queue_add_wait_semaphore(Renderer* renderer, Vulkan_Queue_Type queue_type,
                                     VkSemaphoreSubmitInfo submit_info);

//TODO: these should really get batched up and submitted at the end
bool vulkan_command_flush_barriers(Vulkan_Command_Buffer* command_buffer);
bool vulkan_command_add_image_barrier(Vulkan_Command_Buffer* command_buffer,
                                      VkImageMemoryBarrier2 image_memory_barrier);
bool vulkan_command_add_buffer_barrier(Vulkan_Command_Buffer* command_buffer,
                                       VkBufferMemoryBarrier2 buffer_memory_barrier);


//ideally suppose to check semaphore to make buffers reusable
bool vulkan_command_buffer_system_update(Vulkan_Queue_System* system);

bool vulkan_command_buffer_allocate(
    Vulkan_Command_Buffer* out_command_buffer,
    Vulkan_Command_Buffer_Level cb_level,
    VkCommandPool pool, Renderer* renderer);

void vulkan_command_buffer_reset(Vulkan_Command_Buffer* command_buffer);

void vulkan_command_buffer_free(
    Renderer* renderer,
    Vulkan_Command_Buffer* command_buffer, VkCommandPool pool);

void vulkan_command_buffer_begin(Vulkan_Command_Buffer* command_buffer);

void vulkan_command_buffer_end(Vulkan_Command_Buffer* command_buffer);


VkCommandBufferSubmitInfo vulkan_command_buffer_get_submit_info(Vulkan_Command_Buffer* command_buffer);


void vulkan_command_buffer_debug_label_begin(Renderer* renderer, Vulkan_Command_Buffer* command_buffer,
                                             const char* name);
void vulkan_command_buffer_debug_label_color_begin(Renderer* renderer, Vulkan_Command_Buffer* command_buffer,
                                             const char* name, float color[4]);
void vulkan_command_buffer_debug_label_end(Renderer* renderer, Vulkan_Command_Buffer* command_buffer);

//these aren't the right design
void vulkan_command_buffer_submit_binary_semaphore(Renderer* renderer,
                                                   Vulkan_Command_Buffer* command_buffer, VkQueue queue,
                                                   VkSemaphoreSubmitInfo* wait_semaphore,
                                                   VkSemaphoreSubmitInfo* signal_semaphore);

void vulkan_command_buffer_submit_generic(Renderer* renderer,
                                          Vulkan_Command_Buffer* command_buffer, VkQueue queue,
                                          VkSemaphoreSubmitInfo* wait_semaphore,
                                          VkSemaphoreSubmitInfo* signal_semaphore);

s32 vulkan_get_queue_family_index(Renderer* renderer,
                                  Vulkan_Queue_Type queue1)
{
    switch (queue1)
    {
    case VULKAN_QUEUE_TYPE_GRAPHICS:
        return renderer->graphics_queue_index;
    case VULKAN_QUEUE_TYPE_TRANSFER:
        return renderer->transfer_queue_index;
    case VULKAN_QUEUE_TYPE_COMPUTE:
        return renderer->compute_queue_index;
    }

    MASSERT_FALSE();
    return -1;
}

bool vulkan_is_same_queue_family(Renderer* renderer,
                                 Vulkan_Queue_Type queue1,
                                 Vulkan_Queue_Type queue2)
{
    s32 queue1_index = -1;
    s32 queue2_index = -1;
    switch (queue1)
    {
    case VULKAN_QUEUE_TYPE_GRAPHICS:
        queue1_index = renderer->graphics_queue_index;
        break;
    case VULKAN_QUEUE_TYPE_TRANSFER:
        queue1_index = renderer->transfer_queue_index;
        break;
    case VULKAN_QUEUE_TYPE_COMPUTE:
        queue1_index = renderer->compute_queue_index;
        break;
    }

    switch (queue2)
    {
    case VULKAN_QUEUE_TYPE_GRAPHICS:
        queue2_index = renderer->graphics_queue_index;
        break;
    case VULKAN_QUEUE_TYPE_TRANSFER:
        queue2_index = renderer->transfer_queue_index;
        break;
    case VULKAN_QUEUE_TYPE_COMPUTE:
        queue2_index = renderer->compute_queue_index;
        break;
    }

    return queue1_index == queue2_index;
}

void vulkan_queue_add_ownership_transfer(Renderer* renderer,
                                         Vulkan_Queue_Type from_queue,
                                         Vulkan_Queue_Type to_queue,
                                         VkSemaphoreSubmitInfo signal,
                                         VkSemaphoreSubmitInfo wait,
                                         VkDependencyInfo from_dependency_info,
                                         VkDependencyInfo to_dependency_info)
{
    Vulkan_Command_Buffer* from_buffer;
    vulkan_queue_system_get_cb(renderer, from_queue, &from_buffer);
    Vulkan_Command_Buffer* to_buffer;
    vulkan_queue_system_get_cb(renderer, to_queue, &to_buffer);


    //pipeline barrier per resource (basically per texture upload, or whatever else we are doing)
    vkCmdPipelineBarrier2(from_buffer->handle, &from_dependency_info);
    vkCmdPipelineBarrier2(to_buffer->handle, &to_dependency_info);

    vulkan_queue_add_signal_semaphore(renderer, from_queue, signal);


    vulkan_queue_add_wait_semaphore(renderer, to_queue, wait);
}


void queue_ownership_transfer(Renderer* renderer, Vulkan_Queue_Type from_queue, Vulkan_Command_Buffer* from_buffer,
                              Vulkan_Queue_Type to, Vulkan_Command_Buffer* to_buffer)
{
    //all queue transfers need a release from the source queue and acquire from the destination queue

    //TODO: rn just pretend that the dependency information is filled out

    VkSemaphoreSubmitInfo signal = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .value = 0, // TODO: semaphore value, we add it when we submit our queue
        .stageMask = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT,
    };


    VkDependencyInfo from_dependency_info = {0};
    from_dependency_info.sType;
    from_dependency_info.pNext;
    from_dependency_info.dependencyFlags;
    from_dependency_info.memoryBarrierCount;
    from_dependency_info.pMemoryBarriers;
    from_dependency_info.bufferMemoryBarrierCount;
    from_dependency_info.pBufferMemoryBarriers;
    from_dependency_info.imageMemoryBarrierCount;
    from_dependency_info.pImageMemoryBarriers;

    vkCmdPipelineBarrier2(from_buffer->handle, &from_dependency_info);

    VkSemaphoreSubmitInfo wait = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .value = 0, // TODO: semaphore value, we add it when we submit our queue
        .stageMask = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT,
    };


    VkDependencyInfo to_dependency_info = {0};
    to_dependency_info.sType;
    to_dependency_info.pNext;
    to_dependency_info.dependencyFlags;
    to_dependency_info.memoryBarrierCount;
    to_dependency_info.pMemoryBarriers;
    to_dependency_info.bufferMemoryBarrierCount;
    to_dependency_info.pBufferMemoryBarriers;
    to_dependency_info.imageMemoryBarrierCount;
    to_dependency_info.pImageMemoryBarriers;

    vkCmdPipelineBarrier2(to_buffer->handle, &to_dependency_info);

    //NOTE: eventually we submit both to their respective queue's
    // vkQueueSubmit2()
}


//NOTE: use this only for startup code, this causes a queuewaitidle
/**
 * Allocates and begins recording to out_command_buffer.
 */
void vulkan_command_buffer_begin_single_use(
    Renderer* renderer,
    VkCommandPool pool, Vulkan_Command_Buffer* out_command_buffer);

/**
 * Ends recording, submits to and waits for queue operation and frees the provided command buffer.
 */
void vulkan_command_buffer_end_single_use(Renderer* renderer,
                                          VkCommandPool pool, Vulkan_Command_Buffer* command_buffer, VkQueue queue);


//TODO:

/*COMMAND POOL*/
// void command_pool_allocate(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context);
// void command_pool_free(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context);


#endif
