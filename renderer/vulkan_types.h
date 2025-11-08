#ifndef VULKAN_TYPES_H
#define VULKAN_TYPES_H

#include <vulkan/vulkan.h>
#include <defines.h>
#include "logger.h"

#define VK_CHECK(expr)              \
{                                   \
    if(expr != VK_SUCCESS){         \
        FATAL("VULKAN ERROR CODE: %d", expr);\
    }\
}


typedef struct vulkan_image
{
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    // VkSampler texture_sampler; // NOTE: should probably be here
    u32 width;
    u32 height;
} vulkan_image;

typedef enum vulkan_render_pass_state
{
    READY,
    RECORDING,
    IN_RENDER_PASS,
    RECORDING_ENDED,
    SUBMITTED,
    NOT_ALLOCATED
} vulkan_render_pass_state;

typedef struct vulkan_renderpass
{
    VkRenderPass handle;
    f32 x, y, w, h;
    f32 r, g, b, a;

    f32 depth;
    u32 stencil;

    vulkan_render_pass_state state;
} vulkan_renderpass;

typedef struct vulkan_framebuffer
{
    VkFramebuffer framebuffer_handle;
    u32 attachment_count;
    VkImageView* attachments;
    vulkan_renderpass* renderpass;
} vulkan_framebuffer;

typedef struct vulkan_swapchain
{
    //also contains VKformat
    VkSurfaceFormatKHR surface_format;
    u8 max_frames_in_flight;
    VkSwapchainKHR swapchain_handle;
    u32 image_count;

    VkImage* images;
    VkImageView* image_views;

    vulkan_image depth_attachment;

    // framebuffers used for on-screen rendering.
    vulkan_framebuffer* framebuffers;
} vulkan_swapchain;


typedef struct vulkan_swapchain_support_info
{
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
} vulkan_swapchain_support_info;

typedef struct vulkan_device
{
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    vulkan_swapchain_support_info swapchain_support;

    i32 graphics_queue_index;
    i32 present_queue_index;
    i32 transfer_queue_index;

    VkCommandPool graphics_command_pool;

    //family queues
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;


    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;


    VkFormat depth_format;
} vulkan_device;


typedef struct vulkan_physical_device_requirements
{
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;

    // darray
    const char** device_extension_names;

    b8 sampler_anisotropy;
    b8 discrete_gpu;
} vulkan_physical_device_requirements;


typedef struct vulkan_physical_device_queue_family_info
{
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
} vulkan_physical_device_queue_family_info;


typedef struct vk_fence
{
    VkFence fence_handle;
    b8 is_signaled;
} vulkan_fence;

typedef enum vulkan_command_buffer_state
{
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDER_PASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED
} vulkan_command_buffer_state;


typedef struct vulkan_command_buffer
{
    // VkCommandPool command_pool; // TODO:
    VkCommandBuffer command_buffer_handle;
    // Command buffer state.
    vulkan_command_buffer_state state;
} vulkan_command_buffer;


typedef struct vulkan_context
{
    bool is_init;


    VkInstance instance;

    VkAllocationCallbacks* allocator;
    VkDebugUtilsMessengerEXT debug_messenger;

    //Surface
    VkSurfaceKHR surface;
    // The framebuffer's current width and height.
    u32 framebuffer_width;
    u32 framebuffer_height;
    //if it does not match last gen, then make a new one
    u64 framebuffer_size_generation;
    u64 framebuffer_last_generation;
    //value holders for our framebuffer values
    u32 framebuffer_width_new;
    u32 framebuffer_height_new;

    //Device
    vulkan_device device;

    //Swapchain
    vulkan_swapchain swapchain;
    u32 image_index;
    u32 current_frame;
    bool recreating_swapchain;

    //renderpass
    vulkan_renderpass main_renderpass;

    //command buffers
    vulkan_command_buffer* graphics_command_buffers; // darray

    //Semaphores and Fences
    VkSemaphore* image_available_semaphores; // darray
    VkSemaphore* queue_complete_semaphores; // darray
    u32 in_flight_fence_count;
    vulkan_fence* in_flight_fences;

    // Holds pointers to fences which exist and are owned elsewhere.
    vulkan_fence** images_in_flight;
} vulkan_context;


#endif //VULKAN_TYPES_H
