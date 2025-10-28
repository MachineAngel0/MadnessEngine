//
// Created by Adams Humbert on 9/6/2025.
//

#ifndef VK_DEVICE_H
#define VK_DEVICE_H

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>




const std::vector<const char *> instance_extensions = {
    //VK_KHR_SURFACE_EXTENSION_NAME // this does not work it will cause the instance to fail
    //"VK_KHR_win32_surface",
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char *> device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME
    //VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME// I in fact do not use an amd gpu
};

struct Vulkan_Context
{
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice logical_device;

    //TODO: might want to move these elsewhere (maybe)
    VkQueue graphics_queue;
    VkQueue present_queue;
};




/* VULKAN INSTANCE*/
void create_vk_instance(Vulkan_Context& vulkan_context);

bool get_vulkan_api_version(uint32_t& variant, uint32_t& major, uint32_t& minor, uint32_t& patch);


/*DEBUG MESSAGES/Validation Layers*/
void create_vk_debug_messanger(VkInstance& instance, VkDebugUtilsMessengerEXT& debugMessenger);

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugUtilsMessengerEXT* pDebugMessenger);
VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                       void* pUserData);


void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator);

//ensure we have validation layer support
bool ensure_validation_layer_support();







#endif //VK_DEVICE_H
