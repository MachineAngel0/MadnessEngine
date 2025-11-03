#include "../renderer/vk_device.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "darray.h"
#include "logger.h"
#include "platform.h"
#include "str.h"
#include "str_array.h"
#include "vulkan_types.h"


bool get_vulkan_api_version(uint32_t* apiVersion,
                            uint32_t* variant,
                            uint32_t* major,
                            uint32_t* minor,
                            uint32_t* patch)
{
    // gets the instance
    if (vkEnumerateInstanceVersion(apiVersion) == VK_SUCCESS)
    {
        *variant = VK_API_VERSION_VARIANT(*apiVersion);
        *major = VK_API_VERSION_MAJOR(*apiVersion);
        *minor = VK_API_VERSION_MINOR(*apiVersion);
        *patch = VK_API_VERSION_PATCH(*apiVersion);
        INFO("VULKAN VERSION: %d.%d.%d. VERSION VARIANT: %d", *major, *minor, *patch, *variant);
        return true;
    }

    return false;
}

bool create_vk_instance(Vulkan_Context* vulkan_context)
{

    uint32_t apiVersion;
    uint32_t variant;
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    // gets the instance
    if (!get_vulkan_api_version(&apiVersion, &variant, &major, &minor, &patch))
    {
        MASSERT_MSG(false, "failed to find api version!");
    }
    INFO("VULKAN VERSION: %d.%d.%d. VERSION VARIANT: %d", major, minor, patch, variant);


    VkApplicationInfo application_info = {};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = "MadnessEngine";
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName = "MadnessEngine";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    //application_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);
    //application_info.apiVersion = VK_API_VERSION_1_4;
    application_info.apiVersion = VK_MAKE_API_VERSION(0, major, minor, patch);


    /*
   typedef struct VkInstanceCreateInfo {
       VkStructureType sType;
       const void* pNext;
       VkInstanceCreateFlags flags;
       const VkApplicationInfo* pApplicationInfo;
       uint32_t enabledLayerCount;
       const char* const* ppEnabledLayerNames;
       uint32_t enabledExtensionCount;
       const char* const* ppEnabledExtensionNames;
   } VkInstanceCreateInfo;
   */

    //Getting extension names
    const char** extensions_names_array = darray_create(const char*);
    darray_push(extensions_names_array, &VK_KHR_SURFACE_EXTENSION_NAME);
    platform_get_extension_names(&extensions_names_array);


#if defined(DEBUG_BUILD)
    darray_push(extensions_names_array, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    DEBUG("Required extensions:");

    u32 length = darray_get_size(extensions_names_array);

    for (u32 i = 0; i < length; ++i)
    {
        DEBUG(extensions_names_array[i]);
    }
#endif

    //Validation layer extensions
    const char** validation_layers_names = 0;
    u32 validation_layers_count = 0;

#if defined(DEBUG_BUILD)

    INFO("Validation layers enabled. Enumerating...");


    validation_layers_names = darray_create(const char*);
    darray_push(validation_layers_names, &"VK_LAYER_KHRONOS_validation");
    validation_layers_count = darray_get_size(validation_layers_names);

    u32 available_layers_count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layers_count, NULL));
    VkLayerProperties* available_layers = darray_create_reserve(VkLayerProperties, available_layers_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layers_count, available_layers));

    for (u64 i = 0; i < validation_layers_count; i++)
    {
        INFO("Searching for layer: %s...", validation_layers_names[i]);
        bool layerFound = false;

        for (u32 j = 0; j < available_layers_count; j++)
        {
            if (strcmp(validation_layers_names[i], available_layers[j].layerName) == 0)
            {
                layerFound = true;
                INFO("FOUND.");

                break;
            }
        }

        if (!layerFound)
        {
            FATAL("Required validaton layer is missing: %s", validation_layers_names[i]);
            return false;
        }
    }
    INFO("All required validation layers are present.");
#endif

    //validation layers out messages
    // VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    // populateDebugMessengerCreateInfo(&debugCreateInfo);


    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.flags = 0;
    create_info.pApplicationInfo = &application_info;
    create_info.enabledExtensionCount = darray_get_size(extensions_names_array);
    create_info.ppEnabledExtensionNames = extensions_names_array;
    create_info.enabledLayerCount = validation_layers_count;
    create_info.ppEnabledLayerNames = validation_layers_names;


    create_info.pNext = 0;
    // create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;

    /*
        VkResult vkCreateInstance(
        const VkInstanceCreateInfo*                 pCreateInfo,
        const VkAllocationCallbacks*                pAllocator,
        VkInstance*                                 pInstance);
   */
    //TODO allocator
    VK_CHECK(vkCreateInstance(&create_info, NULL, &vulkan_context->instance));

    INFO("CREATE INSTANCE SUCCESS");

#if defined(DEBUG_BUILD)
    DEBUG("VULKAN INSTANCE CREATED");
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
    debug_create_info.sType =  VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                          VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug_callback;
    debug_create_info.pUserData = 0;

    //loading the function pointer
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
    vulkan_context->instance, "vkCreateDebugUtilsMessengerEXT");
    MASSERT_MSG(func,"Failed to create debug messenger!");
    {
        //SAME THING: func == vkCreateDebugUtilsMessengerEXT
        VK_CHECK(func(vulkan_context->instance, &debug_create_info, NULL, &vulkan_context->debugMessenger));
    }
    DEBUG("VULKAN DEBUGGER CREATED");

#endif

    return true;
}



VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data)
{
    switch (message_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            M_ERROR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            WARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            INFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            TRACE(callback_data->pMessage);
            break;
    }
    return VK_FALSE;
}



bool vk_instance_shutdown(Vulkan_Context* vulkan_context, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
         vulkan_context->instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
    {
        func(vulkan_context->instance, debugMessenger, pAllocator);
    }
}