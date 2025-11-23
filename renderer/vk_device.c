#include "../renderer/vk_device.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "darray.h"
#include "logger.h"
#include "platform.h"
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

bool vulkan_instance_create(vulkan_context* vulkan_context)
{
    vulkan_context->allocator = 0;

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
    platform_get_vulkan_extension_names(&extensions_names_array);

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
    VK_CHECK(vkCreateInstance(&create_info, vulkan_context->allocator, &vulkan_context->instance));

    DEBUG("VULKAN INSTANCE CREATED");

    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug_callback;
    debug_create_info.pUserData = 0;

    //loading the function pointer
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
        vulkan_context->instance, "vkCreateDebugUtilsMessengerEXT");
    MASSERT_MSG(func, "Failed to create debug messenger!"); {
        //SAME THING: func == vkCreateDebugUtilsMessengerEXT
        VK_CHECK(func(vulkan_context->instance, &debug_create_info, NULL, &vulkan_context->debug_messenger));

    }
    DEBUG("VULKAN DEBUGGER CREATED");


    return true;
}


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                 VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                 void* user_data)
{
    switch (message_severity)
    {
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

bool vulkan_instance_destroy(vulkan_context* vulkan_context)
{
    INFO("VULKAN DESTROYING DEBUGGER");
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
        vulkan_context->instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
    {
        func(vulkan_context->instance, vulkan_context->debug_messenger, vulkan_context->allocator);
    }
    vkDestroyInstance(vulkan_context->instance, vulkan_context->allocator);
    INFO("VULKAN INSTANCED DESTROYED");
    return true;
}


bool vulkan_device_create(vulkan_context* vulkan_context)
{
    //query physical device
    if (!select_physical_device(vulkan_context))
    {
        return false;
    }

    //create logical device
    INFO("Creating logical device...");

    // NOTE: Do not create additional queues for shared indices.
    b8 present_shares_graphics_queue = vulkan_context->device.graphics_queue_index == vulkan_context->device.
                                       present_queue_index;
    b8 transfer_shares_graphics_queue = vulkan_context->device.graphics_queue_index == vulkan_context->device.
                                        transfer_queue_index;
    u32 index_count = 1;

    if (!present_shares_graphics_queue)
    {
        index_count++;
    }
    if (!transfer_shares_graphics_queue)
    {
        index_count++;
    }

    u32* indices = darray_create_reserve(u32, index_count);
    u8 index = 0;

    indices[index++] = vulkan_context->device.graphics_queue_index;
    if (!present_shares_graphics_queue)
    {
        indices[index++] = vulkan_context->device.present_queue_index;
    }
    if (!transfer_shares_graphics_queue)
    {
        indices[index++] = vulkan_context->device.transfer_queue_index;
    }

    //get device queue info
    VkDeviceQueueCreateInfo* queue_create_infos = darray_create_reserve(VkDeviceQueueCreateInfo, index_count);
    for (u32 i = 0; i < index_count; ++i)
    {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = indices[i];
        queue_create_infos[i].queueCount = 1;

        if (indices[i] == vulkan_context->device.graphics_queue_index)
        {
            queue_create_infos[i].queueCount = 2;
        }

        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = 0;

        f32 queue_priority = 1.0f;
        queue_create_infos[i].pQueuePriorities = &queue_priority;
    }


    // Request device features.
    // TODO: should be config driven
    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE; // Request anistrophy


    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = index_count;
    device_create_info.pQueueCreateInfos = queue_create_infos;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = 1;
    const char* extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    device_create_info.ppEnabledExtensionNames = &extension_names;

    // Deprecated
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = 0;


    // Create the device.
    VK_CHECK(vkCreateDevice(
        vulkan_context->device.physical_device,
        &device_create_info,
        vulkan_context->allocator,
        &vulkan_context->device.logical_device));

    INFO("Logical device created.");

    // Get queues.
    vkGetDeviceQueue(
        vulkan_context->device.logical_device,
        vulkan_context->device.graphics_queue_index,
        0,
        &vulkan_context->device.graphics_queue);

    vkGetDeviceQueue(
        vulkan_context->device.logical_device,
        vulkan_context->device.present_queue_index,
        0,
        &vulkan_context->device.present_queue);

    vkGetDeviceQueue(
        vulkan_context->device.logical_device,
        vulkan_context->device.transfer_queue_index,
        0,
        &vulkan_context->device.transfer_queue);

    INFO("Queues obtained.");


    //create the command pool for the graphics queue
    //each command pool is tied to its queue family
    VkCommandPoolCreateInfo pool_create_info = {};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_create_info.queueFamilyIndex = vulkan_context->device.graphics_queue_index;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(vulkan_context->device.logical_device,
        &pool_create_info, vulkan_context->allocator,
        &vulkan_context->graphics_command_pool));

    INFO("GRAPHICS COMMAND POOL CREATED.");


    darray_free(indices);
    darray_free(queue_create_infos);

    return TRUE;
}

bool vulkan_device_destroy(vulkan_context* vulkan_context)
{
    INFO("Vulkan family queues reset")
    vulkan_context->device.graphics_queue = 0;
    vulkan_context->device.present_queue = 0;
    vulkan_context->device.transfer_queue = 0;

    // Destroy logical device
    INFO("Destroying logical device...");
    if (vulkan_context->device.logical_device)
    {
        vkDestroyDevice(vulkan_context->device.logical_device, vulkan_context->allocator);
        vulkan_context->device.logical_device = 0;
    }

    // Physical devices are not destroyed.
    INFO("Physical device reset");
    vulkan_context->device.physical_device = 0;

    //TODO: this free crashes, look into later, when we got an allocator up and running
    darray_debug_header(vulkan_context->device.swapchain_capabilities.formats);

    if (vulkan_context->device.swapchain_capabilities.formats)
    {
        darray_free(vulkan_context->device.swapchain_capabilities.formats);
        vulkan_context->device.swapchain_capabilities.formats = 0;
        vulkan_context->device.swapchain_capabilities.format_count = 0;
    }


    if (vulkan_context->device.swapchain_capabilities.present_modes)
    {
        darray_free(vulkan_context->device.swapchain_capabilities.present_modes);
        vulkan_context->device.swapchain_capabilities.present_modes = 0;
        vulkan_context->device.swapchain_capabilities.present_mode_count = 0;
    }


    memset(&vulkan_context->device.swapchain_capabilities.capabilities, 0,
           sizeof(vulkan_context->device.swapchain_capabilities.capabilities));

    vulkan_context->device.graphics_queue_index = -1;
    vulkan_context->device.present_queue_index = -1;
    vulkan_context->device.transfer_queue_index = -1;

    INFO("VULKAN DEVICE DESTROYED");

    return true;

}


bool select_physical_device(vulkan_context* vulkan_context)
{
    //once for the count
    u32 physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(vulkan_context->instance, &physical_device_count, 0));
    if (physical_device_count == 0)
    {
        FATAL("No devices which support Vulkan were found.");
        return FALSE;
    }

    //twice for the devices
    VkPhysicalDevice* physical_devices = darray_create_reserve(VkPhysicalDevice, physical_device_count);
    VK_CHECK(vkEnumeratePhysicalDevices(vulkan_context->instance, &physical_device_count, physical_devices));
    for (u32 i = 0; i < physical_device_count; ++i)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        // TODO: These requirements should probably be driven by engine
        // configuration.
        vulkan_physical_device_requirements requirements = {};
        requirements.graphics = TRUE;
        requirements.present = TRUE;
        requirements.transfer = TRUE;
        // NOTE: Enable this if compute will be required.
        // requirements.compute = TRUE;
        requirements.sampler_anisotropy = TRUE;
        requirements.discrete_gpu = TRUE;
        requirements.device_extension_names = darray_create(const char*);
        darray_push(requirements.device_extension_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan_physical_device_queue_family_info queue_info = {};
        b8 result = physical_device_meets_requirements(
            physical_devices[i],
            vulkan_context->surface,
            &properties,
            &features,
            &requirements,
            &queue_info,
            &vulkan_context->device.swapchain_capabilities);

        if (result)
        {
            INFO("Selected device: '%s'.", properties.deviceName);
            // GPU type, etc.
            switch (properties.deviceType)
            {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    INFO("GPU type is Unknown.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    INFO("GPU type is Integrated.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    INFO("GPU type is Discrete.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    INFO("GPU type is Virtual.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    INFO("GPU type is CPU.");
                    break;
            }

            INFO(
                "GPU Driver version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.driverVersion),
                VK_VERSION_MINOR(properties.driverVersion),
                VK_VERSION_PATCH(properties.driverVersion));

            // Vulkan API version.
            INFO(
                "Vulkan API version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.apiVersion),
                VK_VERSION_MINOR(properties.apiVersion),
                VK_VERSION_PATCH(properties.apiVersion));

            // Memory information
            for (u32 j = 0; j < memory.memoryHeapCount; ++j)
            {
                f32 memory_size_gib = (((f32) memory.memoryHeaps[j].size) / GB(1));
                if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                {
                    INFO("Local GPU memory: %.2f GB", memory_size_gib);
                }
                else
                {
                    INFO("Shared System memory: %.2f GB", memory_size_gib);
                }
            }

            vulkan_context->device.physical_device = physical_devices[i];
            vulkan_context->device.graphics_queue_index = queue_info.graphics_family_index;
            vulkan_context->device.present_queue_index = queue_info.present_family_index;
            vulkan_context->device.transfer_queue_index = queue_info.transfer_family_index;
            // NOTE: set compute index here if needed.

            // Keep a copy of properties, features and memory info for later use.
            vulkan_context->device.properties = properties;
            vulkan_context->device.features = features;
            vulkan_context->device.memory = memory;
            break;
        }
    }

    // Ensure a device was selected
    if (!vulkan_context->device.physical_device)
    {
        M_ERROR("No physical devices were found which meet the requirements.");
        return FALSE;
    }

    INFO("Physical device selected.");
    return TRUE;
}

bool physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_info,
    vulkan_swapchain_capabilities_info* out_swapchain_support)
{
    // Evaluate device properties to determine if it meets the needs of our application.
    out_queue_info->graphics_family_index = -1;
    out_queue_info->present_family_index = -1;
    out_queue_info->compute_family_index = -1;
    out_queue_info->transfer_family_index = -1;

    // Discrete GPU?
    if (requirements->discrete_gpu)
    {
        if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            INFO("Device is not a discrete GPU, and one is required. Skipping.");
            return FALSE;
        }
    }

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);
    VkQueueFamilyProperties* queue_families = darray_create_reserve(VkQueueFamilyProperties, queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    // Look at each queue and see what queues it supports
    INFO("Graphics | Present | Compute | Transfer | Name");
    u8 min_transfer_score = 255;
    for (u32 i = 0; i < queue_family_count; ++i)
    {
        u8 current_transfer_score = 0;

        // Graphics queue?
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            out_queue_info->graphics_family_index = i;
            ++current_transfer_score;
        }

        // Compute queue?
        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            out_queue_info->compute_family_index = i;
            ++current_transfer_score;
        }

        // Transfer queue?
        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            // Take the index if it is the current lowest. This increases the
            // liklihood that it is a dedicated transfer queue.
            if (current_transfer_score <= min_transfer_score)
            {
                min_transfer_score = current_transfer_score;
                out_queue_info->transfer_family_index = i;
            }
        }

        // Present queue?
        VkBool32 supports_present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present));
        if (supports_present)
        {
            out_queue_info->present_family_index = i;
        }
    }

    // Print out some info about the device
    INFO("       %d |       %d |       %d |        %d | %s",
         out_queue_info->graphics_family_index != -1,
         out_queue_info->present_family_index != -1,
         out_queue_info->compute_family_index != -1,
         out_queue_info->transfer_family_index != -1,
         properties->deviceName);

    if (
        (!requirements->graphics || (requirements->graphics && out_queue_info->graphics_family_index != -1)) &&
        (!requirements->present || (requirements->present && out_queue_info->present_family_index != -1)) &&
        (!requirements->compute || (requirements->compute && out_queue_info->compute_family_index != -1)) &&
        (!requirements->transfer || (requirements->transfer && out_queue_info->transfer_family_index != -1)))
    {
        INFO("Device meets queue requirements.");
        TRACE("Graphics Family Index: %i", out_queue_info->graphics_family_index);
        TRACE("Present Family Index:  %i", out_queue_info->present_family_index);
        TRACE("Transfer Family Index: %i", out_queue_info->transfer_family_index);
        TRACE("Compute Family Index:  %i", out_queue_info->compute_family_index);

        // Query swapchain support.
        vulkan_device_query_swapchain_support(
            device,
            surface,
            out_swapchain_support);

        if (out_swapchain_support->format_count < 1 || out_swapchain_support->present_mode_count < 1)
        {
            if (out_swapchain_support->formats)
            {
                free(out_swapchain_support->formats);
            }
            if (out_swapchain_support->present_modes)
            {
                free(out_swapchain_support->present_modes);
            }
            INFO("Required swapchain support not present, skipping device.");
            return FALSE;
        }

        // Device extensions.
        if (requirements->device_extension_names)
        {
            u32 available_extension_count = 0;
            VkExtensionProperties* available_extensions = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(
                device,
                0,
                &available_extension_count,
                0));
            if (available_extension_count != 0)
            {
                available_extensions = _darray_create(sizeof(VkExtensionProperties) * available_extension_count,
                                                      sizeof(VkExtensionProperties));
                VK_CHECK(vkEnumerateDeviceExtensionProperties(
                    device,
                    0,
                    &available_extension_count,
                    available_extensions));

                u32 required_extension_count = darray_get_size(requirements->device_extension_names);
                for (u32 i = 0; i < required_extension_count; ++i)
                {
                    b8 found = FALSE;
                    for (u32 j = 0; j < available_extension_count; ++j)
                    {
                        if (strcmp(requirements->device_extension_names[i], available_extensions[j].extensionName))
                        {
                            found = TRUE;
                            break;
                        }
                    }

                    if (!found)
                    {
                        INFO("Required extension not found: '%s', skipping device.",
                             requirements->device_extension_names[i]);
                        darray_free(available_extensions);
                        return FALSE;
                    }
                }
            }
            darray_free(available_extensions);
        }

        // Sampler anisotropy
        if (requirements->sampler_anisotropy && !features->samplerAnisotropy)
        {
            INFO("Device does not support samplerAnisotropy, skipping.");
            return FALSE;
        }

        // Device meets all requirements.
        return TRUE;
    }

    return FALSE;
}


void vulkan_device_query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                                           vulkan_swapchain_capabilities_info* out_support_info)
{
    // Surface capabilities
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device,
        surface,
        &out_support_info->capabilities));

    // Surface formats
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device,
        surface,
        &out_support_info->format_count,
        0));

    if (out_support_info->format_count != 0)
    {
        out_support_info->formats = darray_create_reserve(VkSurfaceFormatKHR,
                                                          sizeof(VkSurfaceFormatKHR) * out_support_info->format_count);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device,
            surface,
            &out_support_info->format_count,
            out_support_info->formats));
    }

    // Present modes
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device,
        surface,
        &out_support_info->present_mode_count,
        0));
    if (out_support_info->present_mode_count != 0)
    {
        out_support_info->present_modes = darray_create_reserve(VkPresentModeKHR,
                                                                sizeof(VkPresentModeKHR*) * out_support_info->
                                                                format_count);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device,
            surface,
            &out_support_info->present_mode_count,
            out_support_info->present_modes));
    }
}

bool vulkan_device_detect_depth_format(vulkan_device* device)
{
    // Format candidates
    const u64 candidate_count = 3;
    //order we prefer them in
    VkFormat candidates[3] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    //try to find a suitable format
    u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for (u64 i = 0; i < candidate_count; ++i)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(device->physical_device, candidates[i], &properties);

        if ((properties.linearTilingFeatures & flags) == flags)
        {
            device->depth_format = candidates[i];
            return TRUE;
        }
        else if ((properties.optimalTilingFeatures & flags) == flags)
        {
            device->depth_format = candidates[i];
            return TRUE;
        }
    }


    return FALSE;
}
