#ifndef VULKAN_TYPES_H
#define VULKAN_TYPES_H

#include <vulkan/vulkan.h>
#include <defines.h>


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

    //family queues
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;


    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;


    VkFormat depth_format;
} vulkan_device;

typedef struct vulkan_context
{
    VkInstance instance;

    VkAllocationCallbacks* allocator;
    VkDebugUtilsMessengerEXT debugMessenger;

    //Surface
    VkSurfaceKHR surface;
    // The framebuffer's current width.
    u32 framebuffer_width;
    // The framebuffer's current height.
    u32 framebuffer_height;

    //Device
    vulkan_device device;

    //Swapchain
    vulkan_swapchain swapchain;
    u32 image_index;
    u32 current_frame;
    bool recreate_swapchain;
} vulkan_context;


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


#endif //VULKAN_TYPES_H
