#include <stdlib.h>

#include <vulkan/vulkan.h>
#include "vk_device.h"

int main()
{
    Vulkan_Context* context = (Vulkan_Context*)malloc(sizeof(Vulkan_Context));
    create_vk_instance(context);
}

