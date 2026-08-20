#ifndef VK_DEVICE_H
#define VK_DEVICE_H


#include "vk_swapchain.h"
#include "vulkan_struct_types.h"

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

bool vulkan_instance_create(Vulkan_Context* vulkan_context, Renderer* renderer);

bool vulkan_instance_destroy(Vulkan_Context* vulkan_context, Renderer* renderer);

/*DEBUG MESSAGES/Validation Layers*/
VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                 VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                 void* user_data);


/*** VULKAN Device ***/


bool vulkan_device_create2(Renderer* renderer);
bool vulkan_device_destroy2(Renderer* renderer);
void vulkan_device_print_info(VkPhysicalDevice current_device, VkSurfaceKHR surface, Allocator* allocator);
bool vulkan_physical_device_meets_requirements(VkPhysicalDevice current_device, VkSurfaceKHR surface, Scratch_Allocator* scratch);
void vulkan_physical_device_get_supported_features(VkPhysicalDevice current_device, VkPhysicalDeviceFeatures2* out_features);


void vulkan_physical_device_find_transfer_queue(Vulkan_Physical_Device_Heuristic* device_heuristic,
                                                u32 queue_family_count, Array* queue_families);
void vulkan_physical_device_find_compute_queue(Vulkan_Physical_Device_Heuristic* device_heuristic, u32 queue_family_count, Array* queue_families);





/* SURFACE */
// void create_surface(Vulkan_Context* vulkan_context, platform_state* platform);


void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    Vulkan_Swapchain_Capabilities_Info* out_support_info);

/*get depth format from device*/
bool vulkan_device_detect_depth_stencil_format(Renderer* renderer);
bool vulkan_device_detect_depth_format(Renderer* renderer);



#endif //VK_DEVICE_H
