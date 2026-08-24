#ifndef VULKAN_ALLOCATOR_H
#define VULKAN_ALLOCATOR_H


#include "vulkan_struct_types.h"


/**
 * @link https://docs.vulkan.org/refpages/latest/refpages/source/VkAllocationCallbacks.html
 * link to how this is supposed to be defined
 */


// testing rn

const char* vulkan_malloc_describe_scope(VkSystemAllocationScope allocationScope)
{
    switch (allocationScope)
    {
    case VK_SYSTEM_ALLOCATION_SCOPE_COMMAND:
        return "Command";
        break;
    case VK_SYSTEM_ALLOCATION_SCOPE_OBJECT:
        return "Object";
        break;
    case VK_SYSTEM_ALLOCATION_SCOPE_CACHE:
        return "Cache";
        break;
    case VK_SYSTEM_ALLOCATION_SCOPE_DEVICE:
        return "Device";
        break;
    case VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE:
        return "Instance";
        break;
    case VK_SYSTEM_ALLOCATION_SCOPE_MAX_ENUM:
        return "Object";
        break;
    }
    return "nothing";
}

void* vulkan_malloc_alloc(void* pUserData,
                          size_t size,
                          size_t alignment,
                          VkSystemAllocationScope allocationScope)
{
    printf("malloc - size: %zu alignment: %zu, scope: %s\n", size, alignment,
           vulkan_malloc_describe_scope(allocationScope));

    return _aligned_malloc(size, alignment);
}

void* vulkan_malloc_realloc(void* pUserData,
                            void* pOriginal,
                            size_t size,
                            size_t alignment,
                            VkSystemAllocationScope allocationScope)
{
    printf("remalloc - size: %zu alignment: %zu, scope: %s", size, alignment,
           vulkan_malloc_describe_scope(allocationScope));
    return _aligned_realloc(pOriginal, size, alignment);
}

void vulkan_malloc_free(void* pUserData,
                        void* pMemory)
{
    printf("free\n");
    _aligned_free(pMemory);
}

void vulkan_malloc_internal_alloc_notification(void* pUserData,
                                               size_t size,
                                               VkInternalAllocationType allocationType,
                                               VkSystemAllocationScope allocationScope)
{
    FATAL("malloc alloc notification - size: %zu, scope: %s", size, vulkan_malloc_describe_scope(allocationScope));
}

void vulkan_malloc_internal_free_notification(void* pUserData,
                                              size_t size,
                                              VkInternalAllocationType allocationType,
                                              VkSystemAllocationScope allocationScope)
{
    FATAL("malloc free notification - size: %zu, scope: %s", size, vulkan_malloc_describe_scope(allocationScope));
}


Vulkan_CPU_Allocator* vulkan_allocator_init(Renderer* renderer)
{
    Vulkan_CPU_Allocator* allocator = allocator_alloc(&renderer->allocator, sizeof(Vulkan_CPU_Allocator));


    renderer->vk_allocator_callback->pfnAllocation = vulkan_malloc_alloc;
    renderer->vk_allocator_callback->pfnFree = vulkan_malloc_free;
    renderer->vk_allocator_callback->pfnInternalAllocation = vulkan_malloc_internal_alloc_notification;
    renderer->vk_allocator_callback->pfnInternalFree = vulkan_malloc_internal_free_notification;
    renderer->vk_allocator_callback->pfnReallocation = vulkan_malloc_realloc;
    renderer->vk_allocator_callback->pUserData = allocator;

    return allocator;
}

void* vulkan_allocator_alloc(void* pUserData,
                             size_t size,
                             size_t alignment,
                             VkSystemAllocationScope allocationScope)
{
}

void* vulkan_allocator_realloc(void* pUserData,
                               void* pOriginal,
                               size_t size,
                               size_t alignment,
                               VkSystemAllocationScope allocationScope)
{
}

void vulkan_allocator_free(void* pUserData,
                           void* pMemory)
{
}

void vulkan_allocator_internal_alloc_notification(void* pUserData,
                                                  size_t size,
                                                  VkInternalAllocationType allocationType,
                                                  VkSystemAllocationScope allocationScope)
{
}

void vulkan_allocator_internal_free_notification(void* pUserData,
                                                 size_t size,
                                                 VkInternalAllocationType allocationType,
                                                 VkSystemAllocationScope allocationScope)
{
}


#endif //VULKAN_ALLOCATOR_H
