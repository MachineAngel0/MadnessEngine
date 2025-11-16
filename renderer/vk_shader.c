#include "vk_shader.h"

#include "c_string.h"
#include "logger.h"
#include "math_types.h"


#define BUILTIN_SHADER_NAME_OBJECT "shader_object"


bool vulkan_object_shader_create(vulkan_context* context, vulkan_object_shader* out_shader)
{
    // Shader module init per stage.
    char stage_type_strs[OBJECT_SHADER_STAGE_COUNT][5] = {"vert", "frag"};
    VkShaderStageFlagBits stage_types[OBJECT_SHADER_STAGE_COUNT] = {
        VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT
    };

    for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i)
    {
        if (!create_shader_module(context, BUILTIN_SHADER_NAME_OBJECT, stage_type_strs[i], stage_types[i], i,
                                  out_shader->stages))
        {
            M_ERROR("Unable to create %s shader module for '%s'.", stage_type_strs[i], BUILTIN_SHADER_NAME_OBJECT);
            return false;
        }
    }

    // TODO: Descriptors


    // Pipeline creation

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = (f32) context->framebuffer_height;
    viewport.width = (f32) context->framebuffer_width;
    viewport.height = -(f32) context->framebuffer_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;


    // Scissor
    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context->framebuffer_width;
    scissor.extent.height = context->framebuffer_height;


    // Attributes
    u32 offset = 0;
    const i32 attribute_count = 1;
    // VkVertexInputAttributeDescription attribute_descriptions[attribute_count];
    VkVertexInputAttributeDescription attribute_descriptions[1];


    // Position
    // VkFormat formats[attribute_count] = {
    VkFormat formats[] = {
        VK_FORMAT_R32G32B32_SFLOAT
    };


    // u64 sizes[attribute_count] = {
    u64 sizes[] = {
        sizeof(vec3)
    };


    for (u32 i = 0; i < attribute_count; ++i)
    {
        attribute_descriptions[i].binding = 0; // binding index - should match binding desc
        attribute_descriptions[i].location = i; // attrib location
        attribute_descriptions[i].format = formats[i];
        attribute_descriptions[i].offset = offset;
        offset += sizes[i];
    }


    // TODO: Desciptor set layouts.


    // Stages
    // NOTE: Should match the number of shader->stages.
    VkPipelineShaderStageCreateInfo stage_create_infos[OBJECT_SHADER_STAGE_COUNT];
    memset(stage_create_infos, 0, sizeof(stage_create_infos));


    for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i)
    {
        stage_create_infos[i].sType = out_shader->stages[i].shader_stage_create_info.sType;
        stage_create_infos[i] = out_shader->stages[i].shader_stage_create_info;
    }


    if (!vulkan_graphics_pipeline_create(
        context,
        &context->main_renderpass,
        attribute_count,
        attribute_descriptions,
        0,
        0,
        OBJECT_SHADER_STAGE_COUNT,
        stage_create_infos,
        viewport,
        scissor,
        false,
        &out_shader->pipeline))
    {
        M_ERROR("Failed to load graphics pipeline for object shader.");


        return false;
    }

    return true;
}

void vulkan_object_shader_destroy(vulkan_context* context, struct vulkan_object_shader* shader)
{
    vulkan_pipeline_destroy(context, &shader->pipeline);
    // Destroy shader modules.
    for (u32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; ++i)
    {
        vkDestroyShaderModule(context->device.logical_device, shader->stages[i].handle, context->allocator);
        shader->stages[i].handle = 0;
    }
}

void vulkan_object_shader_use(vulkan_context* context, struct vulkan_object_shader* shader)
{
    u32 image_index = context->image_index;
    vulkan_pipeline_bind(&context->graphics_command_buffers[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS, &shader->pipeline);
}

b8 create_shader_module(vulkan_context* context, const char* name, const char* type_str,
                        VkShaderStageFlagBits shader_stage_flag, u32 stage_index, vulkan_shader_stage* shader_stages)
{
    // Build file name.
    char file_name[512];


    c_string_format(file_name, "C:/Users/Adams Humbert/Documents/Clion/MadnessEngine/renderer/shaders/%s.%s.spv", name, type_str);
    memset(&shader_stages[stage_index].create_info, 0, sizeof(VkShaderModuleCreateInfo));

    shader_stages[stage_index].create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    // Obtain file handle.
    file_handle handle;


    if (!filesystem_open(file_name, FILE_MODE_READ, true, &handle))
    {
        M_ERROR("Unable to read shader module: %s.", file_name);
        return false;
    }

    // Read the entire file as binary.
    u64 size = 0;
    u8* file_buffer = 0;

    if (!filesystem_read_all_bytes(&handle, &file_buffer, &size))
    {
        M_ERROR("Unable to binary read shader module: %s.", file_name);
        return false;
    }


    shader_stages[stage_index].create_info.codeSize = size;
    shader_stages[stage_index].create_info.pCode = (u32 *) file_buffer;

    // Close the file.
    filesystem_close(&handle);

    VK_CHECK(vkCreateShaderModule(
        context->device.logical_device,
        &shader_stages[stage_index].create_info,
        context->allocator,
        &shader_stages[stage_index].handle));


    // Shader stage info
    memset(&shader_stages[stage_index].shader_stage_create_info, 0, sizeof(VkPipelineShaderStageCreateInfo));
    shader_stages[stage_index].shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[stage_index].shader_stage_create_info.stage = shader_stage_flag;
    shader_stages[stage_index].shader_stage_create_info.module = shader_stages[stage_index].handle;
    shader_stages[stage_index].shader_stage_create_info.pName = "main";

    if (file_buffer)
    {
        free(file_buffer);
        file_buffer = 0;
    }

    return true;
}
