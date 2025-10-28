#include "vk_device.h"

#include <iostream>
#include <stdexcept>
#include <string.h>


bool get_vulkan_api_version(uint32_t& variant, uint32_t& major, uint32_t& minor, uint32_t& patch)
{
    uint32_t apiVersion;
    // gets the instance
    if (vkEnumerateInstanceVersion(&apiVersion) == VK_SUCCESS)
    {
        variant = VK_API_VERSION_VARIANT(apiVersion);
        major = VK_API_VERSION_MAJOR(apiVersion);
        minor = VK_API_VERSION_MINOR(apiVersion);
        patch = VK_API_VERSION_PATCH(apiVersion);
        std::cout << "Vulkan Version: " << major << "." << minor << "." << patch << " Version Variant: " << variant <<
                '\n';
        return true;
    }

    return false;
}

void create_vk_instance(Vulkan_Context& vulkan_context)
{
     if (!ensure_validation_layer_support())
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }


    uint32_t apiVersion{};
    uint32_t variant{};
    uint32_t major{};
    uint32_t minor{};
    uint32_t patch{};

    // gets the instance
    if (vkEnumerateInstanceVersion(&apiVersion) == VK_SUCCESS)
    {
        variant = VK_API_VERSION_VARIANT(apiVersion);
        major = VK_API_VERSION_MAJOR(apiVersion);
        minor = VK_API_VERSION_MINOR(apiVersion);
        patch = VK_API_VERSION_PATCH(apiVersion);
        std::cout << "Vulkan Version: " << major << "." << minor << "." << patch << " Version Variant: " << variant <<
                '\n';
    }
    else
    {
        throw std::runtime_error("failed to find api version!");
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
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    //create_info.flags;
    create_info.pApplicationInfo = &application_info;
    //extensions
    //we want to get the glfw extensions needed as well


    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.insert(extensions.end(), instance_extensions.begin(), instance_extensions.end());


    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    //validation layers
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    create_info.enabledLayerCount = validationLayers.size();
    create_info.ppEnabledLayerNames = validationLayers.data();
    populateDebugMessengerCreateInfo(debugCreateInfo);

    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;


    //VkInstance instance;
    /*
        VkResult vkCreateInstance(
            const VkInstanceCreateInfo*                 pCreateInfo,
            const VkAllocationCallbacks*                pAllocator,
            VkInstance*                                 pInstance);
            */
    if (vkCreateInstance(&create_info, nullptr, &vulkan_context.instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance");
    }

    std::cout << "CREATE INSTANCE SUCCESS\n";
}

bool ensure_validation_layer_support()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName: validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties: availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                       void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void create_vk_debug_messanger(VkInstance& instance, VkDebugUtilsMessengerEXT& debugMessenger)
{
    //if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);


    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }

    std::cout << "CREATE DEBUG MESSANGER SUCCESS\n";
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}


