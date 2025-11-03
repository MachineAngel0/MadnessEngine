#ifndef VK_DEVICE_H
#define VK_DEVICE_H

#include <stdbool.h>
#include <vulkan/vulkan.h>


const char* instance_extensions = {
    //VK_KHR_SURFACE_EXTENSION_NAME // this does not work it will cause the instance to fail
    //"VK_KHR_win32_surface",
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};
// const char* validationLayers = {
//     "VK_LAYER_KHRONOS_validation"
// };

// const char* device_extensions = {
//     VK_KHR_SWAPCHAIN_EXTENSION_NAME,
//     VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME
//     //VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME// I in fact do not use an amd gpu
// };

typedef struct Vulkan_Context
{
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    // VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice logical_device;

    //TODO: might want to move these elsewhere (maybe)
    VkQueue graphics_queue;
    VkQueue present_queue;
}Vulkan_Context;


// /* VULKAN INSTANCE*/


bool get_vulkan_api_version(uint32_t* apiVersion, uint32_t* variant,
                            uint32_t* major,uint32_t* minor, uint32_t* patch);

bool create_vk_instance(Vulkan_Context* vulkan_context);

bool vk_instance_shutdown(Vulkan_Context* vulkan_context, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator);

/*DEBUG MESSAGES/Validation Layers*/
VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);







#endif //VK_DEVICE_H
