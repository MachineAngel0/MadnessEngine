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
    darray_push(extensions_names_array, &VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

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

    //validation extensions
    VkValidationFeaturesEXT validation_features_info = {0};
    bool validation_ext_enabled = false;

#ifndef NDEBUG
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

    VkValidationFeatureEnableEXT enable_features[2] = {
        VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
        VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
    };

    validation_features_info.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
    validation_features_info.enabledValidationFeatureCount = 2;
    validation_features_info.pEnabledValidationFeatures    = enable_features;
    validation_ext_enabled = true;

#endif

    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &application_info;
    create_info.enabledExtensionCount = darray_get_size(extensions_names_array);
    create_info.ppEnabledExtensionNames = extensions_names_array;
    create_info.enabledLayerCount = validation_layers_count;
    create_info.ppEnabledLayerNames = validation_layers_names;

    create_info.pNext = 0;

    //TODO: enable if you want extra info, its very slow
    // if (validation_ext_enabled)
    // {
    //     create_info.pNext = &validation_features_info;
    // }
    // create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;

    /*
        VkResult vkCreateInstance(
        const VkInstanceCreateInfo*                 pCreateInfo,
        const VkAllocationCallbacks*                pAllocator,
        VkInstance*                                 pInstance);
   */
    VK_CHECK(vkCreateInstance(&create_info, vulkan_context->allocator, &vulkan_context->instance));

    //create the debugger
#ifndef NDEBUG

    DEBUG("VULKAN INSTANCE CREATED");

    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};
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
#endif


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
            // M_ERROR("%d Validation Layer: Error %s: %s", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
            M_ERROR("%s: %s", callback_data->pMessageIdName, callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            WARN("%s: %s", callback_data->pMessageIdName, callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            INFO("%s: %s", callback_data->pMessageIdName, callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            TRACE("%s: %s", callback_data->pMessageIdName, callback_data->pMessage);
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
    hash_set* indices = hash_set_init(sizeof(i32), 10);
    i32* index_array = darray_create_reserve(i32, 10);
    if (hash_set_insert(indices, &vulkan_context->device.graphics_queue_index))
    {
        darray_push(index_array, vulkan_context->device.graphics_queue_index);
    };
    if (hash_set_insert(indices, &vulkan_context->device.present_queue_index))
    {
        darray_push(index_array, vulkan_context->device.present_queue_index);
    };
    if (hash_set_insert(indices, &vulkan_context->device.compute_queue_index))
    {
        darray_push(index_array, vulkan_context->device.compute_queue_index);
    }
    if (hash_set_insert(indices, &vulkan_context->device.transfer_queue_index))
    {
        darray_push(index_array, vulkan_context->device.transfer_queue_index);
    }

    u64 index_count = hash_set_get_size(indices);
    u64 index_array_size = darray_get_size(index_array);
    // i32 hash_set_contains_index(const hash_set* h, void* key)

    f32 default_queue_priority = 1.0f;
    //get device queue info for each unique queue family
    VkDeviceQueueCreateInfo* queue_create_infos = darray_create_reserve(VkDeviceQueueCreateInfo, index_count);
    for (u64 i = 0; i < index_count; ++i)
    {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = index_array[i];
        queue_create_infos[i].queueCount = 1;

        if (index_array[i] == vulkan_context->device.graphics_queue_index)
        {
            queue_create_infos[i].queueCount = 2;
        }

        queue_create_infos[i].pQueuePriorities = &default_queue_priority;

        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = 0;
    }


    //prints out all extensions available
    /*
    uint32_t extCount = 0;
    vkEnumerateDeviceExtensionProperties(vulkan_context->device.physical_device, NULL, &extCount, NULL);

    VkExtensionProperties* extensions = malloc(extCount * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(vulkan_context->device.physical_device, NULL, &extCount, extensions);

    printf("\n=== Supported Device Extensions ===\n");
    for (uint32_t i = 0; i < extCount; i++) {
        printf("  %s (v%u)\n", extensions[i].extensionName, extensions[i].specVersion);
    }
    printf("Total: %u extensions\n\n", extCount);

    free(extensions);*/

    //device extensions
    const char* extension_names[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // VK_EXT_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME // not supported
        VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, //promoted in 1.2
        // VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, // doesnt work
        //for buffer device addressing
        // VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME, // doesnt work on my hardware
        // VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        VK_KHR_SHADER_RELAXED_EXTENDED_INSTRUCTION_EXTENSION_NAME,
    };

    // Enable only specific Vulkan 1.3 features
    //will be plugged into the device create infos pNext pointer

    VkPhysicalDeviceShaderRelaxedExtendedInstructionFeaturesKHR relaxed_shader_extension = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_RELAXED_EXTENDED_INSTRUCTION_FEATURES_KHR,
        .shaderRelaxedExtendedInstruction = VK_TRUE,
        .pNext = NULL,
    };

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT enable_extended_dynamic_state_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT,
        .extendedDynamicState = VK_TRUE,
        .pNext = &relaxed_shader_extension,
    };


    // VkPhysicalDeviceVulkan14Features enable_vulkan14_features = {
    // .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
    // };

    VkPhysicalDeviceVulkan13Features enable_vulkan13_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &enable_extended_dynamic_state_features,
        .synchronization2 = VK_TRUE,
        .dynamicRendering = VK_TRUE,
    };
    VkPhysicalDeviceVulkan12Features enable_vulkan12_features =
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .scalarBlockLayout = VK_TRUE,
        .descriptorIndexing = VK_TRUE,
        .runtimeDescriptorArray = VK_TRUE,
        .descriptorBindingPartiallyBound = VK_TRUE,
        .descriptorBindingSampledImageUpdateAfterBind  = VK_TRUE,
        .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
        .bufferDeviceAddress = VK_TRUE,
        .bufferDeviceAddressMultiDevice = VK_TRUE,
        .pNext = &enable_vulkan13_features,
    };
    VkPhysicalDeviceVulkan11Features enable_vulkan11_features =
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = &enable_vulkan12_features,
    };

    // Request device features.
    // TODO: should be config driven
    VkPhysicalDeviceFeatures device_features = {
        .samplerAnisotropy = VK_TRUE // Request anistrophy
    };

    VkPhysicalDeviceFeatures2 enable_device_features2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &enable_vulkan11_features,
        .features = device_features,
    };

    vkGetPhysicalDeviceFeatures2(vulkan_context->device.physical_device, &enable_device_features2);
    vkGetPhysicalDeviceFeatures(vulkan_context->device.physical_device, &device_features);



    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &enable_device_features2,
        .queueCreateInfoCount = index_count,
        .pQueueCreateInfos = queue_create_infos,
        .pEnabledFeatures = NULL, // do not use is pNext is used
        .enabledExtensionCount = ARRAY_SIZE(extension_names),
        .ppEnabledExtensionNames = extension_names,
        // Deprecated
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
    };



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
        vulkan_context->device.compute_queue_index,
        0,
        &vulkan_context->device.compute_queue);

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
    VkCommandPoolCreateInfo pool_create_info = {0};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_create_info.queueFamilyIndex = vulkan_context->device.graphics_queue_index;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(vulkan_context->device.logical_device,
        &pool_create_info, vulkan_context->allocator,
        &vulkan_context->graphics_command_pool));

    INFO("GRAPHICS COMMAND POOL CREATED.");

    vkDeviceWaitIdle(vulkan_context->device.logical_device);

    darray_free(index_array);
    darray_free(queue_create_infos);
    hash_set_free(indices);

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
    VK_CHECK(vkEnumeratePhysicalDevices(vulkan_context->instance, &physical_device_count, NULL));
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
        vkGetPhysicalDeviceProperties(physical_devices[i], &vulkan_context->device.properties);

        vkGetPhysicalDeviceFeatures(physical_devices[i], &vulkan_context->device.features);

        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &vulkan_context->device.memory);

        // TODO: These requirements should probably be driven by engine
        // configuration.
        vulkan_physical_device_requirements requirements = {};
        requirements.graphics = TRUE;
        requirements.present = TRUE;
        requirements.transfer = TRUE;
        requirements.compute = TRUE;

        requirements.sampler_anisotropy = TRUE;
        requirements.discrete_gpu = TRUE;
        requirements.device_extension_names = darray_create(const char*);
        darray_push(requirements.device_extension_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan_physical_device_queue_family_info queue_info = {};
        b8 result = physical_device_meets_requirements(
            physical_devices[i],
            vulkan_context->surface,
            &vulkan_context->device.properties,
            &vulkan_context->device.features,
            &requirements,
            &queue_info,
            &vulkan_context->device.swapchain_capabilities);

        if (result)
        {
            INFO("Selected device: '%s'.", &vulkan_context->device.properties.deviceName);
            // GPU type, etc.
            switch (vulkan_context->device.properties.deviceType)
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
                VK_VERSION_MAJOR(vulkan_context->device.properties.driverVersion),
                VK_VERSION_MINOR(vulkan_context->device.properties.driverVersion),
                VK_VERSION_PATCH(vulkan_context->device.properties.driverVersion));

            // Vulkan API version.
            INFO(
                "Vulkan API version: %d.%d.%d",
                VK_VERSION_MAJOR(vulkan_context->device.properties.apiVersion),
                VK_VERSION_MINOR(vulkan_context->device.properties.apiVersion),
                VK_VERSION_PATCH(vulkan_context->device.properties.apiVersion));

            // Memory information
            for (u32 j = 0; j < vulkan_context->device.memory.memoryHeapCount; ++j)
            {
                f32 memory_size_gib = (((f32) vulkan_context->device.memory.memoryHeaps[j].size) / GB(1));
                if (vulkan_context->device.memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
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
            vulkan_context->device.compute_queue_index = queue_info.compute_family_index;
            // NOTE: set compute index here if needed.

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

    //TODO: rn its only picking a queue with each available queue type, later if performance is needed,
    // then using a specialized queue, like for compute or transfer would be better

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
                available_extensions = darray_create_reserve(VkExtensionProperties, available_extension_count);
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

//were not using this one
bool vulkan_device_detect_depth_format(vulkan_device* device)
{
    // Format candidates
    const u64 candidate_count = 3;
    //order we prefer them in
    VkFormat candidates[] = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
    };
    //try to find a suitable format
    u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for (u64 i = 0; i < candidate_count; ++i)
    {
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(device->physical_device, candidates[i], &format_properties);

        if ((format_properties.linearTilingFeatures & flags) == flags)
        {
            device->depth_format = candidates[i];
            return TRUE;
        }
        else if ((format_properties.optimalTilingFeatures & flags) == flags)
        {
            device->depth_format = candidates[i];
            return TRUE;
        }
    }


    return FALSE;
}


bool vulkan_device_detect_depth_stencil_format(vulkan_device* device)
{
    // Format candidates
    const u64 candidate_count = 3;
    //order we prefer them in
    VkFormat candidates[] = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
    };
    //try to find a suitable format
    u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for (u64 i = 0; i < candidate_count; ++i)
    {
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(device->physical_device, candidates[i], &format_properties);

        if ((format_properties.linearTilingFeatures & flags) == flags)
        {
            device->depth_format = candidates[i];
            return TRUE;
        }
        else if ((format_properties.optimalTilingFeatures & flags) == flags)
        {
            device->depth_format = candidates[i];
            return TRUE;
        }
    }


    return FALSE;
}
