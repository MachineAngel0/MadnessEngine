#include "spv_reflect.h"

#define max_bindless_resources 4092u
#define max_bindless_bindings 16;


typedef enum descriptor_type
{
    UNIFORM_BUFFER,
    TEXTURE,
    descriptor_type_max,
} descriptor_type;

VkDescriptorType descriptor_type_lookup[descriptor_type_max] = {
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
};


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


    //BINDLESS

    VkDescriptorPoolSize bindless_pool_sizes[] =
    {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = max_bindless_resources
        },
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = max_bindless_resources
        },
        {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = max_bindless_resources
        },
    };

    VkDescriptorPoolCreateInfo bindless_pool_info = {0};
    bindless_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    bindless_pool_info.poolSizeCount = ARRAY_SIZE(bindless_pool_sizes); // number of different pool sizes, we created
    bindless_pool_info.pPoolSizes = bindless_pool_sizes;
    bindless_pool_info.maxSets = ARRAY_SIZE(bindless_pool_sizes) * max_bindless_resources;
    bindless_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
    // |VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT // if we every want to call vkFreeDescriptorSets,

    VkResult bindless_result = vkCreateDescriptorPool(context->device.logical_device, &bindless_pool_info,
                                                      context->allocator,
                                                      &descriptor_pools->bindless_descriptor_pool);
    VK_CHECK(bindless_result);
}

void descriptor_pool_allocator_destroy(vulkan_context* context, descriptor_pool_allocator* descriptor_pools)
{
    vkDestroyDescriptorPool(context->device.logical_device, descriptor_pools->descriptor_pool, NULL);
}

void descriptor_pool_allocator_clear(vulkan_context* context, descriptor_pool_allocator* descriptor_pools)
{
    VK_CHECK(vkResetDescriptorPool(context->device.logical_device, descriptor_pools->descriptor_pool, 0));
}

void descriptor_pool_alloc_bindless(vulkan_context* context, descriptor_pool_allocator* descriptor_pools,
                                    VkDescriptorSetLayout* set_layout, u32* descriptor_set_count,
                                    VkDescriptorSet* out_descriptors)
{
    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT count_info = {0};
    count_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    count_info.descriptorSetCount = *descriptor_set_count;
    // This number is the max allocatable count
    // count_info.pDescriptorCounts = (const u32*)max_bindless_resources;
    const u32 binding_count = max_bindless_resources - 1;
    count_info.pDescriptorCounts = &binding_count;

    VkDescriptorSetAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pools->bindless_descriptor_pool;
    alloc_info.descriptorSetCount = *descriptor_set_count;
    alloc_info.pSetLayouts = set_layout;
    alloc_info.pNext = &count_info;

    VkResult alloc_result = vkAllocateDescriptorSets(context->device.logical_device, &alloc_info, out_descriptors);

    if (alloc_result == VK_ERROR_OUT_OF_POOL_MEMORY || alloc_result == VK_ERROR_FRAGMENTED_POOL)
    {
        FATAL("FAILED TO ALLOCATE FROM DESCRIPTOR POOL, INCREASE POOL SIZE")
    }
    VK_CHECK(alloc_result);
}

/*
void createDescriptorsTexture_reflect_test(vulkan_context* context,
                                           descriptor_pool_allocator* descriptor_pool_allocator,
                                           vulkan_shader_texture* shader_texture)
{
    spirv_reflect_descriptor_set_info* d_set_reflect_info = spriv_reflect_get_descriptor_set(
        NULL, "../renderer/shaders/shader_texture.vert.spv", "../renderer/shaders/shader_texture.frag.spv");


    shader_texture->descriptor_sets = darray_create_reserve(
        VkDescriptorSet, context->swapchain.max_frames_in_flight);
    shader_texture->descriptor_set_count = (u32)context->swapchain.max_frames_in_flight;


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
        descriptor_pool_alloc_bindless(context, descriptor_pool_allocator, &shader_texture->descriptor_set_layout,
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
        bufferInfo.buffer = context->global_uniform_buffers.uniform_buffers[i];
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

void update_descriptors_texture_reflect_test(vulkan_context* context,
                                             descriptor_pool_allocator* descriptor_pool_allocator,
                                             vulkan_shader_texture* shader_texture)
{
    spirv_reflect_descriptor_set_info* d_set_reflect_info = spriv_reflect_get_descriptor_set(
        NULL, "../renderer/shaders/shader_texture.vert.spv", "../renderer/shaders/shader_texture.frag.spv");


    // Where the descriptor set layout is the interface, the descriptor set points to actual data
    // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight
    for (uint32_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        u32 set_count = 1;
        descriptor_pool_alloc_bindless(context, descriptor_pool_allocator, &shader_texture->descriptor_set_layout,
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
        bufferInfo.buffer = context->global_uniform_buffers.uniform_buffers[i];
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
*/


void create_texture_bindless_descriptor_set(vulkan_context* context,
                                            descriptor_pool_allocator* descriptor_pool_allocator,
                                            vulkan_bindless_descriptors* texture_descriptors)
{
    texture_descriptors->descriptor_sets = darray_create_reserve(
        VkDescriptorSet, context->swapchain.max_frames_in_flight);
    texture_descriptors->descriptor_set_count = (u32)context->swapchain.max_frames_in_flight;

    // Descriptor set layouts define the interface between our application and the shader
    // Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
    // So every shader binding should map to one descriptor set layout binding

    //GLOBAL TEXTURE SET LAYOUT

    //SET 1, Layout 0
    VkDescriptorSetLayoutBinding layout_binding = {0};
    //image sampler
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout_binding.binding = 0;
    layout_binding.descriptorCount = max_bindless_resources;
    layout_binding.stageFlags = VK_SHADER_STAGE_ALL;
    layout_binding.pImmutableSamplers = NULL;

    VkDescriptorBindingFlags bindless_flags = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT |
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT |
        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extended_info = {0};
    extended_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    extended_info.bindingCount = 1; // bindingCount is zero or the number of elements in pBindingFlags.
    extended_info.pBindingFlags = &bindless_flags;
    extended_info.pNext = NULL;

    VkDescriptorSetLayoutCreateInfo layout_create_info = {0};
    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.bindingCount = 1;
    layout_create_info.pBindings = &layout_binding;
    layout_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
    layout_create_info.pNext = &extended_info;


    VK_CHECK(vkCreateDescriptorSetLayout(context->device.logical_device, &layout_create_info, 0,
        &texture_descriptors->descriptor_set_layout));

    // Where the descriptor set layout is the interface, the descriptor set points to actual data
    // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight

    for (u32 i = 0; i < texture_descriptors->descriptor_set_count; i++)
    {
        u32 set_count = 1;
        descriptor_pool_alloc_bindless(context, descriptor_pool_allocator, &texture_descriptors->descriptor_set_layout,
                                       &set_count,
                                       &texture_descriptors->descriptor_sets[i]);
    }


    //TODO:
    // darray_free(texture_descriptors->descriptor_sets);
}

void update_global_texture_bindless_descriptor_set(vulkan_context* context,
                                                   vulkan_bindless_descriptors* texture_descriptors,
                                                   Texture* texture, u32 array_index)
{
    //TODO: update vulkan_shader_texture* test_texture, to a Texture* struct type

    VkDescriptorImageInfo image_info = {0}; // for textures
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = texture->texture_image_view;
    image_info.sampler = texture->texture_sampler;

    for (int i = 0; i < texture_descriptors->descriptor_set_count; i++)
    {
        VkWriteDescriptorSet write_descriptor_set = {0};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstSet = texture_descriptors->descriptor_sets[i];
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write_descriptor_set.dstBinding = 0;
        // write_descriptor_set.descriptorCount = max_bindless_resources;
        write_descriptor_set.descriptorCount = 1;
        // is the number of descriptors to update or the number of elements in pimageinfo/pbufferinfo etc...
        write_descriptor_set.dstArrayElement = array_index; // starting element of the array
        write_descriptor_set.pImageInfo = &image_info;
        //
        vkUpdateDescriptorSets(context->device.logical_device, 1,
                               &write_descriptor_set, 0, 0);
    }
}


void create_bindless_uniform_buffer_descriptor_set(vulkan_context* context,
                                                   descriptor_pool_allocator* descriptor_pool_allocator,
                                                   vulkan_bindless_descriptors* uniform_descriptors)
{
    uniform_descriptors->descriptor_sets = darray_create_reserve(
        VkDescriptorSet, context->swapchain.max_frames_in_flight);
    uniform_descriptors->descriptor_set_count = (u32)context->swapchain.max_frames_in_flight;

    // Descriptor set layouts define the interface between our application and the shader
    // Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
    // So every shader binding should map to one descriptor set layout binding

    //GLOBAL TEXTURE SET LAYOUT

    //SET 0, Layout 0
    VkDescriptorSetLayoutBinding layout_binding = {0};
    //image sampler
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.binding = 0;
    layout_binding.descriptorCount = max_bindless_resources;
    layout_binding.stageFlags = VK_SHADER_STAGE_ALL;
    layout_binding.pImmutableSamplers = NULL;

    VkDescriptorBindingFlags bindless_flags = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT |
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT |
        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extended_info = {0};
    extended_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    extended_info.bindingCount = 1; // bindingCount is zero or the number of elements in pBindingFlags.
    extended_info.pBindingFlags = &bindless_flags;
    extended_info.pNext = NULL;

    VkDescriptorSetLayoutCreateInfo layout_create_info = {0};
    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.bindingCount = 1;
    layout_create_info.pBindings = &layout_binding;
    layout_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
    layout_create_info.pNext = &extended_info;


    VK_CHECK(vkCreateDescriptorSetLayout(context->device.logical_device, &layout_create_info, 0,
        &uniform_descriptors->descriptor_set_layout));

    // Where the descriptor set layout is the interface, the descriptor set points to actual data
    // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight

    for (int i = 0; i < uniform_descriptors->descriptor_set_count; i++)
    {
        u32 set_count = 1;
        descriptor_pool_alloc_bindless(context, descriptor_pool_allocator, &uniform_descriptors->descriptor_set_layout,
                                       &set_count,
                                       &uniform_descriptors->descriptor_sets[i]);
    }


    //TODO:
    // darray_free(texture_descriptors->descriptor_sets);
}

void update_uniform_buffer_bindless_descriptor_set(vulkan_context* context,
                                                   vulkan_bindless_descriptors* uniform_descriptors,
                                                   vulkan_buffer* buffer, u64 data_size, u32 array_index)
{
    MASSERT_MSG(buffer->type == BUFFER_TYPE_UNIFORM,
                "update_uniform_buffer_bindless_descriptor_set: NOT A UNIFORM BUFFER TYPE PASSED IN");

    // The buffer's information is passed using a descriptor info structure
    VkDescriptorBufferInfo bufferInfo = {0}; // for uniform buffer
    bufferInfo.buffer = buffer->handle;
    bufferInfo.range = data_size;
    bufferInfo.offset = 0;

    for (int j = 0; j < uniform_descriptors->descriptor_set_count; j++)
    {
        VkWriteDescriptorSet write_descriptor_set = {0};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstSet = uniform_descriptors->descriptor_sets[j];
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_descriptor_set.dstBinding = 0;
        // write_descriptor_set.descriptorCount = max_bindless_resources;
        write_descriptor_set.descriptorCount = 1;
        // is the number of descriptors to update or the number of elements in pimageinfo/pbufferinfo etc...
        write_descriptor_set.dstArrayElement = array_index; // starting element of the array
        write_descriptor_set.pBufferInfo = &bufferInfo;
        //
        vkUpdateDescriptorSets(context->device.logical_device, 1,
                               &write_descriptor_set, 0, 0);
    }
}


void create_bindless_storage_buffer_descriptor_set(vulkan_context* context,
                                                   descriptor_pool_allocator* descriptor_pool_allocator,
                                                   vulkan_bindless_descriptors* storage_descriptors)
{
    storage_descriptors->descriptor_sets = darray_create_reserve(
        VkDescriptorSet, context->swapchain.max_frames_in_flight);
    storage_descriptors->descriptor_set_count = (u32)context->swapchain.max_frames_in_flight;

    // Descriptor set layouts define the interface between our application and the shader
    // Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
    // So every shader binding should map to one descriptor set layout binding

    //GLOBAL TEXTURE SET LAYOUT

    //SET 2, Layout 0
    VkDescriptorSetLayoutBinding layout_binding = {0};
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layout_binding.binding = 0;
    layout_binding.descriptorCount = max_bindless_resources;
    layout_binding.stageFlags = VK_SHADER_STAGE_ALL;
    layout_binding.pImmutableSamplers = NULL;

    VkDescriptorBindingFlags bindless_flags = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT |
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT |
        VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extended_info = {0};
    extended_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    extended_info.bindingCount = 1; // bindingCount is zero or the number of elements in pBindingFlags.
    extended_info.pBindingFlags = &bindless_flags;
    extended_info.pNext = NULL;

    VkDescriptorSetLayoutCreateInfo layout_create_info = {0};
    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.bindingCount = 1;
    layout_create_info.pBindings = &layout_binding;
    layout_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
    layout_create_info.pNext = &extended_info;


    VK_CHECK(vkCreateDescriptorSetLayout(context->device.logical_device, &layout_create_info, 0,
        &storage_descriptors->descriptor_set_layout));

    // Where the descriptor set layout is the interface, the descriptor set points to actual data
    // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight

    for (int i = 0; i < storage_descriptors->descriptor_set_count; i++)
    {
        u32 set_count = 1;
        descriptor_pool_alloc_bindless(context, descriptor_pool_allocator, &storage_descriptors->descriptor_set_layout,
                                       &set_count,
                                       &storage_descriptors->descriptor_sets[i]);
    }


    //TODO:
    // darray_free(texture_descriptors->descriptor_sets);
}


void update_storage_buffer_bindless_descriptor_set(vulkan_context* context,
                                                   vulkan_bindless_descriptors* storage_descriptors,
                                                   vulkan_buffer* buffer, u32 array_index)
{
    MASSERT_MSG(buffer->type == BUFFER_TYPE_CPU_STORAGE,
                "update_storage_buffer_bindless_descriptor_set: NOT A CPU_STORAGE BUFFER TYPE PASSED IN");

    // The buffer's information is passed using a descriptor info structure
    VkDescriptorBufferInfo bufferInfo = {0}; // for uniform buffer
    bufferInfo.buffer = buffer->handle;
    bufferInfo.range = buffer->current_offset;
    bufferInfo.offset = 0;

    for (int j = 0; j < storage_descriptors->descriptor_set_count; j++)
    {
        VkWriteDescriptorSet write_descriptor_set = {0};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstSet = storage_descriptors->descriptor_sets[j];
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write_descriptor_set.dstBinding = 0;
        // write_descriptor_set.descriptorCount = max_bindless_resources;
        write_descriptor_set.descriptorCount = 1;
        // is the number of descriptors to update or the number of elements in pimageinfo/pbufferinfo etc...
        write_descriptor_set.dstArrayElement = array_index; // starting element of the array
        write_descriptor_set.pBufferInfo = &bufferInfo;
        //
        vkUpdateDescriptorSets(context->device.logical_device, 1,
                               &write_descriptor_set, 0, 0);
    }
}
