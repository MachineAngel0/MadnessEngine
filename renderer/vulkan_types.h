#ifndef VULKAN_TYPES_H
#define VULKAN_TYPES_H




#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include <vulkan/vulkan.h>
#include <defines.h>
#include "logger.h"
#include "math_types.h"
#include "vk_vertex.h"

#define VK_CHECK(expr)              \
{                                   \
    if(expr != VK_SUCCESS){         \
        FATAL("VULKAN ERROR CODE: %d", expr);\
    }\
}


typedef struct vulkan_image
{
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    // VkSampler texture_sampler; // NOTE: should probably be here
    u32 width;
    u32 height;
} vulkan_image;




typedef enum vulkan_render_pass_state
{
    READY,
    RECORDING,
    IN_RENDER_PASS,
    RECORDING_ENDED,
    SUBMITTED,
    NOT_ALLOCATED
} vulkan_render_pass_state;

typedef struct vulkan_renderpass
{
    VkRenderPass handle;
    vec4 screen_pos;
    vec4 clear_color;

    f32 depth;
    u32 stencil;

    vulkan_render_pass_state state;
} vulkan_renderpass;

typedef struct vulkan_framebuffer
{
    VkFramebuffer framebuffer_handle;
    u32 attachment_count;
    VkImageView* attachments;
    vulkan_renderpass* renderpass;
} vulkan_framebuffer;

typedef struct vulkan_swapchain
{
    //also contains VKformat
    VkSurfaceFormatKHR surface_format;
    u8 max_frames_in_flight;
    VkSwapchainKHR swapchain_handle;

    u32 image_count;
    VkImage* images;
    VkImageView* image_views;

    vulkan_image depth_attachment;

    // framebuffers used for on-screen rendering.
    vulkan_framebuffer* framebuffers;
} vulkan_swapchain;


typedef struct vulkan_swapchain_support_info
{
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
} vulkan_swapchain_capabilities_info;

typedef struct vulkan_device
{
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    vulkan_swapchain_capabilities_info swapchain_capabilities;

    i32 graphics_queue_index;
    i32 present_queue_index;
    i32 transfer_queue_index;
    i32 compute_queue_index;

    //family queues
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkQueue compute_queue;


    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;


    VkFormat depth_format;
} vulkan_device;

typedef struct vulkan_physical_device_requirements
{
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;

    // darray
    const char** device_extension_names;

    b8 sampler_anisotropy;
    b8 discrete_gpu;
} vulkan_physical_device_requirements;

typedef struct vulkan_physical_device_queue_family_info
{
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
} vulkan_physical_device_queue_family_info;

typedef struct vulkan_command_buffer
{
    // VkCommandPool command_pool; // TODO:
    VkCommandBuffer handle;
} vulkan_command_buffer;

typedef struct vulkan_shader_stage
{
    VkShaderModuleCreateInfo create_info;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo shader_stage_create_info;
} vulkan_shader_stage;



typedef struct vulkan_buffer
{
    // u64 total_size;
    // VkBufferUsageFlagBits usage;
    VkDeviceMemory memory;
    VkBuffer handle;
} vulkan_buffer;

typedef struct vulkan_uniform_buffer
{
    //all arrays
    VkBuffer* uniform_buffers;
    VkDeviceMemory* uniform_buffers_memory;
    void** uniform_buffers_mapped;
} vulkan_uniform_buffer;


typedef struct vulkan_shader_pipeline
{
    VkPipelineLayout pipeline_layout;
    VkPipeline handle;
} vulkan_shader_pipeline;


typedef struct Texture
{
    VkImage texture_image;
    VkDeviceMemory texture_image_memory;
    VkImageView texture_image_view;
    VkSampler texture_sampler;
} Texture;

typedef struct Material
{
    Texture* texture;
    vulkan_shader_pipeline* pipeline;
}Material;

typedef struct Shader_System
{
    Texture* error_texture;
}Shader_System;


typedef struct vulkan_shader_default
{
    vulkan_shader_pipeline default_shader_pipeline;

    //TODO: temporary for now
    VkDescriptorSetLayout descriptor_set_layout;

    VkDescriptorSet* descriptor_sets;
    u32 descriptor_set_count;
} vulkan_shader_default;


typedef struct vertex_info
{
    //TODO: they should be darrays or arenas, but for now its fine
    vertex vertices[1000];
    u64 vertices_size;
    uint16_t indices[1000];
    u64 indices_size;
} vertex_info;

typedef struct vulkan_mesh_default
{
    vulkan_shader_pipeline mesh_shader_pipeline;

    //TODO: temporary for now
    VkDescriptorSetLayout descriptor_set_layout;

    VkDescriptorSet* descriptor_sets;
    u32 descriptor_set_count;

    vulkan_buffer vertex_buffer;
    vulkan_buffer mesh_index_buffer;
    vertex_info vertex_info;


} vulkan_mesh_default;



typedef struct vulkan_shader_texture
{
    Texture texture_test_object;

    vulkan_shader_pipeline shader_texture_pipeline;

    //TODO: temporary for now
    VkDescriptorSetLayout descriptor_set_layout;

    VkDescriptorSet* descriptor_sets; // darray
    u32 descriptor_set_count;

    vulkan_buffer vertex_buffer;
    vulkan_buffer index_buffer;
    vertex_info vertex_info;

} vulkan_shader_texture;


typedef struct descriptor_pool_allocator
{
    VkDescriptorPool descriptor_pool;
} descriptor_pool_allocator;



typedef struct vulkan_context
{
    bool is_init;



    //Instance
    VkInstance instance;

    //Validation Layer
    VkAllocationCallbacks* allocator;
    VkDebugUtilsMessengerEXT debug_messenger;

    //Memory
    //TODO: not instantiated or used
    Arena renderer_arena;
    Frame_Arena frame_allocator;


    //Surface
    VkSurfaceKHR surface;
    // The framebuffer's current width and height.
    u32 framebuffer_width;
    u32 framebuffer_height;
    //value holders for our framebuffer values
    u32 framebuffer_width_new;
    u32 framebuffer_height_new;

    //Device
    vulkan_device device;

    //Swapchain
    vulkan_swapchain swapchain;
    bool recreating_swapchain;

    //renderpass
    vulkan_renderpass main_renderpass;

    //command buffers
    VkCommandPool graphics_command_pool;
    vulkan_command_buffer* graphics_command_buffer; // darray

    //global_descriptor_pool
    descriptor_pool_allocator global_descriptor_pool;

    //model,view, projection matrix
    vulkan_uniform_buffer global_uniform_buffers;


    //TODO: vertex buffers and vertex data, here for now
    vulkan_buffer vertex_buffer;
    vulkan_buffer index_buffer;
    vertex_info default_vertex_info;
    vulkan_shader_default default_shader_info;


    //textured triangle
    vulkan_shader_texture shader_texture;

    //temp
    vulkan_mesh_default mesh_default;


    //Semaphores and Fences
    // VkSemaphore* image_available_semaphores; // darray
    // VkSemaphore* queue_complete_semaphores; // darray
    u32 current_frame;

    VkFence* queue_submit_fence;
    VkCommandPool* primary_command_pool;
    VkCommandBuffer* primary_command_buffer;
    VkSemaphore* swapchain_acquire_semaphore; // semaphore that tells us when our next image is ready for usage/writing to
    VkSemaphore* swapchain_release_semaphore; // semaphore that signals when we are allowed to sumbit our new buffers
} vulkan_context;


#endif //VULKAN_TYPES_H
