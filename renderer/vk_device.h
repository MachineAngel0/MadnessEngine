#ifndef VK_DEVICE_H
#define VK_DEVICE_H

#include <stdbool.h>

#include <vulkan/vulkan.h>
#include "platform.h"
#include "vk_swapchain.h"


// const char* instance_extensions = {
//     //VK_KHR_SURFACE_EXTENSION_NAME // this does not work it will cause the instance to fail
//     //"VK_KHR_win32_surface",
//     VK_EXT_DEBUG_UTILS_EXTENSION_NAME
// };
// const char* validationLayers = {
//     "VK_LAYER_KHRONOS_validation"
// };

// const char* device_extensions = {
//     VK_KHR_SWAPCHAIN_EXTENSION_NAME,
//     VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME
//     //VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME// I in fact do not use an amd gpu
// };




/*** VULKAN INSTANCE ***/

bool get_vulkan_api_version(uint32_t* apiVersion, uint32_t* variant,
                            uint32_t* major, uint32_t* minor, uint32_t* patch);

bool vulkan_instance_create(vulkan_context* vulkan_context);

bool vulkan_instance_destroy(vulkan_context* vulkan_context);

/*DEBUG MESSAGES/Validation Layers*/
VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                 VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                 void* user_data);


/*** VULKAN Device ***/
bool vulkan_device_create(vulkan_context* vulkan_context);
bool vulkan_device_destroy(vulkan_context* vulkan_context);

/* SURFACE */
// void create_surface(Vulkan_Context* vulkan_context, platform_state* platform);

/* PHYSICAL DEVICE*/
bool select_physical_device(vulkan_context* vulkan_context);

bool physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_info,
    vulkan_swapchain_capabilities_info* out_swapchain_support);


void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    vulkan_swapchain_capabilities_info* out_support_info);

/*get depth format from device*/
bool vulkan_device_detect_depth_format(vulkan_device* device);



#endif //VK_DEVICE_H
