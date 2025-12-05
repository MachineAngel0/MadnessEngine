#include "vk_descriptors.h"

#include "filesystem.h"


void descriptor_pool_allocator_init(vulkan_context* context, descriptor_pool_allocator* descriptor_pools)
{
    descriptor_pools->ready_pools = darray_create_reserve(VkDescriptorPool, 1);
    descriptor_pools->full_pools = darray_create_reserve(VkDescriptorPool, 1);
    descriptor_pools->sets_per_pool = 4092; // arbitrary number
}


void descriptor_pool_allocator_clear(vulkan_context* context, descriptor_pool_allocator* descriptor_pools)
{
    u64 full_pool_size = darray_get_size(descriptor_pools->full_pools);
    u64 ready_pool_size = darray_get_size(descriptor_pools->ready_pools);
    for (u64 i = 0; i < full_pool_size; i++)
    {
        vkResetDescriptorPool(context->device.logical_device, descriptor_pools->full_pools[i], 0);
    }
    for (u64 i = 0; i < ready_pool_size; i++)
    {
        vkResetDescriptorPool(context->device.logical_device, descriptor_pools->ready_pools[i], 0);
    }
}

void descriptor_pool_allocator_destroy(vulkan_context* context, descriptor_pool_allocator* descriptor_pools)
{
    u64 full_pool_size = darray_get_size(descriptor_pools->full_pools);
    u64 ready_pool_size = darray_get_size(descriptor_pools->ready_pools);
    for (u64 i = 0; i < full_pool_size; i++)
    {
        vkDestroyDescriptorPool(context->device.logical_device, descriptor_pools->full_pools[i], NULL);
    }
    for (u64 i = 0; i < ready_pool_size; i++)
    {
        vkDestroyDescriptorPool(context->device.logical_device, descriptor_pools->ready_pools[i], NULL);
    }

    darray_free(descriptor_pools->full_pools);
    darray_free(descriptor_pools->ready_pools);
}


VkDescriptorPool* descriptor_pool_create_return(vulkan_context* context, descriptor_pool_allocator* descriptor_pools,
                                                VkDescriptorType* types, u64 types_size)
{
    //TODO: decide if this is what i want or if i want another interface for arrays
    Array* pool_size_array = array_create(sizeof(VkDescriptorPoolSize), types_size);
    VkDescriptorPoolSize* sizes = (VkDescriptorPoolSize *) pool_size_array->data;
    for (u64 i = 0; i < pool_size_array->capacity; i++)
    {
        sizes[i].type = types[i];
        sizes[i].descriptorCount = (uint32_t) context->swapchain.max_frames_in_flight;
    }


    //this should be an array
    VkDescriptorPoolSize poolSize = {0};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER // uniform buffer
    // VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE // texture image
    poolSize.descriptorCount = (uint32_t) context->swapchain.max_frames_in_flight;

    VkDescriptorPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = descriptor_pools->sets_per_pool;

    VkDescriptorPool* out_pool = {0};
    VkResult result = vkCreateDescriptorPool(context->device.logical_device, &poolInfo, context->allocator,
                                             out_pool);
    VK_CHECK(result);

    return out_pool;
}


void vulkan_descriptor_pool_allocate(vulkan_context* context, descriptor_pool_allocator* descriptor_pools,
                                     VkDescriptorSetLayout* set_layout, VkDescriptorType* types,
                                     VkDescriptorSet* out_descriptors)
{
    VkDescriptorSetAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pools->ready_pools[darray_get_num_count(descriptor_pools->ready_pools)];
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = set_layout;

    VkResult alloc_result = vkAllocateDescriptorSets(context->device.logical_device, &alloc_info, out_descriptors);

    if (alloc_result == VK_ERROR_OUT_OF_POOL_MEMORY || alloc_result == VK_ERROR_FRAGMENTED_POOL)
    {
        {
            //add from ready pool to full pool
            VkDescriptorPool* back = darray_pop_return(descriptor_pools->ready_pools);
            darray_push(descriptor_pools->full_pools, back);

            // TODO: this line is wrong, magic number, types dont make too much sense for me rn,
            // do you ever use multiple types realistically?
            VkDescriptorPool* new_pool = descriptor_pool_create_return(context, descriptor_pools, types, 10);

            //update the pool ref
            alloc_info.descriptorPool = *new_pool;

            VK_CHECK(vkAllocateDescriptorSets(context->device.logical_device, &alloc_info,
                out_descriptors));
        }
    }
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


//Todo: create an out struct for all the data i would want
typedef struct spirv_refect_info
{
    int descriptor_set_count;
    int descriptor_binding_count;
    //etc...
} spirv_refect_info;

void spriv_reflection_testing(vulkan_context* context, vulkan_shader_texture* shader_texture)
{
    file_read_data vert_data = {0};

    filesystem_open_and_return_bytes("../renderer/shaders/spirv_reflect_test.vert.spv", &vert_data);


    SpvReflectShaderModule module = {0};
    SpvReflectResult result = spvReflectCreateShaderModule(vert_data.size, vert_data.data, &module);
    MASSERT_MSG(result == SPV_REFLECT_RESULT_SUCCESS, "SpvReflectCreateShaderModule failed");
    FATAL("SpvReflectCreateShaderModule result SUCCESS")

    // this has an additional flag param i dont need rn
    // SpvReflectResult result2 = spvReflectCreateShaderModule2();
    // SpvReflectModuleFlags a;
    // Enumerate and extract shader's input variables

    uint32_t var_count = 0;
    result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectInterfaceVariable** input_vars =
            (SpvReflectInterfaceVariable **) malloc(var_count * sizeof(SpvReflectInterfaceVariable *));
    result = spvReflectEnumerateInputVariables(&module, &var_count, input_vars);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    for (u32 i = 0; i < var_count; i++)
    {
        //format of the in values
        FATAL("FORMAT %d", input_vars[i]->format);
        FATAL("spirv_id %d", input_vars[i]->spirv_id);
        FATAL("stride %d", input_vars[i]->array.stride);
    }


    uint32_t d_set_count = 0;
    result = spvReflectEnumerateDescriptorSets(&module, &d_set_count, NULL);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectDescriptorSet** d_sets =
            (SpvReflectDescriptorSet **) malloc(var_count * sizeof(SpvReflectDescriptorSet *));
    result = spvReflectEnumerateDescriptorSets(&module, &d_set_count, d_sets);
    MASSERT(result == SPV_REFLECT_RESULT_SUCCESS);


    for (u32 i = 0; i < d_set_count; i++)
    {
        //format of the in values
        FATAL("D-Set BINDING COUNT %d", d_sets[i]->binding_count);
        for (u32 j = 0; j < d_sets[i]->binding_count; j++)
        {
            FATAL("BINDING NAME: %s", d_sets[i]->bindings[j]->name);
            INFO("type %d, NAME: %s", d_sets[i]->bindings[j]->descriptor_type,
                 SpvReflectDescriptorType_TABLE[d_sets[i]->bindings[j]->descriptor_type]);
        }
        FATAL("D-Set set %d", d_sets[i]->set);
        // FATAL("stride %d", d_sets[i]->binding_count);
    }

    uint32_t d_binding_count = 0;
    spvReflectEnumerateDescriptorBindings(&module, &d_binding_count, NULL);
    SpvReflectDescriptorBinding** d_bindings =
            (SpvReflectDescriptorBinding **) malloc(var_count * sizeof(SpvReflectDescriptorBinding *));
    spvReflectEnumerateDescriptorBindings(&module, &d_binding_count, d_bindings);


    uint32_t input_var_count = 0;
    spvReflectEnumerateInputVariables(&module, &input_var_count, NULL);
    SpvReflectInterfaceVariable** inputs_variables =
            (SpvReflectInterfaceVariable **) malloc(var_count * sizeof(SpvReflectInterfaceVariable *));
    spvReflectEnumerateInputVariables(&module, &input_var_count, inputs_variables);

    uint32_t output_var_count = 0;
    spvReflectEnumerateOutputVariables(&module, &output_var_count, NULL);
    SpvReflectInterfaceVariable** ouput_variables =
            (SpvReflectInterfaceVariable **) malloc(var_count * sizeof(SpvReflectInterfaceVariable *));
    spvReflectEnumerateOutputVariables(&module, &output_var_count, ouput_variables);

    uint32_t push_constant_count = 0;
    spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, NULL);
    SpvReflectBlockVariable** push_constants =
            (SpvReflectBlockVariable **) malloc(var_count * sizeof(SpvReflectBlockVariable *));
    spvReflectEnumeratePushConstantBlocks(&module, &push_constant_count, push_constants);

    uint32_t specialized_constant_count = 0;
    spvReflectEnumerateSpecializationConstants(&module, &specialized_constant_count, NULL);
    SpvReflectSpecializationConstant** specialized_constants =
            (SpvReflectSpecializationConstant **) malloc(var_count * sizeof(SpvReflectSpecializationConstant *));
    spvReflectEnumerateSpecializationConstants(&module, &specialized_constant_count, specialized_constants);

    free(d_bindings);
    free(inputs_variables);
    free(ouput_variables);
    free(push_constants);
    free(specialized_constants);

    spvReflectDestroyShaderModule(&module);

    /*

        shader_texture->descriptor_sets = darray_create_reserve(
            VkDescriptorSet, context->swapchain.max_frames_in_flight);
        shader_texture->descriptor_set_count = (u32) context->swapchain.max_frames_in_flight;

        // Descriptors are allocated from a pool, that tells the implementation how many and what types of descriptors we are going to use (at maximum)
        VkDescriptorPoolSize descriptor_pools[2] = {0};
        // This example only one descriptor type (uniform buffer)
        descriptor_pools[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // We have one buffer (and as such descriptor) per frame
        descriptor_pools[0].descriptorCount = context->swapchain.max_frames_in_flight;
        //TEXTURE
        descriptor_pools[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_pools[1].descriptorCount = context->swapchain.max_frames_in_flight;

        // Create the global descriptor pool
        // All descriptors used in this example are allocated from this pool
        VkDescriptorPoolCreateInfo descriptor_pool_create_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        descriptor_pool_create_info.poolSizeCount = 2;
        descriptor_pool_create_info.pPoolSizes = descriptor_pools;
        // Set the max. number of descriptor sets that can be requested from this pool (requesting beyond this limit will result in an error)
        // Our sample will create one set per uniform buffer per frame
        descriptor_pool_create_info.maxSets = (uint32_t) context->swapchain.max_frames_in_flight;
        VK_CHECK(
            vkCreateDescriptorPool(context->device.logical_device, &descriptor_pool_create_info, 0,
                &context->shader_texture.descriptor_pool));

        // Descriptor set layouts define the interface between our application and the shader
        // Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
        // So every shader binding should map to one descriptor set layout binding

        // Binding 0: Uniform buffer (Vertex shader)
        VkDescriptorSetLayoutBinding layoutBinding[2] = {0};
        //uniform buffer
        layoutBinding[0].binding = 0;
        layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBinding[0].descriptorCount = 1;
        layoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        //Binding 1: Sampler/Texture
        layoutBinding[1].binding = 1;
        layoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBinding[1].descriptorCount = 1;
        layoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo descriptorLayoutCI = {0};
        descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorLayoutCI.bindingCount = 2;
        descriptorLayoutCI.pBindings = layoutBinding;


        VK_CHECK(
            vkCreateDescriptorSetLayout(context->device.logical_device, &descriptorLayoutCI, 0, &shader_texture->
                descriptor_set_layout));

        // Where the descriptor set layout is the interface, the descriptor set points to actual data
        // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight
        for (uint32_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
        {
            VkDescriptorSetAllocateInfo allocInfo = {0};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = shader_texture->descriptor_pool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &shader_texture->descriptor_set_layout;
            VK_CHECK(
                vkAllocateDescriptorSets(context->device.logical_device, &allocInfo, &shader_texture->descriptor_sets[i]));

            // Update the descriptor set determining the shader binding points
            // For every binding point used in a shader there needs to be one
            // descriptor set matching that binding point
            VkWriteDescriptorSet writeDescriptorSet[2] = {0};
            writeDescriptorSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            // The buffer's information is passed using a descriptor info structure
            VkDescriptorBufferInfo bufferInfo = {0};
            bufferInfo.buffer = context->default_shader_info.global_uniform_buffers.uniform_buffers[i];
            bufferInfo.range = sizeof(uniform_buffer_object);
            bufferInfo.offset = 0;

            // Binding 0 : Uniform buffer
            writeDescriptorSet[0].dstSet = shader_texture->descriptor_sets[i];
            writeDescriptorSet[0].descriptorCount = 1;
            writeDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescriptorSet[0].pBufferInfo = &bufferInfo;
            writeDescriptorSet[0].dstBinding = 0;
            writeDescriptorSet[1].dstArrayElement = 0;

            VkDescriptorImageInfo image_info = {0};
            image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView = shader_texture->texture_test_object.texture_image_view;
            image_info.sampler = shader_texture->texture_test_object.texture_sampler;
            // Binding 1 : TEXTURE
            writeDescriptorSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet[1].dstSet = shader_texture->descriptor_sets[i];
            writeDescriptorSet[1].descriptorCount = 1;
            writeDescriptorSet[1].dstBinding = 1;
            writeDescriptorSet[1].dstArrayElement = 0;
            writeDescriptorSet[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writeDescriptorSet[1].pImageInfo = &image_info;

            vkUpdateDescriptorSets(context->device.logical_device, 2, writeDescriptorSet, 0, 0);
        }
        */
}
