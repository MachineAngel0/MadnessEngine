//Vulkan
#include "SPIRV-Reflect-main/spirv_reflect.c"

//Renderer

#include "renderer.c"
#include "spv_reflect.c"

#include "camera.c"
#include "lights.c"
#include "shader_system.c"
#include "UI.c"
#include "vk_buffer.c"
#include "vk_command_buffer.c"
#include "vk_descriptors.c"
#include "vk_device.c"
#include "vk_framebuffer.c"
#include "vk_image.c"
#include "vk_pipeline.c"
#include "vk_renderpass.c"
#include "vk_shader.c"
#include "vk_swapchain.c"
#include "vk_sync.c"

#include "pipelines/sprite_render.c"
#include "pipelines/ui_render.c"
#include "pipelines/material_render.c"
#include "pipelines/transform_render.c"
#include "pipelines/mesh_render.c"
