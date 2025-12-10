#include "vk_descriptors.h"

#include "arena.h"
#include "array.h"
#include "filesystem.h"

void descriptor_pool_allocator_init(vulkan_context* context, descriptor_pool_allocator* descriptor_pools)
{
    /* list of all available types
        VK_DESCRIPTOR_TYPE_SAMPLER = 0,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
        VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
        VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
        VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
        VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK = 1000138000,
        VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR = 1000150000,
        VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV = 1000165000,
        VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM = 1000440000,
        VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM = 1000440001,
        VK_DESCRIPTOR_TYPE_TENSOR_ARM = 1000460000,
        VK_DESCRIPTOR_TYPE_MUTABLE_EXT = 1000351000,
        VK_DESCRIPTOR_TYPE_PARTITIONED_ACCELERATION_STRUCTURE_NV = 1000570000,
        VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
        VK_DESCRIPTOR_TYPE_MUTABLE_VALVE = VK_DESCRIPTOR_TYPE_MUTABLE_EXT,
        VK_DESCRIPTOR_TYPE_MAX_ENUM = 0x7FFFFFFF
        */

    //upfront allocation
    //FUTURE: most likely I can tailor each type to a different max size
    u32 max_pool_sizes = 4092;

    VkDescriptorPoolSize pool_sizes[2] =
    {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = max_pool_sizes
        },
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = max_pool_sizes
        },

    };

    VkDescriptorPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = ARRAY_SIZE(pool_sizes); // number of different pool sizes, we created
    poolInfo.pPoolSizes = pool_sizes;
    poolInfo.maxSets = ARRAY_SIZE(pool_sizes) * max_pool_sizes;
    // poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT; // TODO: bindless


    VkResult result = vkCreateDescriptorPool(context->device.logical_device, &poolInfo, context->allocator,
                                             &descriptor_pools->descriptor_pool);
    VK_CHECK(result);
}

void descriptor_pool_allocator_destroy(vulkan_context* context, descriptor_pool_allocator* descriptor_pools)
{
    vkDestroyDescriptorPool(context->device.logical_device, descriptor_pools->descriptor_pool, NULL);
}

void descriptor_pool_allocator_clear(vulkan_context* context, descriptor_pool_allocator* descriptor_pools)
{
    VK_CHECK(vkResetDescriptorPool(context->device.logical_device, descriptor_pools->descriptor_pool, 0));
}

void descriptor_pool_alloc(vulkan_context* context, descriptor_pool_allocator* descriptor_pools,
                           VkDescriptorSetLayout* set_layout, const u32* descriptor_set_count,
                           VkDescriptorSet* out_descriptors)
{
    VkDescriptorSetAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pools->descriptor_pool;
    alloc_info.descriptorSetCount = *descriptor_set_count;
    alloc_info.pSetLayouts = set_layout;

    VkResult alloc_result = vkAllocateDescriptorSets(context->device.logical_device, &alloc_info, out_descriptors);

    if (alloc_result == VK_ERROR_OUT_OF_POOL_MEMORY || alloc_result == VK_ERROR_FRAGMENTED_POOL)
    {
        FATAL("FAILED TO ALLOCATE FROM DESCRIPTOR POOL, INCREASE POOL SIZE")
    }
    VK_CHECK(alloc_result);
}


#include <stdio.h>

const char* SpvReflectDescriptorType_TABLE[] = {
    "SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE",
    "SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC",
    "SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT",
    "SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR",
};


spirv_refect_info* spriv_reflection_testing(const char* shader_path)
{
    //TODO: add a type, vert, frag, (or frag and vert combined) compute etc

    spirv_refect_info* out_reflect_info = malloc(sizeof(spirv_refect_info));

    file_read_data shader_data = {0};


    filesystem_open_and_return_bytes("../renderer/shaders/spirv_reflect_test.vert.spv", &shader_data);


    SpvReflectShaderModule module = {0};
    SpvReflectResult result = spvReflectCreateShaderModule(shader_data.size, shader_data.data, &module);
    MASSERT_MSG(result == SPV_REFLECT_RESULT_SUCCESS, "SpvReflectCreateShaderModule failed");
    TRACE("SpvReflectCreateShaderModule result SUCCESS")

    // this has an additional flag param i dont need rn
    // SpvReflectResult result2 = spvReflectCreateShaderModule2();
    // SpvReflectModuleFlags a;
    // Enumerate and extract shader's input variables


    uint32_t input_var_count = 0;
    spvReflectEnumerateInputVariables(&module, &input_var_count, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectInterfaceVariable** inputs_variables =
            (SpvReflectInterfaceVariable **) malloc(input_var_count * sizeof(SpvReflectInterfaceVariable *));
    spvReflectEnumerateInputVariables(&module, &input_var_count, inputs_variables);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    for (u32 i = 0; i < input_var_count; i++)
    {
        //format of the in values
        TRACE("FORMAT %d", inputs_variables[i]->format);
        TRACE("spirv_id %d", inputs_variables[i]->spirv_id);
        TRACE("stride %d", inputs_variables[i]->array.stride);
    }


    uint32_t d_set_count = 0;
    result = spvReflectEnumerateDescriptorSets(&module, &d_set_count, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectDescriptorSet** d_sets =
            (SpvReflectDescriptorSet **) malloc(d_set_count * sizeof(SpvReflectDescriptorSet *));
    result = spvReflectEnumerateDescriptorSets(&module, &d_set_count, d_sets);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);


    DEBUG("D-Set COUNTsss %d", d_set_count);
    for (u32 i = 0; i < d_set_count; i++)
    {
        //format of the in values
        TRACE("D-Set set %d", d_sets[i]->set);
        TRACE("D-Set BINDING COUNT %d", d_sets[i]->binding_count);
        for (u32 j = 0; j < d_sets[i]->binding_count; j++)
        {
            TRACE("BINDING NAME: %s", d_sets[i]->bindings[j]->name);
            DEBUG("BINDING NUMBER %d", d_sets[i]->bindings[j]->binding);
            INFO("type %d, NAME: %s", d_sets[i]->bindings[j]->descriptor_type,
                 SpvReflectDescriptorType_TABLE[d_sets[i]->bindings[j]->descriptor_type]);
        }
    }


    //NOTE: kinda no need for this since this data is contained in d_sets
    uint32_t d_binding_count = 0;
    spvReflectEnumerateDescriptorBindings(&module, &d_binding_count, NULL);
    SpvReflectDescriptorBinding** d_bindings =
            (SpvReflectDescriptorBinding **) malloc(d_binding_count * sizeof(SpvReflectDescriptorBinding *));
    spvReflectEnumerateDescriptorBindings(&module, &d_binding_count, d_bindings);


    uint32_t output_var_count = 0;
    spvReflectEnumerateOutputVariables(&module, &output_var_count, NULL);
    SpvReflectInterfaceVariable** ouput_variables =
            (SpvReflectInterfaceVariable **) malloc(output_var_count * sizeof(SpvReflectInterfaceVariable *));
    spvReflectEnumerateOutputVariables(&module, &output_var_count, ouput_variables);

    uint32_t push_constant_count = 0;
    spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, NULL);
    SpvReflectBlockVariable** push_constants =
            (SpvReflectBlockVariable **) malloc(push_constant_count * sizeof(SpvReflectBlockVariable *));
    spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, push_constants);

    uint32_t specialized_constant_count = 0;
    spvReflectEnumerateSpecializationConstants(&module, &specialized_constant_count, NULL);
    SpvReflectSpecializationConstant** specialized_constants =
            (SpvReflectSpecializationConstant **) malloc(
                specialized_constant_count * sizeof(SpvReflectSpecializationConstant *));
    spvReflectEnumerateSpecializationConstants(&module, &specialized_constant_count, specialized_constants);

    // free(d_sets);
    // free(d_bindings);
    // free(specialized_constants);
    // free(push_constants);
    // free(ouput_variables);
    // free(inputs_variables);

    spvReflectDestroyShaderModule(&module);


    return out_reflect_info;
}

typedef struct
{
    uint32_t flag;
    const char* name;
} ShaderStageName;

static const ShaderStageName ShaderStageTable[] = {
    {SPV_REFLECT_SHADER_STAGE_VERTEX_BIT, "VERTEX"},
    {SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT, "TESSELLATION_CONTROL"},
    {SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, "TESSELLATION_EVALUATION"},
    {SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT, "GEOMETRY"},
    {SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT, "FRAGMENT"},
    {SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT, "COMPUTE"},
    {SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV, "TASK_NV"},
    {SPV_REFLECT_SHADER_STAGE_TASK_BIT_EXT, "TASK_EXT"},
    {SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV, "MESH_NV"},
    {SPV_REFLECT_SHADER_STAGE_MESH_BIT_EXT, "MESH_EXT"},
    {SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR, "RAYGEN_KHR"},
    {SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR, "ANY_HIT_KHR"},
    {SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, "CLOSEST_HIT_KHR"},
    {SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR, "MISS_KHR"},
    {SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR, "INTERSECTION_KHR"},
    {SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR, "CALLABLE_KHR"}
};
static const int ShaderStageTableCount = sizeof(ShaderStageTable) / sizeof(ShaderStageTable[0]);

const char* GetShaderStageName(uint32_t flag)
{
    for (int i = 0; i < ShaderStageTableCount; ++i)
    {
        if (ShaderStageTable[i].flag == flag)
            return ShaderStageTable[i].name;
    }
    return "UNKNOWN_STAGE";
}

spirv_reflect_descriptor_set_info* spriv_reflection_testing_just_descriptor_set(
    Frame_Arena* arena, const char* shader_path)
{
    //TODO: arena
    spirv_reflect_descriptor_set_info* out_reflect_info = malloc(sizeof(spirv_reflect_descriptor_set_info));

    file_read_data vert_shader_data = {0};
    file_read_data frag_shader_data = {0};

    filesystem_open_and_return_bytes("../renderer/shaders/shader_texture.vert.spv", &vert_shader_data);
    filesystem_open_and_return_bytes("../renderer/shaders/shader_texture.frag.spv", &frag_shader_data);


    SpvReflectShaderModule vert_module = {0};
    SpvReflectResult result = spvReflectCreateShaderModule(vert_shader_data.size, vert_shader_data.data, &vert_module);
    MASSERT_MSG(result == SPV_REFLECT_RESULT_SUCCESS, "SpvReflectCreateShaderModule VERT failed");
    TRACE("SpvReflectCreateShaderModule VERT result SUCCESS")

    SpvReflectShaderModule frag_module = {0};
    result = spvReflectCreateShaderModule(frag_shader_data.size, frag_shader_data.data, &frag_module);
    MASSERT_MSG(result == SPV_REFLECT_RESULT_SUCCESS, "SpvReflectCreateShaderModule FRAG failed");
    TRACE("SpvReflectCreateShaderModule FRAG result SUCCESS")

    INFO("STAGE TYPE %p, %s", vert_module.shader_stage, GetShaderStageName(vert_module.shader_stage));
    INFO("STAGE TYPE %p, %s", frag_module.shader_stage, GetShaderStageName(frag_module.shader_stage));


    //vert
    uint32_t d_set_count_vert = 0;
    result = spvReflectEnumerateDescriptorSets(&vert_module, &d_set_count_vert, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectDescriptorSet** d_sets_vert =
            (SpvReflectDescriptorSet **) malloc(d_set_count_vert * sizeof(SpvReflectDescriptorSet *));
    result = spvReflectEnumerateDescriptorSets(&vert_module, &d_set_count_vert, d_sets_vert);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

    //frag
    uint32_t d_set_count_frag = 0;
    result = spvReflectEnumerateDescriptorSets(&frag_module, &d_set_count_frag, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectDescriptorSet** d_sets_frag =
            (SpvReflectDescriptorSet **) malloc(d_set_count_frag * sizeof(SpvReflectDescriptorSet *));
    result = spvReflectEnumerateDescriptorSets(&frag_module, &d_set_count_frag, d_sets_frag);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

    out_reflect_info->descriptor_set_count = d_set_count_vert + d_set_count_frag;
    out_reflect_info->stage_flags = darray_create_reserve(SpvReflectShaderStageFlagBits,
                                                          out_reflect_info->descriptor_set_count);
    out_reflect_info->binding_number = darray_create_reserve(u32, out_reflect_info->descriptor_set_count);
    out_reflect_info->descriptor_set_types = darray_create_reserve(SpvReflectDescriptorType,
                                                                   out_reflect_info->descriptor_set_count);

    //DEBUG
    TRACE("D-Set COUNT VERT %d", d_set_count_vert);
    for (u32 i = 0; i < d_set_count_vert; i++)
    {
        darray_push(out_reflect_info->stage_flags, vert_module.shader_stage);

        //format of the in values
        TRACE("D-Set BINDING COUNT %d", d_sets_vert[i]->binding_count);
        for (u32 j = 0; j < d_sets_vert[i]->binding_count; j++)
        {
            darray_push(out_reflect_info->binding_number, d_sets_vert[i]->bindings[j]->binding);
            darray_push(out_reflect_info->descriptor_set_types, d_sets_vert[i]->bindings[j]->descriptor_type);

            TRACE("BINDING NAME: %s", d_sets_vert[i]->bindings[j]->name);
            DEBUG("BINDING NUMBER %d", d_sets_vert[i]->bindings[j]->binding);
            DEBUG("SET BINDING NUMBER %d", d_sets_vert[i]->bindings[j]->set);

            INFO("type %d, NAME: %s", d_sets_vert[i]->bindings[j]->descriptor_type,
                 SpvReflectDescriptorType_TABLE[d_sets_vert[i]->bindings[j]->descriptor_type]);
        }
        TRACE("D-Set set %d", d_sets_vert[i]->set);
    }

    TRACE("D-Set COUNT FRAG %d", d_set_count_frag);
    for (u32 i = 0; i < d_set_count_frag; i++)
    {
        darray_push(out_reflect_info->stage_flags, frag_module.shader_stage);

        TRACE("D-Set FRAG BINDING COUNT %d", d_sets_frag[i]->binding_count);
        for (u32 j = 0; j < d_sets_frag[i]->binding_count; j++)
        {
            darray_push(out_reflect_info->binding_number, d_sets_frag[i]->bindings[j]->binding);
            darray_push(out_reflect_info->descriptor_set_types, d_sets_frag[i]->bindings[j]->descriptor_type);


            TRACE("BINDING NAME FRAG: %s", d_sets_frag[i]->bindings[j]->name);
            DEBUG("BINDING NUMBER %d", d_sets_frag[i]->bindings[j]->binding);
            DEBUG("SET BINDING NUMBER %d", d_sets_frag[i]->bindings[j]->set);
            INFO("type %d, NAME FRAG: %s", d_sets_frag[i]->bindings[j]->descriptor_type,
                 SpvReflectDescriptorType_TABLE[d_sets_frag[i]->bindings[j]->descriptor_type]);
        }
        TRACE("D-Set FRAG set %d", d_sets_frag[i]->set);
    }


    // free(d_sets_vert);
    // free(d_sets_frag);

    spvReflectDestroyShaderModule(&vert_module);


    return out_reflect_info;
}


void createDescriptorsTexture_reflect_test(vulkan_context* context,
                                           descriptor_pool_allocator* descriptor_pool_allocator,
                                           vulkan_shader_texture* shader_texture)
{
    spirv_reflect_descriptor_set_info* d_set_reflect_info = spriv_reflection_testing_just_descriptor_set(NULL, "blah");

    shader_texture->descriptor_sets = darray_create_reserve(
        VkDescriptorSet, context->swapchain.max_frames_in_flight);
    shader_texture->descriptor_set_count = (u32) context->swapchain.max_frames_in_flight;


    // Descriptor set layouts define the interface between our application and the shader
    // Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
    // So every shader binding should map to one descriptor set layout binding


    VkDescriptorSetLayoutBinding* layoutBinding = darray_create_reserve(VkDescriptorSetLayoutBinding,
                                                                        d_set_reflect_info->descriptor_set_count);
    for (int i = 0; i < d_set_reflect_info->descriptor_set_count; i++)
    {
        //uniform buffer
        layoutBinding[i].binding = d_set_reflect_info->binding_number[i];
        layoutBinding[i].descriptorType = d_set_reflect_info->descriptor_set_types[i];
        layoutBinding[i].descriptorCount = 1;
        //TODO:  this can stay as 1 until we use bindless, then we have to use the max value
        layoutBinding[i].stageFlags = d_set_reflect_info->stage_flags[i];
    }


    VkDescriptorSetLayoutCreateInfo descriptor_layout_ci = {0};
    descriptor_layout_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout_ci.bindingCount = d_set_reflect_info->descriptor_set_count;
    descriptor_layout_ci.pBindings = layoutBinding;


    VK_CHECK(
        vkCreateDescriptorSetLayout(context->device.logical_device, &descriptor_layout_ci, 0, &shader_texture->
            descriptor_set_layout));

    // Where the descriptor set layout is the interface, the descriptor set points to actual data
    // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight
    for (uint32_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        u32 set_count = 1;
        descriptor_pool_alloc(context, descriptor_pool_allocator, &shader_texture->descriptor_set_layout,
                              &set_count,
                              &shader_texture->descriptor_sets[i]);

        // Update the descriptor set determining the shader binding points
        // For every binding point used in a shader there needs to be one
        // descriptor set matching that binding point
        VkWriteDescriptorSet* writeDescriptorSet = darray_create_reserve(
            VkWriteDescriptorSet, d_set_reflect_info->descriptor_set_count);

        //TODO: these values (uniform buffer/texture) are either going to be seperate functions or enums switches

        // The buffer's information is passed using a descriptor info structure
        VkDescriptorBufferInfo bufferInfo = {0}; // for uniform buffer
        bufferInfo.buffer = context->default_shader_info.global_uniform_buffers.uniform_buffers[i];
        bufferInfo.range = sizeof(uniform_buffer_object);
        bufferInfo.offset = 0;

        VkDescriptorImageInfo image_info = {0}; // for texture
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = shader_texture->texture_test_object.texture_image_view;
        image_info.sampler = shader_texture->texture_test_object.texture_sampler;

        for (int j = 0; j < d_set_reflect_info->descriptor_set_count; j++)
        {
            writeDescriptorSet[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet[j].dstSet = shader_texture->descriptor_sets[i];
            writeDescriptorSet[j].descriptorType = d_set_reflect_info->descriptor_set_types[j];
            writeDescriptorSet[j].dstBinding = d_set_reflect_info->binding_number[j];
            writeDescriptorSet[j].descriptorCount = 1;
            writeDescriptorSet[j].dstArrayElement = 0;

            switch (d_set_reflect_info->descriptor_set_types[j])
            {
                case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: // texture
                    writeDescriptorSet[j].pImageInfo = &image_info;
                    break;
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: // uniform buffer
                    writeDescriptorSet[j].pBufferInfo = &bufferInfo;
                    break;
                default:
                    WARN("DESCRIPTOR SET CREATE: TYPE NOT YET SUPPORTED")
            }
        }
        vkUpdateDescriptorSets(context->device.logical_device, d_set_reflect_info->descriptor_set_count,
                               writeDescriptorSet, 0, 0);
        darray_free(writeDescriptorSet);

    }


}


void updateDescriptorsTexture_reflect_test(vulkan_context* context,
                                           descriptor_pool_allocator* descriptor_pool_allocator,
                                           vulkan_shader_texture* shader_texture)
{
    spirv_reflect_descriptor_set_info* d_set_reflect_info = spriv_reflection_testing_just_descriptor_set(NULL, "blah");

    // Where the descriptor set layout is the interface, the descriptor set points to actual data
    // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight
    for (uint32_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        u32 set_count = 1;
        descriptor_pool_alloc(context, descriptor_pool_allocator, &shader_texture->descriptor_set_layout,
                              &set_count,
                              &shader_texture->descriptor_sets[i]);

        // Update the descriptor set determining the shader binding points
        // For every binding point used in a shader there needs to be one
        // descriptor set matching that binding point
        VkWriteDescriptorSet* writeDescriptorSet = darray_create_reserve(
            VkWriteDescriptorSet, d_set_reflect_info->descriptor_set_count);

        //TODO: these values (uniform buffer/texture) are either going to be seperate functions or enums switches

        // The buffer's information is passed using a descriptor info structure
        VkDescriptorBufferInfo bufferInfo = {0}; // for uniform buffer
        bufferInfo.buffer = context->default_shader_info.global_uniform_buffers.uniform_buffers[i];
        bufferInfo.range = sizeof(uniform_buffer_object);
        bufferInfo.offset = 0;

        VkDescriptorImageInfo image_info = {0}; // for texture
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = shader_texture->texture_test_object.texture_image_view;
        image_info.sampler = shader_texture->texture_test_object.texture_sampler;

        for (int j = 0; j < d_set_reflect_info->descriptor_set_count; j++)
        {
            writeDescriptorSet[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet[j].dstSet = shader_texture->descriptor_sets[i];
            writeDescriptorSet[j].descriptorType = d_set_reflect_info->descriptor_set_types[j];
            writeDescriptorSet[j].dstBinding = d_set_reflect_info->binding_number[j];
            writeDescriptorSet[j].descriptorCount = 1;
            writeDescriptorSet[j].dstArrayElement = 0;

            switch (d_set_reflect_info->descriptor_set_types[j])
            {
                case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: // texture
                    writeDescriptorSet[j].pImageInfo = &image_info;
                    break;
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: // uniform buffer
                    writeDescriptorSet[j].pBufferInfo = &bufferInfo;
                    break;
                default:
                    WARN("DESCRIPTOR SET CREATE: TYPE NOT YET SUPPORTED")
            }
        }
        vkUpdateDescriptorSets(context->device.logical_device, d_set_reflect_info->descriptor_set_count,
                               writeDescriptorSet, 0, 0);
        darray_free(writeDescriptorSet);

    }
}
void updateDescriptorsTexture_reflect_test_just_texture(vulkan_context* context,
                                           descriptor_pool_allocator* descriptor_pool_allocator,
                                           vulkan_shader_texture* shader_texture)
{
spirv_reflect_descriptor_set_info* d_set_reflect_info = spriv_reflection_testing_just_descriptor_set(NULL, "blah");

    // Where the descriptor set layout is the interface, the descriptor set points to actual data
    // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight
    for (uint32_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        u32 set_count = 1;
        descriptor_pool_alloc(context, descriptor_pool_allocator, &shader_texture->descriptor_set_layout,
                              &set_count,
                              &shader_texture->descriptor_sets[i]);

        // Update the descriptor set determining the shader binding points
        // For every binding point used in a shader there needs to be one
        // descriptor set matching that binding point
        VkWriteDescriptorSet* writeDescriptorSet = darray_create_reserve(
            VkWriteDescriptorSet, d_set_reflect_info->descriptor_set_count);

        //TODO: these values (uniform buffer/texture) are either going to be seperate functions or enums switches

        // The buffer's information is passed using a descriptor info structure
        VkDescriptorBufferInfo bufferInfo = {0}; // for uniform buffer
        bufferInfo.buffer = context->default_shader_info.global_uniform_buffers.uniform_buffers[i];
        bufferInfo.range = sizeof(uniform_buffer_object);
        bufferInfo.offset = 0;

        VkDescriptorImageInfo image_info = {0}; // for texture
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = shader_texture->texture_test_object.texture_image_view;
        image_info.sampler = shader_texture->texture_test_object.texture_sampler;

        for (int j = 0; j < d_set_reflect_info->descriptor_set_count; j++)
        {
            writeDescriptorSet[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet[j].dstSet = shader_texture->descriptor_sets[i];
            writeDescriptorSet[j].descriptorType = d_set_reflect_info->descriptor_set_types[j];
            writeDescriptorSet[j].dstBinding = d_set_reflect_info->binding_number[j];
            writeDescriptorSet[j].descriptorCount = 1;
            writeDescriptorSet[j].dstArrayElement = 0;

            switch (d_set_reflect_info->descriptor_set_types[j])
            {
                case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: // texture
                    writeDescriptorSet[j].pImageInfo = &image_info;
                    break;
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: // uniform buffer
                    writeDescriptorSet[j].pBufferInfo = &bufferInfo;
                    break;
                default:
                    WARN("DESCRIPTOR SET CREATE: TYPE NOT YET SUPPORTED")
            }
        }
        vkUpdateDescriptorSets(context->device.logical_device, d_set_reflect_info->descriptor_set_count,
                               writeDescriptorSet, 0, 0);
        darray_free(writeDescriptorSet);

    }


}