#ifndef VULKAN_TYPES_H
#define VULKAN_TYPES_H

#include "vk_vertex.h"


#define VK_CHECK(expr)              \
{                                   \
    if(expr != VK_SUCCESS){         \
        FATAL("VULKAN ERROR CODE: %d", expr);\
    }\
}


/// HANDLES ///
typedef struct shader_handle
{
    u32 handle;
} shader_handle;

typedef struct buffer_handle
{
    u32 handle;
} buffer_handle;



typedef struct vulkan_image
{
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    // VkSampler texture_sampler; // NOTE: should probably be here
    u32 width;
    u32 height;
} vulkan_image;


/// MESH ///
typedef struct vertex_mesh
{
    vec3* pos;
    vec3* normal;
    vec4* tangent;
    vec2* uv;

    // vec4* color; //might not support
} vertex_mesh;

typedef struct pc_mesh
{
    // VkDeviceAddress just a typedef of a u64
    VkDeviceAddress pos_address;
    // u64 normal_index;
    // u64 tangent_index;
    VkDeviceAddress uv_index;

    u32 albedo_material_index;
    uint32_t _padding;
    //TODO:
    // u32 material_albedo_index;

} pc_mesh;


//TODO: every model can have multiple meshes
typedef struct mesh
{
    vertex_mesh vertices;

    size_t* indices;
    u32 indices_count;
    u32 indices_bytes;
    VkIndexType index_type;

    u64 vertex_count;
    u64 vertex_bytes;

    u64 normal_count;
    u64 normal_bytes;

    u64 tangent_count;
    u64 tangent_bytes;

    u64 uv_count;
    u64 uv_bytes;

    shader_handle color_texture;
    //whether it has index's or not // TODO: can probably move out into its own type of mesh, struct mesh_indexless
} mesh;


typedef struct static_mesh
{
    mesh* mesh;
    // the number of meshes in the model
    u32 mesh_size;
    shader_handle* material_handles;
} static_mesh;



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
    // context->device.properties.limits. // gets device limits like max maxDescriptorSetSampledImages,
    // maxMemoryAllocationCount, maxPerStageDescriptorSampledImages

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

    //VkDeviceSize are typedefs for u64's
    u64 current_offset;
    u64 capacity;
} vulkan_buffer;

typedef struct vulkan_staging_buffer
{
    // u64 total_size;
    // VkBufferUsageFlagBits usage;
    VkDeviceMemory memory;
    VkBuffer handle;
    //VkDeviceSize are typedefs for u64's
    uint8_t* mapped_data;
    u64 capacity;
} vulkan_staging_buffer;

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
    u32 texture_indexes[10]; //TODO: could be more but fine for now
    u32 pipeline_indexes;
}Material;




typedef struct shader_system
{
    Texture error_texture;
    Texture textures[100];
    u32 available_texture_indexes; // count up for now, releasing is another issue
    // u32 texture_usage_count[100]; // how many things are referencing this texture, so when it hits 0, we can add it to the free list
    // u32 free_list_texture_indexes[100];

    Material material_references[100];
    u32 material_indexes;
    vulkan_shader_pipeline pipeline_references[100];

    shader_handle default_texture_handle;

}shader_system;

typedef enum buffer_type
{
    UNIFORM,
    STORAGE, // meant to be used in a descriptor set
    VERTEX, // meant to only be used with vkCmdBindVertexBuffers
    INDEX, // meant to only be used as part of a vkCmdBindIndexBuffer or  vkCmdBindIndexBuffer2
    // meant to used as part of a vkCmdDrawIndirect, vkCmdDrawIndexedIndirect, vkCmdDrawMeshTasksIndirectNV, vkCmdDrawMeshTasksIndirectCountNV, vkCmdDrawMeshTasksIndirectEXT, vkCmdDrawMeshTasksIndirectCountEXT,
    INDIRECT,

    //idk what these are used for yet, but they will probably be useful later
    //UNIFORM_TEXEL,
    //STORAGE_TEXEL,
} buffer_type;

typedef struct buffer_system
{
    //an array of them
    //NOTE: if we run out we can always allocate more, for now we just keep one of each
    vulkan_buffer* vertex_buffers;
    vulkan_buffer* uv_buffers;
    vulkan_buffer* index_buffers;
    vulkan_buffer* storage_buffers;
    vulkan_buffer* uniform_buffers;

    //how many have been allocated
    u32 vertex_buffer_count;
    u32 index_buffer_count;
    u32 storage_buffer_count;
    u32 uniform_buffer_count;

    //TODO: this should be an array
    //NOTE: when we multithread, it would make sense to have more than one of these in something like a ring buffer
    vulkan_staging_buffer* staging_buffer_ring;
    u32 staging_buffer_count;


    //TODO: queries for size
    /*
    u64 temp = vulkan_context->device.properties.limits.maxStorageBufferRange;
    u64 temp1 = vulkan_context->device.properties.limits.maxUniformBufferRange;
    u64 temp3 = vulkan_context->device.properties.limits.maxMemoryAllocationCount;
    */
} buffer_system;


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
    vertex_3d vertices[1000];
    u64 vertices_size;
    u16 indices[1000];
    u64 indices_size;
} vertex_info;

typedef struct vulkan_mesh_default
{
    static_mesh* static_mesh;

    vulkan_shader_pipeline mesh_shader_pipeline;
    vulkan_buffer vertex_buffer;
    vulkan_buffer index_buffer;


} vulkan_mesh_default;


typedef struct vulkan_bindless_descriptors
{

    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet* descriptor_sets; //darray, the number of max frames in use
    u32 descriptor_set_count;

} vulkan_bindless_descriptors;


typedef struct vulkan_shader_texture
{
    Texture texture_test_object;

    vulkan_shader_pipeline shader_texture_pipeline;

    vulkan_buffer vertex_buffer;
    vulkan_buffer index_buffer;
    vertex_info vertex_info;

} vulkan_shader_texture;




typedef struct descriptor_pool_allocator
{
    VkDescriptorPool descriptor_pool;
    VkDescriptorPool bindless_descriptor_pool;

} descriptor_pool_allocator;

typedef struct global_descriptor_sets
{
    vulkan_bindless_descriptors uniform_descriptors;
    vulkan_bindless_descriptors texture_descriptors;
} global_descriptor_sets;



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

    //global_descriptor_pool
    descriptor_pool_allocator global_descriptor_pool;

    //model,view, projection matrix
    vulkan_uniform_buffer global_uniform_buffers;


    //TODO: Clean this up
    vulkan_buffer vertex_buffer;
    vulkan_buffer index_buffer;
    vertex_info default_vertex_info;
    vulkan_shader_default default_shader_info;


    //textured triangle
    vulkan_shader_texture shader_texture;
    vulkan_shader_texture shader_texture_bindless;

    global_descriptor_sets global_descriptors;

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



/* CAMERA */
typedef enum Camera_Movement
{
    CAMERA_MOVEMENT_FORWARD,
    CAMERA_MOVEMENT_BACKWARD,
    CAMERA_MOVEMENT_LEFT,
    CAMERA_MOVEMENT_RIGHT
} Camera_Movement;


typedef struct camera
{
    vec3 rotation;
    vec3 pos;
    vec4 viewPos;

    float rotation_speed;
    float move_speed;

    //perspective options
    float fov;
    float znear;
    float zfar;

    mat4 projection;
    mat4 view;

    //fps
    float pitch;
    float yaw;
} camera;

struct camera_arrays
{
    camera lookat_cameras[10];
    camera fps_cameras[10];
    camera arcball_cameras[10];
};


typedef struct renderer
{
    camera main_camera;

    Arena arena; // total memory for the entire renderer
    Arena frame_arena;

    shader_system* shader_system;
    buffer_system* buffer_system;

    //mesh system
    //animation system
    //ui draw info

    //TODO:
    vulkan_context context;

}renderer;







#endif //VULKAN_TYPES_H
