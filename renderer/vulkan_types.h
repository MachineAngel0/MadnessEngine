#ifndef VULKAN_TYPES_H
#define VULKAN_TYPES_H

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

typedef struct Texture
{
    VkImage texture_image;
    VkDeviceMemory texture_image_memory;
    VkImageView texture_image_view;
    VkSampler texture_sampler;
    //optional, not needed rn
    u32 width;
    u32 height;
} Texture;


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
    VkCommandBuffer command_buffer_handle;
} vulkan_command_buffer;

typedef struct vulkan_shader_stage
{
    VkShaderModuleCreateInfo create_info;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo shader_stage_create_info;
} vulkan_shader_stage;


typedef enum pipeline_type
{
    PIPELINE_GRAPHICS,
    PIPELINE_COMPUTE,
} pipeline_type;


typedef struct vertex_info
{
    //TODO: they should be darrays or arenas, but for now its fine
    vertex_3d vertices[1000];
    u64 vertices_size;
    uint16_t indices[1000];
    u64 indices_size;
} vertex_info;

typedef struct vertex_buffer
{
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;

    VkBuffer vertex_staging_buffer;
    VkDeviceMemory vertex_staging_buffer_memory;

    VkBuffer index_buffer;
    VkDeviceMemory index_buffer_memory;

    VkBuffer index_staging_buffer;
    VkDeviceMemory index_staging_buffer_memory;

    void* data_vertex;
    VkDeviceSize vertex_buffer_capacity;
    void* data_index;
    VkDeviceSize index_buffer_capacity;
} vertex_buffer;


typedef struct vulkan_uniform_buffer
{
    //all arrays
    VkBuffer* uniformBuffers;
    VkDeviceMemory* uniformBuffersMemory;
    void** uniformBuffersMapped;
} vulkan_uniform_buffer;


typedef struct vulkan_shader_pipeline
{
    pipeline_type type; // NOTE: not in use rn
    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline_handle;
} vulkan_shader_pipeline;

typedef struct vulkan_shader_default
{
    vulkan_shader_pipeline default_shader_pipeline;
    vulkan_uniform_buffer global_uniform_buffers;

    //TODO: temporary for now
    VkDescriptorSetLayout default_shader_descriptor_set_layout;
    VkDescriptorPool descriptor_pool;

    VkDescriptorSetLayout per_frame_set_layouts[2];
    VkDescriptorSet descriptor_sets[2];
} vulkan_shader_default;


typedef struct vulkan_buffer
{
    u64 total_size;
    VkBuffer handle;
    VkBufferUsageFlagBits usage;
    VkDeviceMemory memory;
} vulkan_buffer;


typedef struct vulkan_context
{
    bool is_init;

    //Instance
    VkInstance instance;

    //Validation Layer
    VkAllocationCallbacks* allocator;
    VkDebugUtilsMessengerEXT debug_messenger;

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

    //TODO: vertex buffers and vertex data, here for now
    vertex_buffer default_vertex_buffer;
    vertex_info default_vertex_info;

    vulkan_shader_default default_shader_info;


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
