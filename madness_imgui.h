#ifndef MADNESSENGINE_VULKAN_IMGUI_H
#define MADNESSENGINE_VULKAN_IMGUI_H


#include "dcimgui.h"

#if MPLATFORM_WINDOWS
#include "dcimgui_impl_win32.h"
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#if MPLATFORM_LINUX
#include "dcimgui_impl_glfw.h"
#endif

#include "dcimgui_impl_vulkan.h"

#include "vulkan_struct_types.h"
#include "logger.h"
#include "math_lib.h"

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}


//call after renderer init
void madness_imgui_init(Platform_State* platform_state, Renderer* renderer)
{
    // Setup Dear ImGui context
    CIMGUI_CHECKVERSION();
    ImGuiContext* imgui_context = ImGui_CreateContext(NULL);
    ImGuiIO* io = ImGui_GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable; // IF using Docking Branch
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // IF using Docking Branch
    // Setup Platform/Renderer backends

    //
    ImFont* font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "../z_assets/fonts/arial.ttf",
                                                  18.0f, NULL, NULL);
    //changes the default sizing, helps with blurriness but not by much
    // ImGuiStyle* style = ImGui_GetStyle();
    // style->FontSizeBase = 18.0f;

    ImGui_StyleColorsDark(NULL);
    ImGuiStyle* style = ImGui_GetStyle();
    style->Colors[ImGuiCol_CheckMark]  = (ImVec4){ 1.0f, 0.0f, 0.0f, 1.0f };
    style->Colors[ImGuiCol_TitleBg]  = (ImVec4){0.425f, 0.05f, 0.456f, 1.0f};;
    style->Colors[ImGuiCol_TitleBgActive]  = (ImVec4){0.425f, 0.05f, 0.456f, 1.0f};
    style->Colors[ImGuiCol_Button]  = *(ImVec4*)COLOR4_PURPLE_PALETTE_PURPLE_STRONG.raw;
    style->Colors[ImGuiCol_ButtonActive]  = *(ImVec4*)COLOR4_PURPLE_PALETTE_DARK2.raw;
    style->Colors[ImGuiCol_ButtonHovered]  = *(ImVec4*)COLOR4_PURPLE_PALETTE_PURPLE_LIGHT2.raw;
    style->Colors[ImGuiCol_SliderGrab]  = (ImVec4){0.991f, 0.356f, 0.851f, 0.643f};
    style->Colors[ImGuiCol_SliderGrabActive]  = (ImVec4){0.376f, 0.122f, 0.620f, 1.000f};
    style->Colors[ImGuiCol_Header]  = (ImVec4){0.425f, 0.05f, 0.456f, 1.0f};;
    style->Colors[ImGuiCol_HeaderHovered]  = *(ImVec4*)COLOR4_PURPLE_HEADER_HOVERED.raw;
    style->Colors[ImGuiCol_HeaderActive]  = *(ImVec4*)COLOR4_PURPLE_HEADER_ACTIVE.raw;
    style->Colors[ImGuiCol_Tab]  = (ImVec4){    0.376f, 0.122f, 0.620f, 1.000f};
    style->Colors[ImGuiCol_TabSelected]  = *(ImVec4*)COLOR4_PURPLE_HEADER_ACTIVE.raw;
    style->Colors[ImGuiCol_TabHovered]  = (ImVec4){0.425f, 0.05f, 0.456f, 1.0f};


    if (!platform_init_imgui(platform_state))
    {
        MASSERT_FALSE()
    }

    // cImGui_ImplGlfw_InitForVulkan(GLFWwindow* window, bool install_callbacks);


    /*VkDescriptorPoolSize pool_sizes[] =
    {
        {
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            1000
        },
    };

    VkDescriptorPoolCreateInfo pool_info = {0};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = (uint32_t)ARRAY_SIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkResult result = vkCreateDescriptorPool(
        renderer->logical_device,
        &pool_info,
        NULL,
        &renderer->imgui_descriptor_pool
    );
    VK_CHECK(result);*/

    ImGui_ImplVulkan_InitInfo init_info = {0};
    init_info.Instance = renderer->instance;
    init_info.PhysicalDevice = renderer->physical_device;
    init_info.Device = renderer->logical_device;
    init_info.QueueFamily = renderer->queue_system->graphics_queue_index;
    init_info.Queue = renderer->queue_system->graphics_queue;
    init_info.PipelineCache = renderer->pipeline_cache->handle;


    // Let the backend create its own descriptor pool.
    // init_info.DescriptorPool = renderer->imgui_descriptor_pool;
    init_info.DescriptorPool = VK_NULL_HANDLE;
    init_info.DescriptorPoolSize = 1000;

    init_info.ApiVersion = VK_API_VERSION_1_4;
    init_info.MinImageCount = renderer->swapchain_capabilities.capabilities.minImageCount;
    init_info.ImageCount = renderer->swapchain.image_count;
    init_info.Allocator = NULL;
    //
    init_info.UseDynamicRendering = true;
    init_info.PipelineInfoMain.RenderPass = VK_NULL_HANDLE;
    init_info.PipelineInfoMain.Subpass = 0;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats =
        &renderer->swapchain.surface_format.format;

    init_info.CheckVkResultFn = check_vk_result;


    if (!cImGui_ImplVulkan_Init(&init_info))
    {
        MASSERT_FALSE()
        // vkDestroyDescriptorPool(
        // renderer->logical_device,
        // renderer->imgui_descriptor_pool,
        // NULL
        // );

        // renderer->imgui_descriptor_pool = VK_NULL_HANDLE;
        // return false;
    }
}

//call before shutting anything else in the renderer down
void madness_imgui_shutdown(void)
{
    cImGui_ImplVulkan_Shutdown();
#if MPLATFORM_WINDOWS
    cImGui_ImplWin32_Shutdown();
#endif
#if MPLATFORM_LINUX
    cImGui_ImplGlfw_Shutdown();
#endif

    ImGui_DestroyContext(NULL);
}


void madness_imgui_frame_start()
{
    // (Your code process and dispatch Win32 messages)
    // Start the Dear ImGui frame

#if MPLATFORM_WINDOWS
    cImGui_ImplWin32_NewFrame();
#endif
#if MPLATFORM_LINUX
    cImGui_ImplGlfw_NewFrame();
#endif
    cImGui_ImplVulkan_NewFrame();
    ImGui_NewFrame();

    bool open_window = true;
    ImGui_ShowDemoWindow(&open_window); // Show demo window! :)
}

void madness_imgui_render(Vulkan_Command_Buffer* command_buffer)
{
    ImGui_Render();
    cImGui_ImplVulkan_RenderDrawData(ImGui_GetDrawData(), command_buffer->handle);


    ImGui_UpdatePlatformWindows();
    ImGui_RenderPlatformWindowsDefault();
}


#endif //MADNESSENGINE_VULKAN_IMGUI_H
