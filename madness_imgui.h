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
                                                  20.0f, NULL, NULL);
    //changes the default sizing, helps with blurriness but not by much
    // ImGuiStyle* style = ImGui_GetStyle();
    // style->FontSizeBase = 18.0f;

    ImGui_StyleColorsDark(NULL);
    ImGuiStyle* style = ImGui_GetStyle();
    style->Colors[ImGuiCol_CheckMark] = (ImVec4){1.0f, 0.0f, 0.0f, 1.0f};
    style->Colors[ImGuiCol_TitleBg] = (ImVec4){0.425f, 0.05f, 0.456f, 1.0f};;
    style->Colors[ImGuiCol_TitleBgActive] = (ImVec4){0.425f, 0.05f, 0.456f, 1.0f};
    style->Colors[ImGuiCol_Button] = *(ImVec4*)COLOR4_PURPLE_PALETTE_PURPLE_STRONG.raw;
    style->Colors[ImGuiCol_ButtonActive] = *(ImVec4*)COLOR4_PURPLE_PALETTE_DARK2.raw;
    style->Colors[ImGuiCol_ButtonHovered] = *(ImVec4*)COLOR4_PURPLE_PALETTE_PURPLE_LIGHT2.raw;
    style->Colors[ImGuiCol_SliderGrab] = (ImVec4){0.991f, 0.356f, 0.851f, 0.643f};
    style->Colors[ImGuiCol_SliderGrabActive] = (ImVec4){0.376f, 0.122f, 0.620f, 1.000f};
    style->Colors[ImGuiCol_Header] = (ImVec4){0.425f, 0.05f, 0.456f, 1.0f};;
    style->Colors[ImGuiCol_HeaderHovered] = *(ImVec4*)COLOR4_PURPLE_HEADER_HOVERED.raw;
    style->Colors[ImGuiCol_HeaderActive] = *(ImVec4*)COLOR4_PURPLE_HEADER_ACTIVE.raw;
    style->Colors[ImGuiCol_Tab] = (ImVec4){0.376f, 0.122f, 0.620f, 1.000f};
    style->Colors[ImGuiCol_TabSelected] = *(ImVec4*)COLOR4_PURPLE_HEADER_ACTIVE.raw;
    style->Colors[ImGuiCol_TabHovered] = (ImVec4){0.425f, 0.05f, 0.456f, 1.0f};


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
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo.depthAttachmentFormat = renderer->depth_format;

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
    PROFILE_ZONE(madness_imgui_frame_start)
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

    PROFILE_ZONE_END(madness_imgui_frame_start)

}

void madness_imgui_render(Vulkan_Command_Buffer* command_buffer)
{
    PROFILE_ZONE(madness_imgui_render)
    ImGui_Render();
    cImGui_ImplVulkan_RenderDrawData(ImGui_GetDrawData(), command_buffer->handle);


    ImGui_UpdatePlatformWindows();
    ImGui_RenderPlatformWindowsDefault();
    PROFILE_ZONE_END(madness_imgui_render)

}

void ImGui_String(const String* string)
{
    ImGui_TextUnformattedEx(string->chars, string->chars + string->length);
}

bool ImGui_file_picker(const char* label, char* out_path, size_t path_size, const char* start_dir)
{
    bool changed = false;

    ImGui_PushID(label); /* overload names: PushID(str) / PushIDInt / PushIDPtr */

    if (ImGui_Button(label))
    {
        char picked[MADNESS_MAX_PATH];
        if (platform_open_file_dialogue(picked, start_dir))
        {
            size_t written = snprintf(out_path, path_size, "%s", picked);
            changed = true;
        }
    }

    ImGui_SameLine();
    ImGui_SetNextItemWidth(-FLT_MIN); /* fill the rest of the row; needs <float.h> */
    ImGui_InputText("##path", out_path, path_size, ImGuiInputTextFlags_ReadOnly);
    /* if InputText takes extra callback args in your header, use ImGui_InputTextEx(..., NULL, NULL) */

    ImGui_PopID();
    return changed;
}


void ImGui_Combo_String(const char* label,  u32 path_index, const String* string_array, const u32 string_count)
{

    const char* combo_preview_value = string_array[path_index].chars;
    if (ImGui_BeginCombo(label, combo_preview_value, 0))
    {
        for (u32 i = 0; i < string_count; i++)
        {
            const bool is_selected = (path_index == i);
            if (ImGui_SelectableEx(string_array[i].chars, is_selected, 0, (ImVec2){0, 0}))
            {
                path_index = i;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
            {
                ImGui_SetItemDefaultFocus();
            }
        }
        ImGui_EndCombo();
    }
}




bool Imgui_Reflect_Using_Data(Reflection_Registry* reflection_registry, Reflection_Runtime_Struct struct_info,
                                   void* passing_data, const char* id, Allocator* allocator)
{
    //TODO: return a struct of what exactly changed, like if it was texture or just a param
    ImGui_TextUnformattedEx(struct_info.name, NULL);

    for (u32 field_index = 0; field_index < struct_info.field_count; field_index++)
    {
        ImGui_PushIDInt(field_index);

        Reflection_Runtime_Struct_Field field_info = struct_info.fields[field_index];
        void* data = (u8*)passing_data + field_info.offset;

        String_Builder* builder = string_builder_create(256, allocator);
        string_builder_append_c_string(builder, field_info.name);
        string_builder_append_c_string(builder, ": ");
        string_builder_append_c_string(builder, id);
        string_builder_append_c_string(builder, ": ");

        const char* custom_name = string_builder_to_c_string(builder);
        String* custom_name_str = string_builder_to_string(builder, allocator);

        switch (field_info.type)
        {
        case REFLECTION_TYPE_INVALID:
            break;
        case REFLECTION_TYPE_U8:
            ImGui_DragScalar(custom_name, ImGuiDataType_U8, data);
            break;
        case REFLECTION_TYPE_U16:
            ImGui_DragScalar(custom_name, ImGuiDataType_U16, data);
            break;
        case REFLECTION_TYPE_U32:
            ImGui_DragScalar(custom_name, ImGuiDataType_U32, data);
            break;
        case REFLECTION_TYPE_U64:
            ImGui_DragScalar(custom_name, ImGuiDataType_U64, data);
            break;
        case REFLECTION_TYPE_S8:
            ImGui_DragScalar(custom_name, ImGuiDataType_S8, data);
            break;
        case REFLECTION_TYPE_S16:
            ImGui_DragScalar(custom_name, ImGuiDataType_S16, data);
            break;
        case REFLECTION_TYPE_S32:
            ImGui_DragScalar(custom_name, ImGuiDataType_S32, data);
            break;
        case REFLECTION_TYPE_S64:
            ImGui_DragScalar(custom_name, ImGuiDataType_S64, data);
            break;
        case REFLECTION_TYPE_F32:
            ImGui_DragScalar(custom_name, ImGuiDataType_Float, data);
            break;
        case REFLECTION_TYPE_F64:
            ImGui_DragScalar(custom_name, ImGuiDataType_Double, data);
            break;
        case REFLECTION_TYPE_SIZE_T:
            ImGui_DragScalar(custom_name, ImGuiDataType_U64, data);
            break;

        case REFLECTION_TYPE_BOOL:
            ImGui_Checkbox(custom_name, data);
            break;
        case REFLECTION_TYPE_STRING:
            // madness_ui_text_box(*custom_name);
            char buff[1024];
            ImGui_InputText(custom_name, buff, 1024, ImGuiInputTextFlags_ReadOnly);
            break;
        case REFLECTION_TYPE_PATH_STRING:
            ImGui_String(custom_name_str);
            ImGui_SameLine();
            Path_String* path_string = *(Path_String**)data;
            ImGui_String(path_string);
            /*madness_ui_combo_box_string(*custom_name, *(Path_String**)data,
                                        madness_ui->asset_list_scan_reference->strings,
                                        madness_ui->asset_list_scan_reference->count);*/

            // TODO: probably would make sense to use a file picker at some point
            // madness_ui_file_picker(*custom_name);


            break;
        case REFLECTION_TYPE_CHAR:
            char buff2[1024];
            ImGui_InputText(custom_name, buff2, 1024, ImGuiInputTextFlags_ReadOnly);
            break;
        case REFLECTION_TYPE_ENUM:
            Reflection_Runtime_Enum runtime_enum = reflection_registry_get_enum(
                reflection_registry, field_info.type_name);
            switch (runtime_enum.type)
            {
            case Reflection_Enum_Type_Normal:
                ImGui_ComboChar(custom_name, data, runtime_enum.enum_names,
                                runtime_enum.count);
                break;
            case Reflection_Enum_Type_Bitflag:
                for (u32 i = 0; i < runtime_enum.count; i++)
                {
                    String_Builder* builder_bitflag = string_builder_create(256, madness_ui->frame_allocator);
                    string_builder_append_c_string(builder_bitflag, runtime_enum.enum_names[i]);
                    string_builder_append_c_string(builder_bitflag, "_");
                    string_builder_append_c_string(builder_bitflag, id);
                    ImGui_Checkbox(string_builder_to_c_string(builder_bitflag), &runtime_enum.bitflag_values[i]);
                }
                break;
            }

            break;
        case REFLECTION_TYPE_STRUCT:
            Reflection_Runtime_Struct reflection_struct = reflection_registry_get_struct(reflection_registry, field_info.type_name);
            Imgui_Reflect_Using_Data(reflection_registry, reflection_struct, data, id, allocator);
            break;

        case REFLECTION_TYPE_VEC2:
            ImGui_DragFloat2(custom_name, data);
            break;
        case REFLECTION_TYPE_VEC3:
            ImGui_DragFloat3(custom_name, data);
            break;
        case REFLECTION_TYPE_VEC4:
            ImGui_DragFloat4(custom_name, data);
            break;
        case REFLECTION_TYPE_CHAR_STRING:
            break;
        case REFLECTION_TYPE_MAT3:
            break;
        case REFLECTION_TYPE_MAT4:
            break;
        case REFLECTION_TYPE_UUID:
            MADNESS_UUID* uuid = data;
            ImGui_DragScalar(custom_name, ImGuiDataType_U64, &uuid->high);
            ImGui_PushID("high");
            ImGui_DragScalar(custom_name, ImGuiDataType_U64, &uuid->low);
            ImGui_PopID();

            break;
        case REFLECTION_TYPE_MAX:
            break;
        }
        ImGui_PopID();
    }
    return false;
}


bool ImGui_Reflect_Material(Asset_Registry* asset_registry, Reflection_Registry* reflection_registry,
                            Reflection_Runtime_Struct struct_info, void* passing_data, const char* id,
                            Asset_List_Scan* texture_asset_list_scan, Allocator* allocator)
{
    //TODO: return a struct of what exactly changed, like if it was texture or just a param
    ImGui_TextUnformattedEx(struct_info.name, NULL);


    for (u32 field_index = 0; field_index < struct_info.field_count; field_index++)
    {
        Reflection_Runtime_Struct_Field field_info = struct_info.fields[field_index];
        void* data = (u8*)passing_data + field_info.offset;

        String_Builder* builder = string_builder_create(256, allocator);
        string_builder_append_c_string(builder, field_info.name);
        string_builder_append_c_string(builder, ": ");
        string_builder_append_c_string(builder, id);
        string_builder_append_c_string(builder, ": ");
        String* custom_name = string_builder_to_string(builder, allocator);
        const char* custom_name2 = string_builder_to_c_string(builder);

        ImGui_PushIDInt(field_index);

        switch (field_info.type)
        {
        case REFLECTION_TYPE_INVALID:
            break;
        case REFLECTION_TYPE_U8:
            // madness_ui_u8(*custom_name, data, 1);
            ImGui_DragInt(custom_name2, data);
            break;
        case REFLECTION_TYPE_U16:
            ImGui_DragInt(custom_name2, data);
            break;
        case REFLECTION_TYPE_U32:
            ImGui_DragInt(custom_name2, data);
            break;
        case REFLECTION_TYPE_U64:
            ImGui_DragInt(custom_name2, data);
            break;
        case REFLECTION_TYPE_S8:
            ImGui_DragInt(custom_name2, data);
            break;
        case REFLECTION_TYPE_S16:
            ImGui_DragInt(custom_name2, data);
            break;
        case REFLECTION_TYPE_S32:
            ImGui_DragInt(custom_name2, data);
            break;
        case REFLECTION_TYPE_S64:
            ImGui_DragInt(custom_name2, data);
            break;
        case REFLECTION_TYPE_F32:
            ImGui_DragFloat(custom_name2, data);
            break;
        case REFLECTION_TYPE_F64:
            ImGui_DragFloat(custom_name2, data);
            break;
        case REFLECTION_TYPE_SIZE_T:
            ImGui_DragInt(custom_name2, data);
            break;
        case REFLECTION_TYPE_BOOL:
            ImGui_Checkbox(custom_name2, data);
            break;
        case REFLECTION_TYPE_STRING:
            // madness_ui_text_box(*custom_name);
            char buff[1024];
            ImGui_InputText(custom_name2, buff, 1024, ImGuiInputTextFlags_ReadOnly);
            break;
        case REFLECTION_TYPE_PATH_STRING:
            ImGui_String(custom_name);
            ImGui_SameLine();
            Path_String* path_string = *(Path_String**)data;
            ImGui_String(path_string);
            /*madness_ui_combo_box_string(*custom_name, *(Path_String**)data,
                                        madness_ui->asset_list_scan_reference->strings,
                                        madness_ui->asset_list_scan_reference->count);*/

            // TODO: probably would make sense to use a file picker at some point
            // madness_ui_file_picker(*custom_name);


            break;
        case REFLECTION_TYPE_CHAR:
            // madness_ui_text_box(*custom_name);
            char buff2[1024];
            ImGui_InputText(custom_name2, buff2, 1024, ImGuiInputTextFlags_ReadOnly);
            break;
        case REFLECTION_TYPE_ENUM:
            Reflection_Runtime_Enum runtime_enum = reflection_registry_get_enum(
                reflection_registry, field_info.type_name);
            switch (runtime_enum.type)
            {
            case Reflection_Enum_Type_Normal:
                ImGui_ComboChar(custom_name2, data, runtime_enum.enum_names,
                                runtime_enum.count);
                break;
            case Reflection_Enum_Type_Bitflag:

                for (u32 i = 0; i < runtime_enum.count; i++)
                {
                    String_Builder* builder_bitflag = string_builder_create(256, madness_ui->frame_allocator);
                    string_builder_append_c_string(builder_bitflag, runtime_enum.enum_names[i]);
                    string_builder_append_c_string(builder_bitflag, "_");
                    string_builder_append_c_string(builder_bitflag, id);
                    ImGui_Checkbox(string_builder_to_c_string(builder_bitflag), &runtime_enum.bitflag_values[i]);
                }

                break;
            }

            break;
        case REFLECTION_TYPE_STRUCT:
            // madness_ui_reflection_runtime_registry(reflection_registry, field_info.type_name, id);
            break;
        case REFLECTION_TYPE_VEC2:
            ImGui_DragFloat2(custom_name2, data);
            break;
        case REFLECTION_TYPE_VEC3:
            ImGui_DragFloat3(custom_name2, data);
            break;
        case REFLECTION_TYPE_VEC4:
            ImGui_DragFloat4(custom_name2, data);
            break;
        case REFLECTION_TYPE_CHAR_STRING:
            break;
        case REFLECTION_TYPE_MAT3:
            MASSERT(false);
            break;
        case REFLECTION_TYPE_MAT4:
            MASSERT(false);
            break;
        case REFLECTION_TYPE_UUID:

            //TODO: this is just an idea, you would want to get the name first before changing it
            // madness_ui_file_picker(*custom_name);

            MADNESS_UUID uuid = *(MADNESS_UUID*)data;
            // madness_ui_string(*asset_meta_data.engine_path);


            u32 selected_texture = 0;

            Asset_Record asset_meta_data = {0};
            if (asset_registry_exists_by_uuid(asset_registry,
                                              uuid,
                                              &asset_meta_data))
            {
                for (u32 i = 0; i < texture_asset_list_scan->count; i++)
                {
                    if (string_compare(&texture_asset_list_scan->strings[i], asset_meta_data.engine_path))
                    {
                        selected_texture = i;
                    }
                }
            }

            // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
            const char* combo_preview_value = texture_asset_list_scan->strings[selected_texture].chars;
            if (ImGui_BeginCombo("combo 1", combo_preview_value, 0))
            {
                for (u32 i = 0; i < texture_asset_list_scan->count; i++)
                {
                    ImGui_String(&texture_asset_list_scan->strings[i]);

                    const bool is_selected = (selected_texture == i);
                    if (ImGui_SelectableEx(texture_asset_list_scan->strings[i].chars, is_selected, 0, (ImVec2){0, 0}))
                    {
                        selected_texture = i;

                        Asset_Record new_asset_metadata = {0};
                        if (asset_registry_exists_by_engine_path(asset_registry,
                                                                 &texture_asset_list_scan->strings[selected_texture],
                                                                 &new_asset_metadata))
                        {
                            memcpy(data, &new_asset_metadata.uuid, sizeof(MADNESS_UUID));
                        }
                        else
                        {
                            madness_ui_string(STRING("ERROR, TEXTURE NOT FOUND IN ASSET REGISTRY"));
                        }
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui_SetItemDefaultFocus();
                }
                ImGui_EndCombo();
            }

            ImGui_DragInt("high", &uuid.high);
            ImGui_DragInt("low", &uuid.low);

            break;
        case REFLECTION_TYPE_MAX:
            break;
        }
        ImGui_PopID();
    }

    return
        false;
}


#endif //MADNESSENGINE_VULKAN_IMGUI_H
