#include "Renderer_old.h"

#include <chrono>
#include <cmath>
#include <format>
#include <fstream>
#include <set>
#include <cstring>
#include <iostream>

#include "Tetris.h"
#include "text.h"
#include "UI.h"
#include "vk_buffer.h"
#include "vk_command_buffer.h"
#include "vk_descriptor.h"
#include "vk_device.h"
#include "vk_renderpass.h"


void key_callback(GLFWwindow* window, Game_State* game_state, VERTEX_DYNAMIC_INFO& vertex_info)
{


    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (!e_key_pressed)
        {
            //printf("button pressed\n");
            //printf("creating a quad\n");

            // Generate random position and color for the new quad
            for (int i = 0; i < 50; i++)
            {
                float random_x = ((rand() % 200) - 100) / 100.0f; // Random between -1 and 1
                float random_y = ((rand() % 200) - 100) / 100.0f;
                float random_r = (rand() % 100) / 100.0f;
                float random_g = (rand() % 100) / 100.0f;
                float random_b = (rand() % 100) / 100.0f;

                add_quad(glm::vec2{random_x, random_y}, glm::vec3{random_r, random_g, random_b}, .1f, vertex_info);
            }
            e_key_pressed = true;
        }
    }
    else
    {
        e_key_pressed = false;
    }

    //input keys
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if (!up_key_pressed)
        {
            move_block(game_state->tetris_grid, game_state->current_tetromino, UP, vertex_info);

            up_key_pressed = true;
        }
    }
    else
    {
        up_key_pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        if (!left_key_pressed)
        {
            move_block(game_state->tetris_grid, game_state->current_tetromino, LEFT, vertex_info);

            left_key_pressed = true;
        }
    }
    else
    {
        left_key_pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if (!down_key_pressed)
        {
            move_block(game_state->tetris_grid, game_state->current_tetromino, DOWN, vertex_info);

            //move_cube(vertex_info, game_state.current_block.id, glm::vec2{0,0.1});
            down_key_pressed = true;
        }
    }
    else
    {
        down_key_pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        if (!right_key_pressed)
        {
            move_block(game_state->tetris_grid, game_state->current_tetromino, RIGHT, vertex_info);

            right_key_pressed = true;
        }
    }
    else
    {
        right_key_pressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (!space_key_pressed)
        {
            while (move_block(game_state->tetris_grid, game_state->current_tetromino, DOWN, vertex_info))
            {
                //we want to keep moving the piece down until we can no longer do so
            }
            game_state->tetris_clock.accumulated_time = 0;
            space_key_pressed = true;
        }
    }
    else
    {
        space_key_pressed = false;
    }

    /* TODO: meant to start the game while testing the UI capability
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (!q_key_pressed)
        {
            //printf("button pressed\n");
            //printf("creating a quad\n");

            //TODO: get rid of with ui system
            // Generate random position and color for the new quad
            init_play_game.emplace_back(game_state);
            q_key_pressed = true;
        }
    }
    else
    {
        q_key_pressed = false;
    }*/
}


void init_vulkan(vulkan_context& vulkan_context,
                 GLFW_Window_Context& window_info,
                 Swapchain_Context& swapchain_context,
                 Graphics_Context& graphics_context,
                 Buffer_Context& buffer_context,
                 Command_Buffer_Context& command_buffer_context,
                 Semaphore_Fences_Context& semaphore_fences_context)
{

    init_window(window_info);
    create_vk_instance(vulkan_context);
    create_vk_debug_messanger(vulkan_context.instance, vulkan_context.debugMessenger);
    create_surface(vulkan_context, window_info);
    pick_physical_device(vulkan_context);
    create_logical_device(vulkan_context);
    create_swapchain(vulkan_context, swapchain_context);
    create_image_views(vulkan_context, swapchain_context);
    renderpass_create(vulkan_context, swapchain_context, graphics_context, RENDER_PASS_CLEAR_COLOR_BUFFER_FLAG, false, false);
    //TODO: for now
    //renderpass_create(vulkan_context, swapchain_context, graphics_context, RENDER_PASS_CLEAR_COLOR_BUFFER_FLAG, false, true);
    create_graphics_pipeline(vulkan_context, swapchain_context, graphics_context);
    create_frame_buffers(vulkan_context, swapchain_context, graphics_context);
    command_pool_allocate(vulkan_context, command_buffer_context);
    create_vertex_buffer_new(vulkan_context, command_buffer_context, buffer_context);
    create_index_buffer_new(vulkan_context, command_buffer_context, buffer_context);
    command_buffer_allocate(vulkan_context, command_buffer_context, MAX_FRAMES_IN_FLIGHT);
    create_sync_objects(vulkan_context, semaphore_fences_context);
}


void init_UI_vulkan(vulkan_context& vulkan_context, Swapchain_Context& swapchain_context, Graphics_Context& ui_graphics_context,
                    Graphics_Context& graphics_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& ui_buffer_context, UI_STATE* ui_state)
{
    //only the buffer context needs to be different

    //get window size
    //vertex and index information
    //same renderpass
    //different graphics pipeline

    ui_state->push_constants.screenSize.x =  swapchain_context.surface_capabilities.currentExtent.width;
    ui_state->push_constants.screenSize.y =  swapchain_context.surface_capabilities.currentExtent.height;

    create_vertex_buffer_new(vulkan_context, command_buffer_context, ui_buffer_context);
    create_index_buffer_new(vulkan_context, command_buffer_context, ui_buffer_context);
    create_ui_graphics_pipeline(vulkan_context, ui_graphics_context, graphics_context);
}

void init_Text_vulkan(vulkan_context& vulkan_context, Swapchain_Context& swapchain_context,
                      Graphics_Context& text_graphics_context, Graphics_Context& graphics_context,
                      Command_Buffer_Context& command_buffer_context, Buffer_Context& text_buffer_context,
                      Text_System& text_system, Descriptor& text_descriptor)
{
    create_text_vertex_buffer_new(vulkan_context, command_buffer_context, text_buffer_context);
    create_index_buffer_new(vulkan_context, command_buffer_context, text_buffer_context);
    create_descriptor_set_layout_text(vulkan_context, text_descriptor);
    //graphics pipeline
    create_text_graphics_pipeline(vulkan_context, text_graphics_context, graphics_context, text_descriptor);

    create_descriptor_pool_text(vulkan_context, text_descriptor);
    create_descriptor_sets_text(vulkan_context, text_system.font_texture, text_descriptor);
}


void draw_frame(vulkan_context& vulkan_context, GLFW_Window_Context& window_context, Swapchain_Context& swapchain_context,
                Graphics_Context& graphics_context, Command_Buffer_Context& command_buffer_context,
                Buffer_Context& buffer_context, VERTEX_DYNAMIC_INFO& vertex_info, Semaphore_Fences_Context& semaphore_fences_info,
                Graphics_Context& ui_graphics_context, Buffer_Context& ui_buffer_context, UI_STATE* ui_state,
                Graphics_Context& text_graphics_context, Buffer_Context& text_buffer_context, Descriptor& text_descriptor)
{

    /*
    At a high level, rendering a frame in Vulkan consists of a common set of steps:
    Wait for the previous frame to finish
    Acquire an image from the swap chain
    Record a command buffer which draws the scene onto that image
    Submit the recorded command buffer
    Present the swap chain image
    */
    //semaphore orders queue operations (waiting happens on the GPU),
    //fences waits until all operations on the GPU are done, meant to sync CPU and GPU



    /*Wait for the previous frame to finish*/
    vkWaitForFences(vulkan_context.logical_device, 1,
                    &semaphore_fences_info.in_flight_fence[semaphore_fences_info.currentFrame], VK_TRUE, UINT64_MAX);


    /* Acquire an image from the swap chain */
    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(vulkan_context.logical_device, swapchain_context.swapchain, UINT64_MAX,
                                            semaphore_fences_info.image_available_semaphore[semaphore_fences_info.
                                                currentFrame], VK_NULL_HANDLE, &image_index);

    /*Checking if our window got resized*/
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreate_swapchain(vulkan_context, window_context, swapchain_context, graphics_context, ui_state);
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }


    //reset fence only when we successfully resize
    vkResetFences(vulkan_context.logical_device, 1,
                  &semaphore_fences_info.in_flight_fence[semaphore_fences_info.currentFrame]);

    //WORLD
    update_vertex_buffer_update(vulkan_context, command_buffer_context, buffer_context, vertex_info);

    //UI
    update_vertex_buffer_update(vulkan_context, command_buffer_context, ui_buffer_context, ui_state->draw_info.vertex_info);

    //TEXT
    text_vertex_buffer_update(vulkan_context, command_buffer_context, text_buffer_context, ui_state->text_system);

    /* Record a command buffer which draws the scene onto that image */

    vkResetCommandBuffer(command_buffer_context.command_buffer[semaphore_fences_info.currentFrame], 0);
    //vkResetCommandBuffer(ui_command_buffer_context.command_buffer[semaphore_fences_info.currentFrame], 0); // TODO:


    //WORLD DRAW COMMAND
    record_command_buffer(swapchain_context, command_buffer_context, graphics_context, buffer_context, vertex_info,image_index, semaphore_fences_info.currentFrame,
        ui_graphics_context, ui_buffer_context, ui_state,
        text_graphics_context, text_buffer_context, text_descriptor);

    //UI DRAW COMMAND
    //ui_record_command_buffer(swapchain_context, ui_command_buffer_context, ui_graphics_context, ui_draw_info, image_index, semaphore_fences_info.currentFrame);



    //std::vector<VkCommandBuffer> command_buffers = {command_buffer_context.command_buffer[semaphore_fences_info.currentFrame], ui_command_buffer_context.command_buffer[semaphore_fences_info.currentFrame]};
    /*Submit the recorded command buffer*/

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {
        semaphore_fences_info.image_available_semaphore[semaphore_fences_info.currentFrame]
    };
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffer_context.command_buffer[semaphore_fences_info.currentFrame];
    //submitInfo.pCommandBuffers = &ui_command_buffer_context.command_buffer[semaphore_fences_info.currentFrame];
    //submitInfo.commandBufferCount = command_buffers.size();
    //submitInfo.pCommandBuffers = command_buffers.data();

    VkSemaphore signalSemaphores[] = {
        semaphore_fences_info.render_finished_semaphore[semaphore_fences_info.currentFrame]
    };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;


    if (vkQueueSubmit(vulkan_context.graphics_queue, 1, &submitInfo,
                   semaphore_fences_info.in_flight_fence[semaphore_fences_info.currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }


    VkRect2D scissor_rectangle = {};

    /*Present Image*/
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.waitSemaphoreCount = 1;

    VkSwapchainKHR swapChains[] = {swapchain_context.swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &image_index;
    /* It allows you to specify an array of VkResult values to check for every individual swap chain if presentation was successful.
     * It's not necessary if you're only using a single swap chain, because you can simply use the return value of the present function.*/
    //presentInfo.pResults = nullptr; // Optional allows you to check every single

    vkQueuePresentKHR(vulkan_context.present_queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window_context.framebufferResized)
    {
        window_context.framebufferResized = false;
        recreate_swapchain(vulkan_context, window_context, swapchain_context, graphics_context, ui_state);
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    semaphore_fences_info.currentFrame = (semaphore_fences_info.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

}

void init_window(GLFW_Window_Context& context)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    //Windowed mode windows can be made full screen by setting a monitor with glfwSetWindowMonitor,
    //and full screen ones can be made windowed by unsetting it with the same function.


    //glfwSetWindowMonitor(context.window, nullptr,100, 100, context.WIDTH, context.HEIGHT, GLFW_DONT_CARE); //set to width wanted
    //glfwSetWindowMonitor(context.window, glfwGetPrimaryMonitor(),100, 100, context.WIDTH, context.HEIGHT, GLFW_DONT_CARE); //sets to full screen


    context.window = glfwCreateWindow(context.WIDTH, context.HEIGHT, context.WINDOW_NAME, nullptr, nullptr);


    //goes full screen
    //context.window = glfwCreateWindow(context.WIDTH, context.HEIGHT, context.WINDOW_NAME, glfwGetPrimaryMonitor(), nullptr);
    glfwSetWindowUserPointer(context.window, &context);
    glfwSetInputMode(context.window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetFramebufferSizeCallback(context.window, framebufferResizeCallback);

    std::cout << "INIT WINDOW SUCCESS\n";
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto a = static_cast<GLFW_Window_Context *>(glfwGetWindowUserPointer(window));
    a->framebufferResized = true; //make sure we got resized
}




void create_logical_device(vulkan_context& vulkan_context)
{
    //specify queue to use, specify device extensions and device features, create logical device


    /*
    // Provided by VK_VERSION_1_0
    typedef struct VkDeviceQueueCreateInfo {
    VkStructureType             sType;
    const void*                 pNext;
    VkDeviceQueueCreateFlags    flags;
    uint32_t                    queueFamilyIndex;
    uint32_t                    queueCount;
    const float*                pQueuePriorities;
    } VkDeviceQueueCreateInfo;
    */


    //grab the queue families
    QueueFamilyIndices indices = find_queue_families(vulkan_context.surface, vulkan_context.physical_device);

    std::set<uint32_t> unique_queue_families = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    //specify the queues we want to use
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float priority = 1.0f;
    for (uint32_t unique_queue_family: unique_queue_families)
    {
        VkDeviceQueueCreateInfo device_queue_create_info = {};
        device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        //device_queue_create_info.pNext;
        //device_queue_create_info.flags;
        device_queue_create_info.queueFamilyIndex = unique_queue_family;
        device_queue_create_info.queueCount = 1;
        device_queue_create_info.pQueuePriorities = &priority;
        queueCreateInfos.push_back(device_queue_create_info);
    }

    /*SPECS: The pNext chain of this structure is used to extend the structure with features defined by extensions.
     *This structure can be used in vkGetPhysicalDeviceFeatures2 or can be included in the pNext chain of a VkDeviceCreateInfo structure,
     *in which case it controls which features are enabled in lieu of pEnabledFeatures.*/
    /*
    typedef struct VkPhysicalDeviceFeatures2 {
        VkStructureType             sType;
        void*                       pNext;
        VkPhysicalDeviceFeatures    features;
    } VkPhysicalDeviceFeatures2; */
    //features to be enabled
    VkPhysicalDeviceFeatures2 deviceFeatures = {};
    //void*p
    deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    //it replaces the old device features
    deviceFeatures.features.samplerAnisotropy = VK_TRUE;


    /*
    typedef struct VkDeviceCreateInfo {
        VkStructureType                    sType;
        const void*                        pNext;
        VkDeviceCreateFlags                flags;
        uint32_t                           queueCreateInfoCount;
        const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
        uint32_t                           enabledExtensionCount;
        const char* const*                 ppEnabledExtensionNames;
        const VkPhysicalDeviceFeatures*    pEnabledFeatures;
    } VkDeviceCreateInfo;*/

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // SPECS: VkPhysicalDeviceFeatures2 should be provided in the pNext chain of VkDeviceCreateInfo instead of using VkDeviceCreateInfo::pNext:pEnabledFeatures.
    device_create_info.pNext = &deviceFeatures;
    device_create_info.pEnabledFeatures = VK_NULL_HANDLE; //DO NOT USE READ LINE ABOVE
    //device_create_info.flags;
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    device_create_info.pQueueCreateInfos = queueCreateInfos.data();
    //extensions
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    device_create_info.ppEnabledExtensionNames = device_extensions.data();
    //TODO: on/off for validation layers
    device_create_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    device_create_info.ppEnabledLayerNames = validationLayers.data();

    /*
    VkResult vkCreateDevice(
    VkPhysicalDevice                            physicalDevice,
    const VkDeviceCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDevice*                                   pDevice);
    */

    vkCreateDevice(vulkan_context.physical_device, &device_create_info, nullptr, &vulkan_context.logical_device);

    vkGetDeviceQueue(vulkan_context.logical_device, indices.graphicsFamily.value(), 0, &vulkan_context.graphics_queue);
    vkGetDeviceQueue(vulkan_context.logical_device, indices.presentFamily.value(), 0, &vulkan_context.present_queue);

    std::cout << "CREATE LOGICAL DEVICE SUCCESS\n";
}

void create_swapchain(vulkan_context& vulkan_context, Swapchain_Context& swapchain_context)
{
    /*
    typedef struct VkSwapchainCreateInfoKHR {
    VkStructureType                  sType;
    const void*                      pNext;
    VkSwapchainCreateFlagsKHR        flags;
    VkSurfaceKHR                     surface;
    uint32_t                         minImageCount;
    VkFormat                         imageFormat;
    VkColorSpaceKHR                  imageColorSpace;
    VkExtent2D                       imageExtent;
    uint32_t                         imageArrayLayers;
    VkImageUsageFlags                imageUsage;
    VkSharingMode                    imageSharingMode;
    uint32_t                         queueFamilyIndexCount;
    const uint32_t*                  pQueueFamilyIndices;
    VkSurfaceTransformFlagBitsKHR    preTransform;
    VkCompositeAlphaFlagBitsKHR      compositeAlpha;
    VkPresentModeKHR                 presentMode;
    VkBool32                         clipped;
    VkSwapchainKHR                   oldSwapchain;
} VkSwapchainCreateInfoKHR;
    */

    //get window/surface info
    //VkSurfaceCapabilitiesKHR surface_capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_context.physical_device, vulkan_context.surface,
                                              &swapchain_context.surface_capabilities);


    //query for surface format
    uint32_t surface_format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_context.physical_device, vulkan_context.surface, &surface_format_count,
                                         nullptr);
    std::vector<VkSurfaceFormatKHR> surface_format_available(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_context.physical_device, vulkan_context.surface, &surface_format_count,
                                         surface_format_available.data());
    //select a surface
    swapchain_context.surface_format = choose_swap_surface_format(surface_format_available);


    //get available present modes
    uint32_t presentModeCount;

    vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_context.physical_device, vulkan_context.surface, &presentModeCount,
                                              nullptr);
    std::vector<VkPresentModeKHR> present_modes_available(presentModeCount);
    if (presentModeCount != 0)
    {
        vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_context.physical_device, vulkan_context.surface,
                                                  &presentModeCount, present_modes_available.data());
    }



    //get the max image count for our surface swaps
    //we just want either min images +1 or max images, if min images is larger than max images
    uint32_t image_count = swapchain_context.surface_capabilities.minImageCount + 1;
    if (swapchain_context.surface_capabilities.maxImageCount > 0 && image_count > swapchain_context.surface_capabilities
        .maxImageCount)
    {
        image_count = swapchain_context.surface_capabilities.maxImageCount;
    }

    //best to use mailbox mode or fifo(fifo is always supported)
    VkPresentModeKHR chosen_swapchain_present_mode = choose_present_mode(present_modes_available);

    QueueFamilyIndices indices = find_queue_families(vulkan_context.surface, vulkan_context.physical_device);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value()};


    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    //swapchain_create_info.pNext;
    //swapchain_create_info.flags;
    swapchain_create_info.surface = vulkan_context.surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = swapchain_context.surface_format.format;
    swapchain_create_info.imageColorSpace = swapchain_context.surface_format.colorSpace;
    swapchain_create_info.imageExtent = swapchain_context.surface_capabilities.currentExtent;
    swapchain_create_info.imageArrayLayers = 1; //1 unless its stereoscopic
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    //swapchain_create_info.queueFamilyIndexCount; //TODO: involved with image sharing
    //swapchain_create_info.pQueueFamilyIndices; //TODO: involved with image sharing
    swapchain_create_info.preTransform = swapchain_context.surface_capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //this sets the alpha value to 1
    swapchain_create_info.presentMode = chosen_swapchain_present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    //swapchain_create_info.oldSwapchain; used for resizing

    //find the sharing mode
    if (indices.graphicsFamily != indices.presentFamily)
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    if (vkCreateSwapchainKHR(vulkan_context.logical_device, &swapchain_create_info, nullptr,
                             &swapchain_context.swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }


    //query how many swapchains we got
    vkGetSwapchainImagesKHR(vulkan_context.logical_device, swapchain_context.swapchain, &image_count, nullptr);
    //retrieve swapchain image handles
    swapchain_context.swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(vulkan_context.logical_device, swapchain_context.swapchain, &image_count,
                            swapchain_context.swap_chain_images.data());

    std::cout << "CREATE SWAPCHAIN SUCCESS\n";
};

VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat: availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace ==
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}


VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& present_modes_available)
{
    for (auto modes_available: present_modes_available)
    {
        if (modes_available == VK_PRESENT_MODE_MAILBOX_KHR) // best for games
        {
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR; // vsync which is required to be available by vulkan
}

void recreate_swapchain(vulkan_context& vulkan_context, GLFW_Window_Context& window_context,
                        Swapchain_Context& swapchain_context, Graphics_Context& graphics_context, UI_STATE* ui_state)
{
    //have the device wait while the screen gets recreated
    int width = 0, height = 0;
    glfwGetFramebufferSize(window_context.window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window_context.window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(vulkan_context.logical_device);

    cleanup_swapchain(vulkan_context, swapchain_context, graphics_context);

    create_swapchain(vulkan_context, swapchain_context);
    create_image_views(vulkan_context, swapchain_context);
    create_frame_buffers(vulkan_context, swapchain_context, graphics_context);
    update_UI_on_resize(ui_state, swapchain_context.surface_capabilities.currentExtent.width, swapchain_context.surface_capabilities.currentExtent.height);

}

void cleanup_swapchain(vulkan_context& vulkan_context, Swapchain_Context& swapchain_context,
                       Graphics_Context& graphics_context)
{
    for (auto framebuffer: graphics_context.frame_buffers)
    {
        vkDestroyFramebuffer(vulkan_context.logical_device, framebuffer, nullptr);
    }
    for (auto imageView: swapchain_context.swap_chain_image_views)
    {
        vkDestroyImageView(vulkan_context.logical_device, imageView, nullptr);
    }


    vkDestroySwapchainKHR(vulkan_context.logical_device, swapchain_context.swapchain, nullptr);
}

void create_image_views(vulkan_context& vulkan_context, Swapchain_Context& swapchain_context)
{
    //create an image view for every image
    swapchain_context.swap_chain_image_views.resize(swapchain_context.swap_chain_images.size());

    /*
    typedef struct VkImageViewCreateInfo {
        VkStructureType            sType;
        const void*                pNext;
        VkImageViewCreateFlags     flags;
        VkImage                    image;
        VkImageViewType            viewType;
        VkFormat                   format;
        VkComponentMapping         components;
        VkImageSubresourceRange    subresourceRange;
    } VkImageViewCreateInfo;*/
    /*
    VkResult vkCreateImageView(
        VkDevice                                    device,
        const VkImageViewCreateInfo*                pCreateInfo,
        const VkAllocationCallbacks*                pAllocator,
        VkImageView*                                pView);*/

    //we are iterating over the views, but we can also iterate over the
    for (size_t i = 0; i < swapchain_context.swap_chain_images.size(); i++)
    {
        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        //image_view_create_info.pNext;
        //image_view_create_info.flags;
        image_view_create_info.image = swapchain_context.swap_chain_images[i]; // set images
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D; //view type, 1d,2d,3d,cube, 1d/2d,cube array
        image_view_create_info.format = swapchain_context.surface_format.format;
        //gotten from the swapchain surface format
        //image_view_create_info.components; // remapping of color components, or even depth or stencil after converted into a color component
        //image_view_create_info.subresourceRange; describes images purpose
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0; //TODO: mipmap
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        VkImageView image_view;
        if (vkCreateImageView(vulkan_context.logical_device, &image_view_create_info, nullptr, &image_view) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        };
        swapchain_context.swap_chain_image_views[i] = image_view;
    }

    std::cout << "CREATE IMAGEVIEWS SUCCESS\n";
}

void create_graphics_pipeline(vulkan_context& vulkan_context, Swapchain_Context& swapchain_context,
                              Graphics_Context& graphics_context)
{
    //load shaders from file
    auto vert_shader_code = read_shader_file("../shaders/vert.spv");
    auto frag_shader_code = read_shader_file("../shaders/frag.spv");
    //C:\Users\Adams Humbert\Documents\Clion\VulkanTetris
    //create shader modules for use in the shader stage create info
    VkShaderModule vert_shader_module = create_shader_module(vulkan_context.logical_device, vert_shader_code);
    VkShaderModule fragment_shader_module = create_shader_module(vulkan_context.logical_device, frag_shader_code);

    /*
     // Provided by VK_VERSION_1_0
typedef struct VkPipelineShaderStageCreateInfo {
    VkStructureType                     sType;
    const void*                         pNext;
    VkPipelineShaderStageCreateFlags    flags;
    VkShaderStageFlagBits               stage;
    VkShaderModule                      module;
    const char*                         pName;
    const VkSpecializationInfo*         pSpecializationInfo;
} VkPipelineShaderStageCreateInfo;
     */

    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";


    VkPipelineShaderStageCreateInfo shaderStages[] = {vert_shader_stage_info, frag_shader_stage_info};

    //vertex input assembly, describe the format of the vertex data
    //Bindings: spacing between data and whether the data is per-vertex or per-instance
    //Attribute descriptions: type of the attributes passed to the vertex shader, which binding to load them from and at which offset

    //TODO: VkVertexInputBindingDescription vertex_binding_description= getBindingDescription();
    //TODO: std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

    auto binding_description = getBindingDescription();
    auto attribute_description = getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 1; // the number of pvertexbinding descriptions
    vertex_input_state_create_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_description.
        size());
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_description.data();

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    //rasterizer.pNext;
    //rasterizer.flags;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    // VK_POLYGON_MODE_LINE for wireframes, VK_POLYGON_MODE_POINT for just points, using these require gpu features
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // counter means positive area is front facing, clockwise means negative area is front facing
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    //rasterizer.depthBiasConstantFactor = 0.0f;
    //rasterizer.depthBiasClamp = 0.0f;
    //rasterizer.depthBiasSlopeFactor = 0.0f;

    //not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    //multisampling.minSampleShading = 1.0f; // Optional
    //multisampling.pSampleMask = nullptr; // Optional
    //multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    //multisampling.alphaToOneEnable = VK_FALSE; // Optional


    //TODO:
    //VkPipelineDepthStencilStateCreateInfo depth_stencil ={};

    //happens after color returns from the fragment shader
    //METHOD:
    //Mix the old and new value to produce a final color
    //Combine the old and new value using a bitwise operation
    /* Both ways showcased below
    * if (blendEnable) {
    finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
    finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
    finalColor = newColor;
    }
    finalColor = finalColor & colorWriteMask;
     */

    //TODO: I should look more into this later, its kinda like photoshop blend modes
    // the most important is the src and dst
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
                                          | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &colorBlendAttachment; // this thing can be a vector
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional


    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    //for resizing the viewport, can be used for blend constants
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();


    //used to send info to the vertex/fragment shader, like in uniform buffers, to change shader behavior
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0; // Optional
    pipeline_layout_info.pSetLayouts = nullptr; // Optional
    pipeline_layout_info.pushConstantRangeCount = 0; // Optional
    pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(vulkan_context.logical_device, &pipeline_layout_info, nullptr, &graphics_context.pipeline_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }


    VkGraphicsPipelineCreateInfo graphics_pipeline_info{};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shaderStages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState = nullptr;
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = graphics_context.pipeline_layout;
    graphics_pipeline_info.renderPass = graphics_context.render_pass;
    graphics_pipeline_info.subpass = 0;
    graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_info.basePipelineIndex = -1;

    /*
    VkResult vkCreateGraphicsPipelines(
        VkDevice                                    device,
        VkPipelineCache                             pipelineCache,
        uint32_t                                    createInfoCount,
        const VkGraphicsPipelineCreateInfo*         pCreateInfos,
        const VkAllocationCallbacks*                pAllocator,
        VkPipeline*                                 pPipelines);*/

    vkCreateGraphicsPipelines(vulkan_context.logical_device, VK_NULL_HANDLE, 1, &graphics_pipeline_info, nullptr,
                              &graphics_context.graphics_pipeline);

    vkDestroyShaderModule(vulkan_context.logical_device, fragment_shader_module, nullptr);
    vkDestroyShaderModule(vulkan_context.logical_device, vert_shader_module, nullptr);
    std::cout << "CREATED GRAPHICS PIPELINE SUCCESS\n";
}

std::vector<char> read_shader_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        std::cout << "Failed to open file: " << filename << std::endl;
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
};

VkShaderModule create_shader_module(VkDevice& logical_device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo shader_module_create_info{};
    //shader_module_create_info.flags = 0;
    //vertex_create_info.pNext;
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = code.size();
    shader_module_create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shader_module;
    if (vkCreateShaderModule(logical_device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vertex shader module!");
    };
    return shader_module;
}

void create_render_pass(vulkan_context& vulkan_context, Swapchain_Context& swapchain_context,
                        Graphics_Context& graphics_context)
{
    //TODO: CHANGE TO DYNAMIC RENDERING AND REPLACES RENDER PASS AND FRAME BUFFer
    //RESOURCE: https://www.youtube.com/watch?v=m1RHLavNjKo&t=624s
    //VkPipelineRenderingCreateInfo
    //VK_KHR_dynamic_rendering_local_read dynamic_render;


    //VkPipelineLayout pipelineLayout; idk what this is here for

    //TODO: multisampling
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain_context.surface_format.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //we are not presenting the UI is
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; //VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(vulkan_context.logical_device, &renderPassInfo, nullptr,
                           &graphics_context.render_pass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }

    std::cout << "CREATED RENDERPASS SUCCESS\n";
}

void create_frame_buffers(vulkan_context& vulkan_context, Swapchain_Context& swapchain_context,
                          Graphics_Context& graphics_context)
{
    graphics_context.frame_buffers.resize(swapchain_context.swap_chain_image_views.size());

    for (size_t i = 0; i < swapchain_context.swap_chain_image_views.size(); i++)
    {
        VkImageView attachments[] = {swapchain_context.swap_chain_image_views[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = graphics_context.render_pass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapchain_context.surface_capabilities.currentExtent.width;
        framebufferInfo.height = swapchain_context.surface_capabilities.currentExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(vulkan_context.logical_device, &framebufferInfo, nullptr,
                                &graphics_context.frame_buffers[i])
            != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    std::cout << "CREATED FRAMEBUFFER SUCCESS\n";
}




void create_vertex_buffer(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context)
{
    //NOTE: You can use a seperate device for transfering the vertex buffer from CPU to GPU
    //https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer (top of the page)
    /*
    * Modify QueueFamilyIndices and findQueueFamilies to explicitly look for a queue family with the VK_QUEUE_TRANSFER_BIT bit, but not the VK_QUEUE_GRAPHICS_BIT.
    *Modify createLogicalDevice to request a handle to the transfer queue
    *Create a second command pool for command buffers that are submitted on the transfer queue family
    *Change the sharingMode of resources to be VK_SHARING_MODE_CONCURRENT and specify both the graphics and transfer queue families
    *Submit any transfer commands like vkCmdCopyBuffer (which we'll be using in this chapter) to the transfer queue instead of the graphics queue
     */

    //create buffer, allocate buffer, and bind to memory (vertex buffer specifically)
    VkDeviceSize buffer_size = sizeof(vertices[0]) * (vertices.size());

    //transfer from a staging buffer into our actual vertex buffer
    //we need to use a staging buffer or else we wont be able to allocate memory using vkMapMemory

    //VK_BUFFER_USAGE_TRANSFER_SRC_BIT: Buffer can be used as source in a memory transfer operation.
    //VK_BUFFER_USAGE_TRANSFER_DST_BIT: Buffer can be used as destination in a memory transfer operation.
    buffer_create(&vulkan_context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  buffer_context.vertex_staging_buffer, buffer_context.vertex_staging_buffer_memory);

    // fill in vertex buffer
    // mapping the buffer memory into cpu accessible memory
    void* data;
    vkMapMemory(vulkan_context.logical_device, buffer_context.vertex_staging_buffer_memory, 0, buffer_size, 0, &data);
    //copy vertex data into the mapped memory and then unmap it
    memcpy(data, vertices.data(), (size_t) buffer_size);
    vkUnmapMemory(vulkan_context.logical_device, buffer_context.vertex_staging_buffer_memory);

    //host visible buffer as temporary buffer and use a device local one as actual vertex buffer.

    /*
    *  It is also possible that writes to the buffer are not visible in the mapped memory yet.
    *  There are two ways to deal with that problem:
    Use a memory heap that is host coherent, indicated with VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

    Call vkFlushMappedMemoryRanges after writing to the mapped memory,
    and call vkInvalidateMappedMemoryRanges before reading from the mapped memory

    We went for the first approach,
    which ensures that the mapped memory always matches the contents of the allocated memory.
    Do keep in mind that this may lead to slightly worse performance than explicit flushing,
    but we'll see why that doesn't matter in the next chapter.
     */

    //vertex buffer in device local memory
    buffer_create(&vulkan_context, buffer_size,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_context.vertex_buffer,
                  buffer_context.vertex_buffer_memory);

    //move for gpu access
    buffer_copy(vulkan_context, command_buffer_context, buffer_context.vertex_staging_buffer, buffer_context.vertex_buffer,
                buffer_size);


    std::cout << "CREATED VERTEX BUFFER SUCCESS\n";
}






void create_index_buffer(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context)
{
    //basically the same thing as create vertex buffer
    VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();

    buffer_create(&vulkan_context, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  buffer_context.index_staging_buffer, buffer_context.index_staging_buffer_memory);

    vkMapMemory(vulkan_context.logical_device, buffer_context.index_staging_buffer_memory, 0, buffer_size, 0, &buffer_context.data_index);
    memcpy(buffer_context.data_index, indices.data(), (size_t) buffer_size);
    vkUnmapMemory(vulkan_context.logical_device, buffer_context.index_staging_buffer_memory);

    buffer_create(&vulkan_context, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_context.index_buffer,
                  buffer_context.index_buffer_memory);

    buffer_copy(vulkan_context, command_buffer_context, buffer_context.index_staging_buffer, buffer_context.index_buffer,
                buffer_size);


    std::cout << "CREATED INDEX BUFFER SUCCESS\n";
}


void create_command_buffer(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context)
{
    command_buffer_context.command_buffer.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo buffer_allocate_info{};
    buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_allocate_info.commandBufferCount = static_cast<uint32_t>(command_buffer_context.command_buffer.size());
    //VK_COMMAND_BUFFER_LEVEL_SECONDARY - idk what having a secondary level means
    buffer_allocate_info.commandPool = command_buffer_context.command_pool;
    buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;


    if (vkAllocateCommandBuffers(vulkan_context.logical_device, &buffer_allocate_info,
                                 command_buffer_context.command_buffer.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffer!");
    }

    std::cout << "CREATED COMMANDBUFFER SUCCESS\n";
}

void record_command_buffer(Swapchain_Context& swapchain_context, Command_Buffer_Context& command_buffer_context,
                           Graphics_Context& graphics_context, Buffer_Context& buffer_context, VERTEX_DYNAMIC_INFO& vertex_info, uint32_t image_index, uint32_t current_frame, Graphics_Context
                           &ui_graphics_context, Buffer_Context& ui_buffer_context, UI_STATE* ui_state, Graphics_Context&
                           text_graphics_context, Buffer_Context& text_buffer_context, Descriptor& text_descriptor)
{
    VkCommandBufferBeginInfo buffer_begin_info{};
    buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //buffer_begin_info.flags = 0;
    /*
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
    VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
    VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can be resubmitted while it is also already pending execution.
    */
    buffer_begin_info.pInheritanceInfo = nullptr; //used if its a secondary command buffer

    if (vkBeginCommandBuffer(command_buffer_context.command_buffer[current_frame], &buffer_begin_info) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin command buffer!");
    }

    //start the render pass
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = graphics_context.render_pass;
    render_pass_info.framebuffer = graphics_context.frame_buffers[image_index];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swapchain_context.surface_capabilities.currentExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; //black color
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;
    // last value is specified by if we are using a secondary command buffer
    vkCmdBeginRenderPass(command_buffer_context.command_buffer[current_frame], &render_pass_info,
                         VK_SUBPASS_CONTENTS_INLINE);

    //this is also where we could specify the compute graphics pipeline
    vkCmdBindPipeline(command_buffer_context.command_buffer[current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphics_context.graphics_pipeline);

    //we have to specify the viewport and scissors since they are dynamic
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain_context.surface_capabilities.currentExtent.width);
    viewport.height = static_cast<float>(swapchain_context.surface_capabilities.currentExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffer_context.command_buffer[current_frame], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain_context.surface_capabilities.currentExtent;
    vkCmdSetScissor(command_buffer_context.command_buffer[current_frame], 0, 1, &scissor);

    //bind the vertex buffer
    //VkBuffer vertex_buffer[] = {buffer_context.vertex_buffer}; // IDK WHY THIS IS IN THE VULKAN TUTORIAL
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer_context.command_buffer[current_frame], 0, 1, &buffer_context.vertex_buffer, offsets);

    //bind index buffer, there can only ever be one index buffer, but you can have multiple vertex buffers
    //the possible types are VK_INDEX_TYPE_UINT16 and VK_INDEX_TYPE_UINT32.
    vkCmdBindIndexBuffer(command_buffer_context.command_buffer[current_frame], buffer_context.index_buffer,
                         0, VK_INDEX_TYPE_UINT16);

    //for vertex only draw
    //vkCmdDraw(command_buffer_context.command_buffer[current_frame], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
    //for vertex + indices
    ///old version, doesn't change dynamically
    //vkCmdDrawIndexed(command_buffer_context.command_buffer[current_frame], static_cast<uint32_t>(indicies.size()),
     //                    1, 0, 0, 0);

    vkCmdDrawIndexed(command_buffer_context.command_buffer[current_frame],
                     static_cast<uint32_t>(vertex_info.dynamic_indices.size()),
                     1, 0, 0, 0);

    /***************************
     * UI
     **************************/


    vkCmdBindPipeline(command_buffer_context.command_buffer[current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                      ui_graphics_context.graphics_pipeline);

    //bind the vertex buffer
    VkBuffer vk_buffer2 = {ui_buffer_context.vertex_buffer};
    VkDeviceSize ui_offsets[] = {0};

    //vkCmdPushConstants(command_buffer_context.command_buffer[current_frame], ui_graphics_context.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(UI_Push_Constants), &ui_draw_info.push_constants);

    vkCmdPushConstants(command_buffer_context.command_buffer[current_frame],
       ui_graphics_context.pipeline_layout,
       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
       0,
       sizeof(Screen_Size_Push_Constants),
       &ui_state->push_constants);

    vkCmdBindVertexBuffers(command_buffer_context.command_buffer[current_frame], 0, 1, &vk_buffer2, ui_offsets);

    vkCmdBindIndexBuffer(command_buffer_context.command_buffer[current_frame], ui_buffer_context.index_buffer,
                         0, VK_INDEX_TYPE_UINT16);

    /*
    vkCmdPushConstants(command_buffer_context.command_buffer[current_frame],
       ui_graphics_context.pipeline_layout,
       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
       0,
       sizeof(UI_Push_Constants),
       &ui_draw_info.push_constants);*/


    vkCmdDrawIndexed(command_buffer_context.command_buffer[current_frame],
                         static_cast<uint32_t>(ui_state->draw_info.vertex_info.dynamic_indices.size()),
                         1, 0, 0, 0);

    /***************************
     * TEXt
     **************************/

    vkCmdBindPipeline(command_buffer_context.command_buffer[current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                     text_graphics_context.graphics_pipeline);

    //bind the vertex buffer
    VkBuffer vk_buffer3 = {text_buffer_context.vertex_buffer};
    VkDeviceSize text_offsets[] = {0};

    vkCmdBindDescriptorSets(command_buffer_context.command_buffer[current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, text_graphics_context.pipeline_layout, 0, 1, &text_descriptor.descriptor_sets[current_frame], 0, nullptr);

    vkCmdBindVertexBuffers(command_buffer_context.command_buffer[current_frame], 0, 1, &vk_buffer3, text_offsets);

    vkCmdBindIndexBuffer(command_buffer_context.command_buffer[current_frame], text_buffer_context.index_buffer,
                         0, VK_INDEX_TYPE_UINT16);

    /*
    vkCmdPushConstants(command_buffer_context.command_buffer[current_frame],
       ui_graphics_context.pipeline_layout,
       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
       0,
       sizeof(UI_Push_Constants),
       &ui_draw_info.push_constants);*/


    vkCmdDrawIndexed(command_buffer_context.command_buffer[current_frame],
                         static_cast<uint32_t>(ui_state->text_system.dynamic_indices.size()),
                         1, 0, 0, 0);



    vkCmdEndRenderPass(command_buffer_context.command_buffer[current_frame]);

    if (vkEndCommandBuffer(command_buffer_context.command_buffer[current_frame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to end command buffer!");
    }

    //std::cout << "RECORD COMMANDPOOL SUCCESS\n";
}

void create_sync_objects(vulkan_context& vulkan_context, Semaphore_Fences_Context& semaphore_fences_info)
{
    semaphore_fences_info.render_finished_semaphore.resize(MAX_FRAMES_IN_FLIGHT);
    semaphore_fences_info.image_available_semaphore.resize(MAX_FRAMES_IN_FLIGHT);
    semaphore_fences_info.in_flight_fence.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(vulkan_context.logical_device, &semaphoreInfo, nullptr,
                              &semaphore_fences_info.image_available_semaphore[i]) != VK_SUCCESS ||
            vkCreateSemaphore(vulkan_context.logical_device, &semaphoreInfo, nullptr,
                              &semaphore_fences_info.render_finished_semaphore[i]) != VK_SUCCESS ||
            vkCreateFence(vulkan_context.logical_device, &fenceInfo, nullptr,
                          &semaphore_fences_info.in_flight_fence[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphores!");
        }
    }

    std::cout << "CREATE SYNC OBJECTS SUCCESS\n";
}

void cleanup(vulkan_context& vulkan_context, GLFW_Window_Context& window_info,
             Swapchain_Context& swapchain_context, Graphics_Context& graphics_context,
             Command_Buffer_Context& command_buffer_context, Buffer_Context&
             buffer_context, Semaphore_Fences_Context& semaphore_fences_context)
{
    cleanup_swapchain(vulkan_context, swapchain_context, graphics_context);

    vkDestroyPipeline(vulkan_context.logical_device, graphics_context.graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(vulkan_context.logical_device, graphics_context.pipeline_layout, nullptr);
    vkDestroyRenderPass(vulkan_context.logical_device, graphics_context.render_pass, nullptr);

    buffer_destroy_free(vulkan_context, buffer_context);




    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(vulkan_context.logical_device, semaphore_fences_context.render_finished_semaphore[i],
                           nullptr);
        vkDestroySemaphore(vulkan_context.logical_device, semaphore_fences_context.image_available_semaphore[i],
                           nullptr);
        vkDestroyFence(vulkan_context.logical_device, semaphore_fences_context.in_flight_fence[i], nullptr);
    }

    vkDestroyCommandPool(vulkan_context.logical_device, command_buffer_context.command_pool, nullptr);

    vkDestroyDevice(vulkan_context.logical_device, nullptr);

    //if (enableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(vulkan_context.instance, vulkan_context.debugMessenger, nullptr);
    //}

    vkDestroySurfaceKHR(vulkan_context.instance, vulkan_context.surface, nullptr);
    vkDestroyInstance(vulkan_context.instance, nullptr);

    glfwDestroyWindow(window_info.window);

    glfwTerminate();
}




void update_vertex_buffer_update(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context, VERTEX_DYNAMIC_INFO& vertex_info)
{
    //TODO: so while not happening rn, we can use an offset to only copy the new data, instead of the whole buffer over
    //so instead of copy buffer we can use copy_buffer_region, which ill have to double check how it works
    if (!vertex_info.vertex_buffer_should_update) return;

    // Only copy we are currently using vertices data
    VkDeviceSize current_vertex_data_size = sizeof(vertices[0]) * vertex_info.dynamic_vertices.size();

    //map and copy data
    //NOTE: YOU TECHNICALLY DONT NEED TO MAP AND UNMAP THE DATA, YOU CAN JUST COPY THE DATA OVER

    //vkMapMemory(vulkan_context.logical_device, command_buffer_context.vertex_staging_buffer_memory, 0, vertex_buffer_capacity, 0,
     //           &data_vertex);
    memcpy(buffer_context.data_vertex, vertex_info.dynamic_vertices.data(), current_vertex_data_size);
    //vkUnmapMemory(vulkan_context.logical_device, command_buffer_context.vertex_staging_buffer_memory);

    //transfer from storage buffer/cpu buffer to gpu buffer
    buffer_copy(vulkan_context, command_buffer_context, buffer_context.vertex_staging_buffer, buffer_context.vertex_buffer,
                current_vertex_data_size);
    //last param in copy_buffer used to be vertex_buffer_capacity and index_buffer_capacity respectively


    VkDeviceSize current_index_data_size = sizeof(indices[0]) * vertex_info.dynamic_indices.size();

    //vkMapMemory(vulkan_context.logical_device, command_buffer_context.index_staging_buffer_memory, 0, index_buffer_capacity, 0, &data_index);
    memcpy(buffer_context.data_index, vertex_info.dynamic_indices.data(), current_index_data_size);
    //vkUnmapMemory(vulkan_context.logical_device, command_buffer_context.index_staging_buffer_memory);

    buffer_copy(vulkan_context, command_buffer_context, buffer_context.index_staging_buffer, buffer_context.index_buffer,
                current_index_data_size);

    vertex_info.vertex_buffer_should_update = false;

}

void copy_buffer_region(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context,
                        VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                        VkDeviceSize srcOffset, VkDeviceSize dstOffset)
{
    // Create a temporary command buffer for the copy operation
    VkCommandBufferAllocateInfo command_buffer_allocation_info{};
    command_buffer_allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocation_info.commandPool = command_buffer_context.command_pool;
    command_buffer_allocation_info.commandBufferCount = 1;

    VkCommandBuffer temp_command_buffer;
    vkAllocateCommandBuffers(vulkan_context.logical_device, &command_buffer_allocation_info, &temp_command_buffer);

    // Begin recording the command buffer
    VkCommandBufferBeginInfo command_buffer_begin_info{};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(temp_command_buffer, &command_buffer_begin_info);

    // Set up the copy region with specified offsets and size
    VkBufferCopy copy_region{};
    copy_region.srcOffset = srcOffset;
    copy_region.dstOffset = dstOffset;
    copy_region.size = size;

    // Record the copy command
    vkCmdCopyBuffer(temp_command_buffer, srcBuffer, dstBuffer, 1, &copy_region);

    vkEndCommandBuffer(temp_command_buffer);

    // Submit the command buffer and wait for completion
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &temp_command_buffer;

    vkQueueSubmit(vulkan_context.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(vulkan_context.graphics_queue);

    // Clean up the temporary command buffer
    vkFreeCommandBuffers(vulkan_context.logical_device, command_buffer_context.command_pool, 1, &temp_command_buffer);
}






void create_index_buffer_new(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context)
{
    // Create buffer large enough for maximum indices, not just initial indices
    buffer_context.index_buffer_capacity = sizeof(uint16_t) * MAX_INDICES; // Use MAX_INDICES instead of indices.size()

    // Create staging buffer
    buffer_create(&vulkan_context, buffer_context.index_buffer_capacity, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  buffer_context.index_staging_buffer, buffer_context.index_staging_buffer_memory);

    // Only copy initial indices data, but allocate full buffer
    VkDeviceSize initial_data_size = sizeof(indices[0]) * indices.size();

    vkMapMemory(vulkan_context.logical_device, buffer_context.index_staging_buffer_memory, 0, buffer_context.index_buffer_capacity, 0, &buffer_context.data_index);

    // Zero out the entire buffer first
    memset(buffer_context.data_index, 0, buffer_context.index_buffer_capacity);
    // Then copy initial data
    memcpy(buffer_context.data_index, indices.data(), initial_data_size);

    vkUnmapMemory(vulkan_context.logical_device, buffer_context.index_staging_buffer_memory);

    // Create device local buffer with full size
    buffer_create(&vulkan_context, buffer_context.index_buffer_capacity,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_context.index_buffer,
                  buffer_context.index_buffer_memory);

    // Copy entire buffer
    buffer_copy(vulkan_context, command_buffer_context, buffer_context.index_staging_buffer, buffer_context.index_buffer,
                buffer_context.index_buffer_capacity);


    std::cout << "CREATED INDEX BUFFER SUCCESS (Size: " << buffer_context.index_buffer_capacity << " bytes for " << MAX_INDICES <<
            " indices)\n";
}


void create_vertex_buffer_new(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context)
{
    // Create buffer large enough for maximum vertices, not just initial vertices
    buffer_context.vertex_buffer_capacity = sizeof(Vertex) * MAX_VERTICES; // Use MAX_VERTICES instead of vertices.size()


    buffer_create(&vulkan_context, buffer_context.vertex_buffer_capacity, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  buffer_context.vertex_staging_buffer, buffer_context.vertex_staging_buffer_memory);

    // Only copy initial vertices data, but allocate full buffer
    VkDeviceSize initial_data_size = sizeof(vertices[0]) * vertices.size();

    vkMapMemory(vulkan_context.logical_device, buffer_context.vertex_staging_buffer_memory, 0, buffer_context.vertex_buffer_capacity, 0,
                &buffer_context.data_vertex);

    // Zero out the entire buffer first
    memset(buffer_context.data_vertex, 0, buffer_context.vertex_buffer_capacity);
    // Then copy initial data
    memcpy(buffer_context.data_vertex, vertices.data(), initial_data_size);
    vkUnmapMemory(vulkan_context.logical_device, buffer_context.vertex_staging_buffer_memory);

    // Create device local buffer with full size
    buffer_create(&vulkan_context, buffer_context.vertex_buffer_capacity,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_context.vertex_buffer,
                  buffer_context.vertex_buffer_memory);

    // Copy entire buffer (including zeros for unused space)
    buffer_copy(vulkan_context, command_buffer_context, buffer_context.vertex_staging_buffer, buffer_context.vertex_buffer,
                buffer_context.vertex_buffer_capacity);


    std::cout << "CREATED VERTEX BUFFER SUCCESS (Size: " << buffer_context.vertex_buffer_capacity << " bytes for " << MAX_VERTICES <<
            " vertices)\n";
}

void create_ui_graphics_pipeline(vulkan_context& vulkan_context, Graphics_Context& ui_graphics_context, Graphics_Context&
                                 graphics_context_renderpass_only)
{
    //load shaders from file
    std::vector<char> vert_shader_code = read_shader_file("../shaders/uivert.spv");
    std::vector<char> frag_shader_code = read_shader_file("../shaders/uifrag.spv");
    //C:\Users\Adams Humbert\Documents\Clion\VulkanTetris
    //create shader modules for use in the shader stage create info
    VkShaderModule vert_shader_module = create_shader_module(vulkan_context.logical_device, vert_shader_code);
    VkShaderModule fragment_shader_module = create_shader_module(vulkan_context.logical_device, frag_shader_code);

    /*
     // Provided by VK_VERSION_1_0
typedef struct VkPipelineShaderStageCreateInfo {
    VkStructureType                     sType;
    const void*                         pNext;
    VkPipelineShaderStageCreateFlags    flags;
    VkShaderStageFlagBits               stage;
    VkShaderModule                      module;
    const char*                         pName;
    const VkSpecializationInfo*         pSpecializationInfo;
} VkPipelineShaderStageCreateInfo;
     */

    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";


    VkPipelineShaderStageCreateInfo shaderStages[] = {vert_shader_stage_info, frag_shader_stage_info};

    //vertex input assembly, describe the format of the vertex data
    //Bindings: spacing between data and whether the data is per-vertex or per-instance
    //Attribute descriptions: type of the attributes passed to the vertex shader, which binding to load them from and at which offset

    //TODO: VkVertexInputBindingDescription vertex_binding_description= getBindingDescription();
    //TODO: std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

    auto binding_description = getBindingDescription();
    auto attribute_description = getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 1; // the number of pvertexbinding descriptions
    vertex_input_state_create_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_description.
        size());
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_description.data();

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    //rasterizer.pNext;
    //rasterizer.flags;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    // VK_POLYGON_MODE_LINE for wireframes, VK_POLYGON_MODE_POINT for just points, using these require gpu features
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    // counter means positive area is front facing, clockwise means negative area is front facing
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    //rasterizer.depthBiasConstantFactor = 0.0f;
    //rasterizer.depthBiasClamp = 0.0f;
    //rasterizer.depthBiasSlopeFactor = 0.0f;

    //not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    //multisampling.minSampleShading = 1.0f; // Optional
    //multisampling.pSampleMask = nullptr; // Optional
    //multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    //multisampling.alphaToOneEnable = VK_FALSE; // Optional


    //TODO:
    //VkPipelineDepthStencilStateCreateInfo depth_stencil ={};

    //happens after color returns from the fragment shader
    //METHOD:
    //Mix the old and new value to produce a final color
    //Combine the old and new value using a bitwise operation
    /* Both ways showcased below
    * if (blendEnable) {
    finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
    finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
    finalColor = newColor;
    }
    finalColor = finalColor & colorWriteMask;
     */

    //TODO: I should look more into this later, its kinda like photoshop blend modes
    // the most important is the src and dst
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
                                          | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &colorBlendAttachment; // this thing can be a vector
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional


    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    //for resizing the viewport, can be used for blend constants
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();


    //Push Constants
    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(Screen_Size_Push_Constants);

    //used to send info to the vertex/fragment shader, like in uniform buffers, to change shader behavior
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0; // Optional
    pipeline_layout_info.pSetLayouts = nullptr; // Optional
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    if (vkCreatePipelineLayout(vulkan_context.logical_device, &pipeline_layout_info, nullptr, &ui_graphics_context.pipeline_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    VkGraphicsPipelineCreateInfo graphics_pipeline_info{};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shaderStages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState = nullptr;
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = ui_graphics_context.pipeline_layout;
    graphics_pipeline_info.renderPass = graphics_context_renderpass_only.render_pass;
    graphics_pipeline_info.subpass = 0;
    graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_info.basePipelineIndex = -1;


    /*
    VkResult vkCreateGraphicsPipelines(
        VkDevice                                    device,
        VkPipelineCache                             pipelineCache,
        uint32_t                                    createInfoCount,
        const VkGraphicsPipelineCreateInfo*         pCreateInfos,
        const VkAllocationCallbacks*                pAllocator,
        VkPipeline*                                 pPipelines);*/

    vkCreateGraphicsPipelines(vulkan_context.logical_device, VK_NULL_HANDLE, 1, &graphics_pipeline_info, nullptr,
                              &ui_graphics_context.graphics_pipeline);

    vkDestroyShaderModule(vulkan_context.logical_device, fragment_shader_module, nullptr);
    vkDestroyShaderModule(vulkan_context.logical_device, vert_shader_module, nullptr);
    std::cout << "CREATED UI GRAPHICS PIPELINE SUCCESS\n";
}

void create_text_graphics_pipeline(vulkan_context& vulkan_context, Graphics_Context& text_graphics_context,
                                   Graphics_Context& graphics_context_renderpass_only, Descriptor& descriptor)
{
    //load shaders from file
    std::vector<char> vert_shader_code = read_shader_file("../shaders/textvert.spv");
    std::vector<char> frag_shader_code = read_shader_file("../shaders/textfrag.spv");
    //C:\Users\Adams Humbert\Documents\Clion\VulkanTetris
    //create shader modules for use in the shader stage create info
    VkShaderModule vert_shader_module = create_shader_module(vulkan_context.logical_device, vert_shader_code);
    VkShaderModule fragment_shader_module = create_shader_module(vulkan_context.logical_device, frag_shader_code);

    /*
     // Provided by VK_VERSION_1_0
typedef struct VkPipelineShaderStageCreateInfo {
    VkStructureType                     sType;
    const void*                         pNext;
    VkPipelineShaderStageCreateFlags    flags;
    VkShaderStageFlagBits               stage;
    VkShaderModule                      module;
    const char*                         pName;
    const VkSpecializationInfo*         pSpecializationInfo;
} VkPipelineShaderStageCreateInfo;
     */

    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";


    VkPipelineShaderStageCreateInfo shaderStages[] = {vert_shader_stage_info, frag_shader_stage_info};

    //vertex input assembly, describe the format of the vertex data
    //Bindings: spacing between data and whether the data is per-vertex or per-instance
    //Attribute descriptions: type of the attributes passed to the vertex shader, which binding to load them from and at which offset

    //TODO: VkVertexInputBindingDescription vertex_binding_description= getBindingDescription();
    //TODO: std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

    auto binding_description = get_text_binding_description();
    auto attribute_description = get_text_attribute_descriptions();

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 1; // the number of pvertexbinding descriptions
    vertex_input_state_create_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_description.
        size());
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_description.data();

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    //rasterizer.pNext;
    //rasterizer.flags;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    // VK_POLYGON_MODE_LINE for wireframes, VK_POLYGON_MODE_POINT for just points, using these require gpu features
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    // counter means positive area is front facing, clockwise means negative area is front facing
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    //rasterizer.depthBiasConstantFactor = 0.0f;
    //rasterizer.depthBiasClamp = 0.0f;
    //rasterizer.depthBiasSlopeFactor = 0.0f;

    //not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    //multisampling.minSampleShading = 1.0f; // Optional
    //multisampling.pSampleMask = nullptr; // Optional
    //multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    //multisampling.alphaToOneEnable = VK_FALSE; // Optional


    //TODO:
    //VkPipelineDepthStencilStateCreateInfo depth_stencil ={};

    //happens after color returns from the fragment shader
    //METHOD:
    //Mix the old and new value to produce a final color
    //Combine the old and new value using a bitwise operation
    /* Both ways showcased below
    * if (blendEnable) {
    finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
    finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
    finalColor = newColor;
    }
    finalColor = finalColor & colorWriteMask;
     */



    //NOTE: it's kinda like photoshop blend modes
    //blending happens before its passed to the fragment shader
    //When we consider blending two colors, we can call the color already in place the destination
    //and the new color we want to blend with it the source.

    //Enable alpha blending for text rendering

    //best to think about blending with this function:
    // blend(source,destination)=(source⋅sourceBlendFactor)blendFunction(dest⋅destinationBlendFactor)

    //classic “alpha blending” (also called “source-over” compositing).
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE; // ENABLE BLENDING

    /*
    // Standard alpha blending: src_alpha * src_color + (1 - src_alpha) * dst_color
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    */

    // Premultiplied alpha blending, will make the text less blurrier in motion
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    // A_out = A_src + (1 - A_src) * A_dst
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &colorBlendAttachment; // this thing can be a vector
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional


    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    //for resizing the viewport, can be used for blend constants
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();


    //used to send info to the vertex/fragment shader, like in uniform buffers, to change shader behavior
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &descriptor.descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(vulkan_context.logical_device, &pipeline_layout_info, nullptr, &text_graphics_context.pipeline_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    VkGraphicsPipelineCreateInfo graphics_pipeline_info{};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shaderStages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState = nullptr;
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = text_graphics_context.pipeline_layout;
    graphics_pipeline_info.renderPass = graphics_context_renderpass_only.render_pass;
    graphics_pipeline_info.subpass = 0;
    graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_info.basePipelineIndex = -1;


    /*
    VkResult vkCreateGraphicsPipelines(
        VkDevice                                    device,
        VkPipelineCache                             pipelineCache,
        uint32_t                                    createInfoCount,
        const VkGraphicsPipelineCreateInfo*         pCreateInfos,
        const VkAllocationCallbacks*                pAllocator,
        VkPipeline*                                 pPipelines);*/

    vkCreateGraphicsPipelines(vulkan_context.logical_device, VK_NULL_HANDLE, 1, &graphics_pipeline_info, nullptr,
                              &text_graphics_context.graphics_pipeline);

    vkDestroyShaderModule(vulkan_context.logical_device, fragment_shader_module, nullptr);
    vkDestroyShaderModule(vulkan_context.logical_device, vert_shader_module, nullptr);
    std::cout << "CREATED TEXT GRAPHICS PIPELINE SUCCESS\n";
}













