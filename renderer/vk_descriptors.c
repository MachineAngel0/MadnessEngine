#include "vk_descriptors.h"


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
