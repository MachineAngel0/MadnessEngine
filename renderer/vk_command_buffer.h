#ifndef VULKAN_COMMAND_BUFFER_H
#define VULKAN_COMMAND_BUFFER_H

#include "vk_sync.h"
#include "vulkan_struct_types.h"


Vulkan_Queue_System* vulkan_queue_system_init(Renderer* renderer);
bool vulkan_queue_system_deinit(Renderer* renderer, Vulkan_Queue_System* queue_system);


void vulkan_queue_system_flush_queues(Renderer* renderer, Vulkan_Queue_System* queue_system);

void vulkan_queue_system_wait_on_frame_compute(Renderer* renderer, u32 current_frame)
{
    Vulkan_Queue_System* queue_system = renderer->queue_system;
    if (!vulkan_fence_wait(&renderer->context,
                           &queue_system->comptute_render_queue.compute_frame_fence[current_frame],
                           UINT64_MAX))
    {
        WARN("COMPUTE START FRAME: In-flight fence wait failure! ");
        return;
    }
}

void vulkan_queue_system_wait_on_frame_graphics(Renderer* renderer, u32 current_frame)
{
    Vulkan_Queue_System* queue_system = renderer->queue_system;
    if (!vulkan_fence_wait(
        &renderer->context,
        &queue_system->graphics_render_queue.frame_submit_fence[current_frame],
        UINT64_MAX))
    {
        WARN("GRAPHICS START FRAME: In-flight fence wait failure!");
        return;
    }
}

void vulkan_queue_frame_end(Renderer* renderer, u32 current_frame, u32 image_index)
{
    Scratch_Allocator scratch = scratch_allocator_begin(&renderer->allocator);

    //TODO: assume we have a sync point here before executing the rest here

    //TODO: for graphics/present add the swapchain semaphores
    Vulkan_Queue_System* queue_system = renderer->queue_system;
    Vulkan_Graphics_Queue* graphics_queue = &renderer->queue_system->graphics_render_queue;


    Vulkan_Command_Buffer* graphics_cb = &graphics_queue->graphics_command_buffer[current_frame];


    VkCommandBufferSubmitInfo cb_submit_info = vulkan_command_buffer_get_submit_info(graphics_cb);


    VkSubmitInfo2 submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.pNext = 0;
    submit_info.flags = 0;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = &cb_submit_info;
    submit_info.waitSemaphoreInfoCount = graphics_queue->wait_semaphore_info_count;
    submit_info.pWaitSemaphoreInfos = graphics_queue->wait_semaphore_info;
    submit_info.signalSemaphoreInfoCount = graphics_queue->signal_semaphore_info_count;
    submit_info.pSignalSemaphoreInfos = graphics_queue->signal_semaphore_info;


    VkSemaphoreSubmitInfo swapchain_wait_semaphore = {0};
    swapchain_wait_semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    swapchain_wait_semaphore.semaphore = graphics_queue->swapchain_wait_semaphore[image_index];
    swapchain_wait_semaphore.value = 0; // not needed for binary semaphores
    swapchain_wait_semaphore.stageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSemaphoreSubmitInfo swapchain_signal_semaphore = {0};

    swapchain_wait_semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    swapchain_wait_semaphore.semaphore = graphics_queue->swapchain_signal_semaphore[image_index];

    VkSubmitInfo2 swapchain_submit_info = {0};
    swapchain_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    swapchain_submit_info.pNext = 0;
    swapchain_submit_info.flags = 0;
    swapchain_submit_info.commandBufferInfoCount = 1;
    swapchain_submit_info.pCommandBufferInfos = &cb_submit_info;
    swapchain_submit_info.waitSemaphoreInfoCount = 1;
    swapchain_submit_info.pWaitSemaphoreInfos = &swapchain_wait_semaphore;
    swapchain_submit_info.signalSemaphoreInfoCount = 1;
    swapchain_submit_info.pSignalSemaphoreInfos = &swapchain_signal_semaphore;


    VkSubmitInfo2 final_submit[] = {
        submit_info,
        swapchain_submit_info,
    };

    VkResult result = vkQueueSubmit2(queue_system->graphics_queue, ARRAY_SIZE(final_submit), final_submit,
                                     graphics_queue->frame_submit_fence[current_frame]);

    VK_CHECK(result);
    scratch_allocator_end(scratch);


    // Give the image back to the swapchain.
    vulkan_swapchain_present_image(
        renderer,
        &renderer->context,
        &renderer->context.swapchain,
        renderer->context.present_queue,
        renderer->context.swapchain_release_semaphore[image_index], image_index);






}


/**
 * @note use like so:
 * Vulkan_Command_Buffer cb = NULL;
* vulkan_command_buffer_system_get_cb(..., ..., &cb);
 */

//we hand out the buffer and let the user manage it
bool vulkan_queue_system_get_cb(Vulkan_Queue_System* system, Vulkan_Queue_Type type,
                                Vulkan_Command_Buffer** out_cb);
bool vulkan_queue_system_get_and_begin_cb(Vulkan_Queue_System* system,
                                          Vulkan_Queue_Type type,
                                          Vulkan_Command_Buffer** out_cb);


bool vulkan_queue_get_aync_command_buffer(Vulkan_Queue_System* system, Vulkan_Queue_Type type,
                                          Vulkan_Command_Buffer** out_cb);


//ideally suppose to check semaphore to make buffers reusable
bool vulkan_command_buffer_system_update(Vulkan_Queue_System* system);

bool vulkan_command_buffer_allocate(Vulkan_Context* context,
                                    Vulkan_Command_Buffer* out_command_buffer,
                                    Vulkan_Command_Buffer_Level cb_level,
                                    VkCommandPool pool);

void vulkan_command_buffer_reset(Vulkan_Command_Buffer* command_buffer);

void vulkan_command_buffer_free(Vulkan_Context* context,
                                Vulkan_Command_Buffer* command_buffer,
                                VkCommandPool pool);

void vulkan_command_buffer_begin(Vulkan_Command_Buffer* command_buffer);

void vulkan_command_buffer_end(Vulkan_Command_Buffer* command_buffer);

bool vulkan_command_buffer_add_semaphore(Vulkan_Command_Buffer* cb,
                                         VkSemaphoreSubmitInfo submit_info,
                                         Vulkan_Semaphore_Submit_Type submit_type);


VkCommandBufferSubmitInfo vulkan_command_buffer_get_submit_info(Vulkan_Command_Buffer* command_buffer);


void vulkan_command_buffer_begin_debug_label(Renderer* renderer, Vulkan_Command_Buffer* command_buffer,
                                             const char* name);
void vulkan_command_buffer_end_debug_label(Renderer* renderer, Vulkan_Command_Buffer* command_buffer);

//these aren't the right design
void vulkan_command_buffer_submit_binary_semaphore(Vulkan_Context* context, Vulkan_Command_Buffer* command_buffer,
                                                   VkQueue queue, VkSemaphoreSubmitInfo* wait_semaphore,
                                                   VkSemaphoreSubmitInfo* signal_semaphore);

void vulkan_command_buffer_submit_generic(Vulkan_Context* context, Vulkan_Command_Buffer* command_buffer,
                                          VkQueue queue, VkSemaphoreSubmitInfo* wait_semaphore,
                                          VkSemaphoreSubmitInfo* signal_semaphore);


void vulkan_command_buffer_end_and_submit(Vulkan_Command_Buffer* command_buffer);


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


///////// OLD /////////

// used by the renderer, the rest are helper functions
void vulkan_renderer_command_buffers_create(Vulkan_Context* vk_context);
void vulkan_renderer_command_buffer_destroy(Vulkan_Context* vk_context);


void vulkan_command_buffer_begin_old(Vulkan_Command_Buffer* command_buffer,
                                     bool is_single_use,
                                     bool is_renderpass_continue,
                                     bool is_simultaneous_use);


/**
 * Allocates and begins recording to out_command_buffer.
 */
void vulkan_command_buffer_allocate_and_begin_single_use(Vulkan_Context* context,
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
