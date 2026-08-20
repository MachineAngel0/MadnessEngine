#include "vk_device.h"
#include "array.h"
#include "platform.h"
#include "hash_set.h"
#include "math_lib.h"

bool get_vulkan_api_version(u32* apiVersion,
                            u32* variant,
                            u32* major,
                            u32* minor,
                            u32* patch)
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

bool vulkan_instance_create(Renderer* renderer)
{
    renderer->vulkan_allocator = 0;

    u32 apiVersion;
    u32 variant;
    u32 major;
    u32 minor;
    u32 patch;
    // gets the instance
    if (!get_vulkan_api_version(&apiVersion, &variant, &major, &minor, &patch))
    {
        MASSERT_MSG(false, "failed to find api version!");
    }


    if (minor < 4)
    {
        FATAL("Device must support vulkan 1.4 minimum");
        return false;
    }


    VkApplicationInfo application_info = {};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pApplicationName = "MadnessEngine";
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName = "MadnessEngine";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    //application_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);
    // application_info.apiVersion = VK_API_VERSION_1_4; // we explicitly rn want 1.4, might move down to 1.3
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

    if (app_is_debug_build())
    {
        //add debug utils
        darray_push(extensions_names_array, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        DEBUG("Required extensions:");

        u32 length = darray_get_size(extensions_names_array);

        for (u32 i = 0; i < length; ++i)
        {
            DEBUG(extensions_names_array[i]);
        }
    }

    //Validation layer extensions
    const char** validation_layers_names = 0;
    u32 validation_layers_count = 0;

    //validation extensions
    VkValidationFeaturesEXT validation_features_info = {0};
    bool validation_ext_enabled = false;

    if (app_is_debug_build())
    {
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

        VkValidationFeatureEnableEXT enable_features[3] = {
            //TODO: enable if you want extra info on the gpu but its very slow
            // VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
            // VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
            VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
        };

        validation_features_info.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        validation_features_info.enabledValidationFeatureCount = ARRAY_SIZE(enable_features);
        validation_features_info.pEnabledValidationFeatures = enable_features;
        validation_ext_enabled = true;
    }

    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &application_info;
    create_info.enabledExtensionCount = darray_get_size(extensions_names_array);
    create_info.ppEnabledExtensionNames = extensions_names_array;
    create_info.enabledLayerCount = validation_layers_count;
    create_info.ppEnabledLayerNames = validation_layers_names;

    create_info.pNext = 0;

    // if (validation_ext_enabled)
    // {
    //     create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&validation_features_info;
    // }

    /*
        VkResult vkCreateInstance(
        const VkInstanceCreateInfo*                 pCreateInfo,
        const VkAllocationCallbacks*                pAllocator,
        VkInstance*                                 pInstance);
   */
    VK_CHECK(vkCreateInstance(&create_info, renderer->vulkan_allocator, &renderer->instance));

    //create the debugger
    if (app_is_debug_build())
    {
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
        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            renderer->instance, "vkCreateDebugUtilsMessengerEXT");
        MASSERT_MSG(func, "Failed to create debug messenger!");
        {
            //SAME THING: func == vkCreateDebugUtilsMessengerEXT
            VK_CHECK(func(renderer->instance, &debug_create_info, NULL, &renderer->debug_messenger));
        }
        DEBUG("VULKAN DEBUGGER CREATED");
    }


    return true;
}


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                 VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                 void* user_data)
{
    switch (message_severity)
    {
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
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
        TRACE("%s: %s", callback_data->pMessageIdName, callback_data->pMessage);
        break;
    }
    return VK_FALSE;
}

bool vulkan_instance_destroy(Renderer* renderer)
{
    INFO("VULKAN DESTROYING DEBUGGER");
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        renderer->instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
    {
        func(renderer->instance, renderer->debug_messenger, renderer->vulkan_allocator);
    }
    vkDestroyInstance(renderer->instance, renderer->vulkan_allocator);
    INFO("VULKAN INSTANCED DESTROYED");
    return true;
}

void vulkan_physical_device_find_transfer_queue(Vulkan_Physical_Device_Heuristic* device_heuristic,
                                                u32 queue_family_count, Array* queue_families)
{
    for (u32 queue_index = 0; queue_index < queue_family_count; queue_index++)
    {
        VkQueueFamilyProperties queue_family_properties = array_get(queue_families, VkQueueFamilyProperties,
                                                                    queue_index);


        VkQueueFlags flags = queue_family_properties.queueFlags;

        bool graphics = (flags & VK_QUEUE_GRAPHICS_BIT);
        bool compute = (flags & VK_QUEUE_COMPUTE_BIT);
        bool transfer = (flags & VK_QUEUE_TRANSFER_BIT);


        if (transfer && !compute && !graphics)
        {
            device_heuristic->dedicated_transfer = true;
            device_heuristic->transfer_queue_index = queue_index;
            return;
        }
    }

    //second best options is a transfer queue with no graphics, but compute is fine
    for (u32 queue_index = 0; queue_index < queue_family_count; queue_index++)
    {
        VkQueueFamilyProperties queue_family_properties = array_get(queue_families, VkQueueFamilyProperties,
                                                                    queue_index);
        VkQueueFlags flags = queue_family_properties.queueFlags;

        bool graphics = (flags & VK_QUEUE_GRAPHICS_BIT);
        bool compute = (flags & VK_QUEUE_COMPUTE_BIT);
        bool transfer = (flags & VK_QUEUE_TRANSFER_BIT);


        if (transfer && compute && !graphics)
        {
            device_heuristic->dedicated_transfer_compute_sharing = true;
            device_heuristic->dedicated_transfer = false;
            device_heuristic->dedicated_compute = false;
            device_heuristic->transfer_queue_index = queue_index;
            device_heuristic->compute_queue_index = queue_index;
            return;
        }
    }


    //if we dont find a dedicated transfer, we fall back to the first available
    if (device_heuristic->dedicated_transfer == false)
    {
        for (u32 queue_index = 0; queue_index < queue_family_count; queue_index++)
        {
            VkQueueFamilyProperties queue_family_properties = array_get(queue_families, VkQueueFamilyProperties,
                                                                        queue_index);

            if (queue_family_properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                device_heuristic->transfer_queue_index = queue_index;
                return;
            }
        }
    }
}

void vulkan_physical_device_find_compute_queue(Vulkan_Physical_Device_Heuristic* device_heuristic,
                                               u32 queue_family_count,
                                               Array* queue_families)
{
    //find a dedicated compute queue (without graphics). Doesn't matter if it shares an index with the transfer queue
    for (u32 queue_index = 0; queue_index < queue_family_count; queue_index++)
    {
        VkQueueFamilyProperties queue_family_properties = array_get(queue_families, VkQueueFamilyProperties,
                                                                    queue_index);


        VkQueueFlags flags = queue_family_properties.queueFlags;

        bool graphics = (flags & VK_QUEUE_GRAPHICS_BIT);
        bool compute = (flags & VK_QUEUE_COMPUTE_BIT);
        // bool transfer = (flags & VK_QUEUE_TRANSFER_BIT);


        if (compute && !graphics)
        {
            if (queue_index == device_heuristic->transfer_queue_index)
            {
                device_heuristic->dedicated_transfer_compute_sharing = true;
                device_heuristic->dedicated_transfer = false;
                device_heuristic->dedicated_compute = false;
            }
            else
            {
                device_heuristic->dedicated_compute = true;
            }
            device_heuristic->compute_queue_index = queue_index;


            return;
        }
    }

    //there are instances where the queue lives on a seperate index from the initial transfer queue, and does not have a transfer queue
    if (device_heuristic->dedicated_transfer && !device_heuristic->dedicated_compute)
    {
        VkQueueFamilyProperties queue_family_properties = array_get(queue_families, VkQueueFamilyProperties,
                                                                    device_heuristic->transfer_queue_index);
        //try to share with transfer but not graphics
        if (queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT &&
            !(queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
            device_heuristic->dedicated_transfer_compute_sharing = true;
            device_heuristic->dedicated_transfer = false;
            device_heuristic->dedicated_compute = false;
            device_heuristic->compute_queue_index = device_heuristic->transfer_queue_index;
            return;
        }
    }


    // fall back to the first available
    if (!device_heuristic->dedicated_compute && !device_heuristic->dedicated_transfer_compute_sharing)
    {
        for (u32 queue_index = 0; queue_index < queue_family_count; queue_index++)
        {
            VkQueueFamilyProperties queue_family_properties = array_get(queue_families, VkQueueFamilyProperties,
                                                                        queue_index);

            if (queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                device_heuristic->compute_queue_index = queue_index;
                return;
            }
        }
    }
}

void vulkan_physical_device_find_graphics_and_present_queue(Renderer* renderer,
                                                            Vulkan_Physical_Device_Heuristic* device_heuristic,
                                                            VkPhysicalDevice current_device,
                                                            u32 queue_family_count,
                                                            Array* queue_families)
{
    for (u32 queue_index = 0; queue_index < queue_family_count; queue_index++)
    {
        VkQueueFamilyProperties queue_family_properties = array_get(queue_families, VkQueueFamilyProperties,
                                                                    queue_index);


        //skip any dedicated queues
        if ((device_heuristic->dedicated_transfer || device_heuristic->dedicated_transfer_compute_sharing) &&
            device_heuristic->transfer_queue_index == queue_index)
        {
            continue;
        }

        //skip any dedicated queues
        if ((device_heuristic->dedicated_compute || device_heuristic->dedicated_transfer_compute_sharing) &&
            device_heuristic->compute_queue_index == queue_index)
        {
            continue;
        }


        // try to find a queue with graphics and present
        VkBool32 supports_present = VK_FALSE;
        VkResult present_result = vkGetPhysicalDeviceSurfaceSupportKHR(
            current_device, queue_index, renderer->surface,
            &supports_present);
        VK_CHECK(present_result)

        if ((queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && supports_present == VK_TRUE)
        {
            device_heuristic->graphics_queue = queue_index;
            device_heuristic->present_queue = queue_index;
            return;
        }
    }

    //if we don't find a queue family for graphics and present, use the first available
    if (device_heuristic->graphics_queue == UINT32_MAX)
    {
        for (u32 queue_index = 0; queue_index < queue_family_count; queue_index++)
        {
            VkQueueFamilyProperties queue_family_properties = array_get(queue_families, VkQueueFamilyProperties,
                                                                        queue_index);

            if (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                device_heuristic->graphics_queue = queue_index;
                break;
            }
        }
    }

    if (device_heuristic->present_queue != UINT32_MAX)
    {
        for (u32 queue_index = 0; queue_index < queue_family_count; queue_index++)
        {
            // try to find a queue with graphics and present
            VkBool32 supports_present = VK_FALSE;
            VkResult present_result = vkGetPhysicalDeviceSurfaceSupportKHR(
                current_device, queue_index, renderer->surface,
                &supports_present);
            VK_CHECK(present_result)

            if (supports_present == VK_TRUE)
            {
                device_heuristic->present_queue = queue_index;
                break;
            }
        }
    }
}

bool vulkan_device_create2(Renderer* renderer)
{
    //process for physical device
    // scan all our physical devices and get all of them which support our requirements
    // from there scan throught and try to get a dedicated queue family for graphics, tranfer, and compute, the most gets the highest score and we selected that one
    // and in general set all the information needed


    //once for the count
    //twice for the devices
    //

    u32 physical_device_count = 0;
    vkEnumeratePhysicalDevices(renderer->instance, &physical_device_count, NULL);
    if (physical_device_count == 0)
    {
        FATAL("No devices which support Vulkan were found.");
        return false;
    }
    //keep this allocated until the renderer free's it
    VkPhysicalDevice* physical_devices = darray_create_reserve(VkPhysicalDevice, physical_device_count);
    VK_CHECK(vkEnumeratePhysicalDevices(renderer->instance, &physical_device_count, physical_devices));


    ARRAY_TYPE(VkPhysicalDevice)* valid_physical_device = array_create(Vulkan_Physical_Device_Suitable,
                                                                       physical_device_count,
                                                                       &renderer->allocator); // TODO: scratch alloc


    // TODO: These requirements should probably be driven by engine configuration.

    Vulkan_Physical_Device_Requirements requirements = {0};
    requirements.graphics = true;
    requirements.present = true;
    requirements.transfer = true;
    requirements.compute = true;
    requirements.sampler_anisotropy = true;


    //scan each device and return a list of all the ones that supports everything we want

    DEBUG("Number of Physical Devices: %d", physical_device_count);

    for (u32 device_idx = 0; device_idx < physical_device_count; device_idx++)
    {
        DEBUG("Physical Devices# 1: %d", device_idx);
        VkPhysicalDevice current_device = physical_devices[device_idx];


        vulkan_device_print_info(current_device, renderer->surface, &renderer->allocator);

        Scratch_Allocator scratch = scratch_allocator_begin(&renderer->allocator);
        if (vulkan_physical_device_meets_requirements(current_device, renderer->surface, &scratch))
        {
            Vulkan_Physical_Device_Suitable temp = {
                .physical_device = current_device,
                .physical_device_index = device_idx,
            };
            array_push(valid_physical_device, &temp);
        }
        scratch_allocator_end(scratch);
    }

    if (array_is_empty(valid_physical_device))
    {
        FATAL("NO VULKAN DEVICE WHICH MEETS OUR REQUIREMENTS");
        return false;
    }


    // at this point we score our devices
    ARRAY_TYPE(Vulkan_Physical_Device_Heuristic)* heuristic_selection = array_create(Vulkan_Physical_Device_Heuristic,
        valid_physical_device->num_items,
        &renderer->allocator);
    Vulkan_Physical_Device_Heuristic default_heuristic = {
        .dedicated_transfer = false,
        .transfer_queue_index = INT32_MAX,
        .dedicated_compute = false,
        .compute_queue_index = INT32_MAX,
        .graphics_queue = INT32_MAX,
        .present_queue = INT32_MAX,
        .score = 0,
    };
    array_fill_up_to_capacity(heuristic_selection, &default_heuristic);


    Vulkan_Physical_Device_Heuristic selected_device_heuristic = {0};
    VkPhysicalDevice best_device = {0};
    s32 best_score = -1;
    u32 device_index = -1;


    for (u32 physical_device_idx = 0; physical_device_idx < valid_physical_device->num_items; physical_device_idx++)
    {
        Vulkan_Physical_Device_Suitable physical_device_suitable = array_get(
            valid_physical_device, Vulkan_Physical_Device_Suitable, physical_device_idx);
        VkPhysicalDevice current_device = physical_device_suitable.physical_device;
        Vulkan_Physical_Device_Heuristic device_heuristic = array_get(heuristic_selection,
                                                                      Vulkan_Physical_Device_Heuristic,
                                                                      physical_device_idx);

        u32 queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(current_device, &queue_family_count, 0);
        ARRAY_TYPE(VkQueueFamilyProperties)* queue_families = array_create(
            VkQueueFamilyProperties, queue_family_count, &renderer->allocator);
        vkGetPhysicalDeviceQueueFamilyProperties(current_device, &queue_family_count,
                                                 queue_families->data);
        queue_families->num_items = queue_family_count;


        //find a transfer queue
        vulkan_physical_device_find_transfer_queue(&device_heuristic, queue_family_count,
                                                   queue_families);
        //find a compute queue
        vulkan_physical_device_find_compute_queue(&device_heuristic, queue_family_count, queue_families);

        //find a graphics queue with present support, that doesn't conflict with dedicated transfer and compute queues
        vulkan_physical_device_find_graphics_and_present_queue(renderer, &device_heuristic, current_device,
                                                               queue_family_count,
                                                               queue_families);
        //score our heuristic
        if (device_heuristic.dedicated_transfer)
        {
            device_heuristic.score++;
        }
        if (device_heuristic.dedicated_compute)
        {
            device_heuristic.score++;
        }
        if (device_heuristic.dedicated_transfer_compute_sharing)
        {
            device_heuristic.score += 2;
        }

        //see if its our new best device
        DEBUG("device score: %d, best score: %d",
              device_heuristic.score,
              best_score);
        if (device_heuristic.score > best_score)
        {
            best_device = current_device;
            device_index = physical_device_suitable.physical_device_index;
            best_score = device_heuristic.score;
            selected_device_heuristic = device_heuristic;
        }
    }

    DEBUG("selected physical device index: %d  score: %d",
          best_score,
          device_index);

    // renderer->device_heuristic = selected_device_heuristic;
    renderer->physical_device = best_device;
    renderer->physical_device_index = device_index;

    renderer->transfer_queue_index = selected_device_heuristic.transfer_queue_index;
    renderer->compute_queue_index = selected_device_heuristic.compute_queue_index;
    renderer->graphics_queue_index = selected_device_heuristic.graphics_queue;
    renderer->present_queue_index = selected_device_heuristic.present_queue;

    vulkan_device_query_swapchain_support(renderer->physical_device, renderer->surface,
                                           &renderer->swapchain_capabilities);

    vulkan_physical_device_get_supported_features(renderer->physical_device, &renderer->features2);

    renderer->properties2 = (VkPhysicalDeviceProperties2){
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext = 0,
        .properties = 0
    };
    vkGetPhysicalDeviceProperties2(renderer->physical_device, &renderer->properties2);

    renderer->physical_device_memory = (VkPhysicalDeviceMemoryProperties2){
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
        .pNext = 0,
        .memoryProperties = 0
    };
    vkGetPhysicalDeviceMemoryProperties2(renderer->physical_device, &renderer->physical_device_memory);


    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(renderer->physical_device, &queue_family_count, 0);
    renderer->queue_family_properties = array_create(
        VkQueueFamilyProperties, queue_family_count, &renderer->allocator);
    vkGetPhysicalDeviceQueueFamilyProperties(renderer->physical_device, &queue_family_count,
                                             renderer->queue_family_properties->data);
    renderer->queue_family_properties->num_items = queue_family_count;

    //after we find our device call this on the device
    // vulkan_device_query_swapchain_support(selected_device, renderer->context.surface,
    // renderer->context.swapchain_capabilities);


    //get device queue info for each unique queue family
    hash_set* indices = hash_set_init(sizeof(s32), 10);
    s32* index_array = darray_create_reserve(s32, 10);
    if (hash_set_insert(indices, &renderer->graphics_queue_index))
    {
        darray_push(index_array, renderer->graphics_queue_index);
    }
    if (hash_set_insert(indices, &renderer->present_queue_index))
    {
        darray_push(index_array, renderer->present_queue_index);
    }
    if (hash_set_insert(indices, &renderer->compute_queue_index))
    {
        darray_push(index_array, renderer->compute_queue_index);
    }
    if (hash_set_insert(indices, &renderer->transfer_queue_index))
    {
        darray_push(index_array, renderer->transfer_queue_index);
    }

    u64 index_count = hash_set_get_size(indices);
    u64 index_array_size = darray_get_size(index_array);


    f32 default_queue_priority = 1.0f;
    //get device queue info for each unique queue family
    VkDeviceQueueCreateInfo* queue_create_infos = darray_create_reserve(VkDeviceQueueCreateInfo, index_array_size);
    for (u64 i = 0; i < index_array_size; ++i)
    {
        VkQueueFamilyProperties family_properties = array_get(renderer->queue_family_properties,
                                                              VkQueueFamilyProperties, index_array[i]);

        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = index_array[i];
        queue_create_infos[i].queueCount = 1;

        //OPTIMIZE: having more than one queue count means litereally having something like
        // VKQueue graphics1
        // VKQueue graphics2
        // which can be submitted to seperatly
        // but this can also mean
        // VKQueue graphics1
        // VKQueue transfer1
        // rn these are the same queue in the family, but they can be different queues in the same family
        /*
        queue_create_infos[i].queueCount = clamp_int(queue_create_infos[i].queueCount,
                                                     1, family_properties.queueCount);
        */

        queue_create_infos[i].pQueuePriorities = &default_queue_priority;
        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = 0;
    }


    //TODO: we need to be checking all these when we query for our physical device

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


    // VkPhysicalDeviceVulkan14Features enable_vulkan14_features = {
    // .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
    // };

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


    VkPhysicalDeviceVulkan14Features enable_vulkan14_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .dynamicRenderingLocalRead = VK_TRUE,
        .pNext = &enable_extended_dynamic_state_features,
    };

    VkPhysicalDeviceVulkan13Features enable_vulkan13_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .synchronization2 = VK_TRUE,
        .dynamicRendering = VK_TRUE,
        .pNext = &enable_vulkan14_features,
    };
    VkPhysicalDeviceVulkan12Features enable_vulkan12_features =
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .scalarBlockLayout = VK_TRUE,
        .descriptorIndexing = VK_TRUE,
        .runtimeDescriptorArray = VK_TRUE,
        .descriptorBindingPartiallyBound = VK_TRUE,
        .descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
        .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
        .bufferDeviceAddress = VK_TRUE,
        .bufferDeviceAddressMultiDevice = VK_TRUE,
        .drawIndirectCount = VK_TRUE,
        .timelineSemaphore = VK_TRUE,
        .separateDepthStencilLayouts = VK_TRUE,
        .descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE,
        .shaderStorageBufferArrayNonUniformIndexing = VK_TRUE,
        .descriptorBindingVariableDescriptorCount = VK_TRUE,
        .pNext = &enable_vulkan13_features,
    };
    VkPhysicalDeviceVulkan11Features enable_vulkan11_features =
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = &enable_vulkan12_features,
        .shaderDrawParameters = VK_TRUE,
    };

    // Request device features.
    // TODO: should be config driven
    VkPhysicalDeviceFeatures device_features = {
        .samplerAnisotropy = VK_TRUE,
        .multiDrawIndirect = VK_TRUE,
        .fillModeNonSolid = VK_TRUE,
    };

    VkPhysicalDeviceFeatures2 enable_device_features2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &enable_vulkan11_features,
        .features = device_features,
    };

    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &enable_device_features2,
        .queueCreateInfoCount = index_array_size,
        .pQueueCreateInfos = queue_create_infos,
        .pEnabledFeatures = NULL, // do not use if pNext is used
        .enabledExtensionCount = ARRAY_SIZE(extension_names),
        .ppEnabledExtensionNames = extension_names,
        // Deprecated
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
    };


    // Create the device.
    VK_CHECK(vkCreateDevice(
        renderer->physical_device,
        &device_create_info,
        renderer->vulkan_allocator,
        &renderer->logical_device));

    INFO("Logical device created.");

    // some commmand buffer debug label stuff
    if (app_is_debug_build())
    {
        renderer->debug_label_start = (PFN_vkCmdBeginDebugUtilsLabelEXT)
            vkGetDeviceProcAddr(
                renderer->logical_device, "vkCmdBeginDebugUtilsLabelEXT");
        renderer->debug_label_end = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetDeviceProcAddr(
            renderer->logical_device, "vkCmdEndDebugUtilsLabelEXT");

        MASSERT(renderer->debug_label_start);
        MASSERT(renderer->debug_label_end);
    }
    // Get queues.
    vkGetDeviceQueue(
        renderer->logical_device,
        renderer->graphics_queue_index,
        0,
        &renderer->graphics_queue);

    vkGetDeviceQueue(
        renderer->logical_device,
        renderer->compute_queue_index,
        0,
        &renderer->compute_queue);

    vkGetDeviceQueue(
        renderer->logical_device,
        renderer->present_queue_index,
        0,
        &renderer->present_queue);

    vkGetDeviceQueue(
        renderer->logical_device,
        renderer->transfer_queue_index,
        0,
        &renderer->transfer_queue);

    INFO("Queues obtained.");

    //each command pool is tied to its queue family

    //create the command pool for the graphics queue
    VkCommandPoolCreateInfo pool_create_info = {0};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_create_info.queueFamilyIndex = renderer->graphics_queue_index;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(renderer->logical_device,
        &pool_create_info, renderer->vulkan_allocator,
        &renderer->graphics_command_pool));

    INFO("GRAPHICS COMMAND POOL CREATED.");

    //create the command pool for the graphics queue
    VkCommandPoolCreateInfo transfer_pool_create_info = {0};
    transfer_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    transfer_pool_create_info.queueFamilyIndex = renderer->transfer_queue_index;
    transfer_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(renderer->logical_device,
        &transfer_pool_create_info, renderer->vulkan_allocator,
        &renderer->transfer_command_pool));

    INFO("TRANSFER COMMAND POOL CREATED.");

    //create the command pool for the graphics queue
    VkCommandPoolCreateInfo compute_create_info = {0};
    compute_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    compute_create_info.queueFamilyIndex = renderer->compute_queue_index;
    compute_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(renderer->logical_device,
        &compute_create_info, renderer->vulkan_allocator,
        &renderer->compute_command_pool));

    INFO("COMPUTE COMMAND POOL CREATED.");

    vkDeviceWaitIdle(renderer->logical_device);

    return true;
}

bool vulkan_device_destroy2(Renderer* renderer)
{
    //TODO:
    MASSERT_FALSE()
    /*
    INFO("Vulkan family queues reset")
    vulkan_context->graphics_queue = 0;
    vulkan_context->present_queue = 0;
    vulkan_context->transfer_queue = 0;

    // Destroy logical device
    INFO("Destroying logical device...");
    if (vulkan_context->logical_device)
    {
        vkDestroyDevice(vulkan_context->logical_device, vulkan_context->allocator);
        vulkan_context->logical_device = 0;
    }

    // Physical devices are not destroyed.
    INFO("Physical device reset");
    vulkan_context->physical_device = 0;

    //TODO: this free crashes, look into later, when we got an allocator up and running
    darray_debug_header(vulkan_context->swapchain_capabilities.formats);

    if (vulkan_context->swapchain_capabilities.formats)
    {
        darray_free(vulkan_context->swapchain_capabilities.formats);
        vulkan_context->swapchain_capabilities.formats = 0;
        vulkan_context->swapchain_capabilities.format_count = 0;
    }


    if (vulkan_context->swapchain_capabilities.present_modes)
    {
        darray_free(vulkan_context->swapchain_capabilities.present_modes);
        vulkan_context->swapchain_capabilities.present_modes = 0;
        vulkan_context->swapchain_capabilities.present_mode_count = 0;
    }


    memset(&vulkan_context->swapchain_capabilities.capabilities, 0,
           sizeof(vulkan_context->swapchain_capabilities.capabilities));

    vulkan_context->graphics_queue_index = -1;
    vulkan_context->present_queue_index = -1;
    vulkan_context->transfer_queue_index = -1;

    INFO("VULKAN DEVICE DESTROYED");
     */

    return true;

}


void vulkan_physical_device_get_supported_features(VkPhysicalDevice current_device,
                                                   VkPhysicalDeviceFeatures2* out_features)
{
    //query device chain
    VkPhysicalDeviceVulkan14Features supported_vulkan14 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
    };

    VkPhysicalDeviceVulkan13Features supported_vulkan13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &supported_vulkan14,
    };

    VkPhysicalDeviceVulkan12Features supported_vulkan12 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &supported_vulkan13,
    };

    VkPhysicalDeviceVulkan11Features supported_vulkan11 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = &supported_vulkan12,
    };

    *out_features = (VkPhysicalDeviceFeatures2){
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &supported_vulkan11,
    };

    vkGetPhysicalDeviceFeatures2(
        current_device,
        out_features);
}

void vulkan_device_print_info(VkPhysicalDevice current_device, VkSurfaceKHR surface, Allocator* allocator)
{
    Scratch_Allocator scratch = scratch_allocator_begin(allocator);


    VkPhysicalDeviceProperties2 physical_device_properties = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext = 0,
        .properties = 0
    };
    physical_device_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;


    VkPhysicalDeviceMemoryProperties2 physical_device_memory_properties = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
        .pNext = 0,
        .memoryProperties = 0
    };

    vkGetPhysicalDeviceProperties2(current_device, &physical_device_properties);
    vkGetPhysicalDeviceMemoryProperties2(current_device, &physical_device_memory_properties);

    VkPhysicalDeviceFeatures2 features2 = {0};
    vulkan_physical_device_get_supported_features(current_device, &features2);


    INFO("device info: '%s'.", &physical_device_properties.properties.deviceName);
    // GPU type
    switch (physical_device_properties.properties.deviceType)
    {
    default:
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
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        INFO("GPU type is Unknown.");
        break;
    }
    INFO(
        "GPU Driver version: %d.%d.%d",
        VK_VERSION_MAJOR(physical_device_properties.properties.driverVersion),
        VK_VERSION_MINOR(physical_device_properties.properties.driverVersion),
        VK_VERSION_PATCH(physical_device_properties.properties.driverVersion));

    // Vulkan API version.
    INFO(
        "Vulkan API version: %d.%d.%d",
        VK_VERSION_MAJOR(physical_device_properties.properties.apiVersion),
        VK_VERSION_MINOR(physical_device_properties.properties.apiVersion),
        VK_VERSION_PATCH(physical_device_properties.properties.apiVersion));

    // Memory information
    for (u32 j = 0; j < physical_device_memory_properties.memoryProperties.memoryHeapCount; ++j)
    {
        f32 memory_size_gib = (((f32)physical_device_memory_properties.memoryProperties.memoryHeaps[j].size) / GB(1));
        if (physical_device_memory_properties.memoryProperties.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            INFO("Local GPU memory: %.2f GB", memory_size_gib);
        }
        else
        {
            INFO("Shared System memory: %.2f GB", memory_size_gib);
        }
    }

    //check for sampler anisotropy
    if (features2.features.samplerAnisotropy)
    {
        INFO("DEVICE SUPPORTS SAMPLER ANISOTRPY")
    }
    else
    {
        INFO("DEVICE DOES NOT SUPPORT SAMPLER ANISOTRPY")
    }

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(current_device, &queue_family_count, 0);
    ARRAY_TYPE(VkQueueFamilyProperties)* queue_families = array_create(
        VkQueueFamilyProperties, queue_family_count, scratch.allocator);
    vkGetPhysicalDeviceQueueFamilyProperties(current_device, &queue_family_count,
                                             queue_families->data);
    queue_families->num_items = queue_family_count;
    for (u32 queue_index = 0; queue_index < queue_family_count; queue_index++)
    {
        VkQueueFamilyProperties queue_family_properties = array_get(queue_families, VkQueueFamilyProperties,
                                                                    queue_index);

        DEBUG("QUEUE INDEX: %d", queue_index);
        // Graphics queue?
        if (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            INFO("SUPPORTS GRAPHICS")
        }
        else
        {
            INFO("NO Graphics SUPPORT")
        }

        // Compute queue?
        if (queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            INFO("SUPPORTS COMPUTE")
        }
        else
        {
            INFO("NO COMPUTE SUPPORT")
        }

        // Transfer queue?
        if (queue_family_properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            INFO("SUPPORTS TRANSFER")
        }
        else
        {
            INFO("NO TRANSFER SUPPORT")
        }

        // Present queue?
        VkBool32 supports_present = VK_FALSE;
        VkResult present_result = vkGetPhysicalDeviceSurfaceSupportKHR(current_device, queue_index, surface,
                                                                       &supports_present);
        VK_CHECK(present_result)

        if (supports_present)
        {
            INFO("SUPPORTS PRESENT")
        }
        else
        {
            INFO("NO PRESENT SUPPORT")
        }
    }


    //list all device extensions
    u32 available_extension_count = 0;
    VK_CHECK(vkEnumerateDeviceExtensionProperties( current_device, 0,
        &available_extension_count, 0));

    ARRAY_TYPE(VkExtensionProperties)* available_extensions = array_create(
        VkExtensionProperties, available_extension_count, scratch.allocator);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(
        current_device,
        0,
        &available_extension_count,
        available_extensions->data));
    available_extensions->num_items = available_extension_count;

    DEBUG("Device Extensions");
    for (u32 ext_index = 0; ext_index < available_extension_count; ++ext_index)
    {
        VkExtensionProperties ext_property = array_get(available_extensions, VkExtensionProperties, ext_index);
        INFO("Extensions: %s", ext_property.extensionName);
    }


    scratch_allocator_end(scratch);
}

bool vulkan_physical_device_meets_requirements(VkPhysicalDevice current_device, VkSurfaceKHR surface,
                                               Scratch_Allocator* scratch)
{
    Vulkan_Physical_Device_Requirements requirements = {0};
    // requirements.device_extension_names = darray_create(const char*);
    // darray_push(requirements.device_extension_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);


    // device_met_requirements.discrete_gpu;
    // device_met_requirements.integrated_gpu;


    VkPhysicalDeviceProperties2 physical_device_properties = {0};
    physical_device_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    VkPhysicalDeviceFeatures2 physical_device_features = {0};
    physical_device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    VkPhysicalDeviceMemoryProperties2 physical_device_memory_properties = {0};
    physical_device_memory_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;

    vkGetPhysicalDeviceProperties2(current_device, &physical_device_properties);
    vkGetPhysicalDeviceFeatures2(current_device, &physical_device_features);
    vkGetPhysicalDeviceMemoryProperties2(current_device, &physical_device_memory_properties);

    // GPU type
    switch (physical_device_properties.properties.deviceType)
    {
    default:
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        INFO("GPU type is Integrated.");
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        INFO("GPU type is Discrete.");
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        INFO("GPU type is Virtual.");
        return false;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        INFO("GPU type is CPU.");
        return false;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        INFO("GPU type is Unknown.");
        return false;
        break;
    }


    //check for sampler anisotropy
    if (!physical_device_features.features.samplerAnisotropy)
    {
        M_ERROR("DEVICE DOES NOT SUPPORT SAMPLER ANISOTRPY")
        return false;
    }

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(current_device, &queue_family_count, 0);
    ARRAY_TYPE(VkQueueFamilyProperties)* queue_families = array_create(
        VkQueueFamilyProperties, queue_family_count, scratch->allocator);
    vkGetPhysicalDeviceQueueFamilyProperties(current_device, &queue_family_count,
                                             queue_families->data);
    queue_families->num_items = queue_family_count;
    for (u32 queue_index = 0; queue_index < queue_family_count; queue_index++)
    {
        VkQueueFamilyProperties queue_family_properties = array_get(queue_families, VkQueueFamilyProperties,
                                                                    queue_index);

        DEBUG("QUEUE INDEX: %d", queue_index);
        // Graphics queue?
        if (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            requirements.graphics = true;
        }
        // Compute queue?
        if (queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            requirements.compute = true;
        }

        // Transfer queue?
        if (queue_family_properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            requirements.transfer = true;
        }

        // Present queue?
        VkBool32 supports_present = VK_FALSE;
        VkResult present_result = vkGetPhysicalDeviceSurfaceSupportKHR(current_device, queue_index, surface,
                                                                       &supports_present);
        VK_CHECK(present_result)

        if (supports_present)
        {
            requirements.present = true;
        }
    }

    if (!requirements.present ||
        !requirements.graphics ||
        !requirements.compute ||
        !requirements.transfer)
    {
        return false;
    }


    //list all device extensions
    u32 available_extension_count = 0;
    VK_CHECK(vkEnumerateDeviceExtensionProperties( current_device, 0,
        &available_extension_count, 0));

    //array for the requirements we want, add to them if we need more
    ARRAY_TYPE(const char*)* required_extension_names = array_create(const char*, available_extension_count,
                                                                     scratch->allocator);
    array_push_macro(required_extension_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME); // dont use the regular push version

    //for the extensions we have
    ARRAY_TYPE(VkExtensionProperties)* available_extensions = array_create(
        VkExtensionProperties, available_extension_count, scratch->allocator);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(
        current_device,
        0,
        &available_extension_count,
        available_extensions->data));
    available_extensions->num_items = available_extension_count;

    //check if our required extensions are on this device
    for (u32 required_index = 0; required_index < required_extension_names->num_items; ++required_index)
    {
        bool found = false;
        const char* required_ext_name = array_get(required_extension_names, const char*, required_index);

        for (u32 ext_index = 0; ext_index < available_extension_count; ++ext_index)
        {
            VkExtensionProperties ext_property = array_get(available_extensions, VkExtensionProperties, ext_index);

            if (strcmp(ext_property.extensionName, required_ext_name) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return false;
        }
    }


    //device meets all our requirements
    return true;
}



void vulkan_device_query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                                           Vulkan_Swapchain_Capabilities_Info* out_support_info)
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
                                                          sizeof(VkSurfaceFormatKHR) * out_support_info->
                                                          format_count);
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
bool vulkan_device_detect_depth_format(Renderer* renderer)
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
        vkGetPhysicalDeviceFormatProperties(renderer->physical_device, candidates[i], &format_properties);


        if ((format_properties.linearTilingFeatures & flags) == flags)
        {
            renderer->depth_format = candidates[i];
            return true;
        }
        if ((format_properties.optimalTilingFeatures & flags) == flags)
        {
            renderer->depth_format = candidates[i];
            return true;
        }
    }


    return false;
}


bool vulkan_device_detect_depth_stencil_format(Renderer* renderer)
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
        vkGetPhysicalDeviceFormatProperties(renderer->physical_device, candidates[i], &format_properties);

        if ((format_properties.linearTilingFeatures & flags) == flags)
        {
            renderer->depth_format = candidates[i];
            return true;
        }
        else if ((format_properties.optimalTilingFeatures & flags) == flags)
        {
            renderer->depth_format = candidates[i];
            return true;
        }
    }


    return false;
}
