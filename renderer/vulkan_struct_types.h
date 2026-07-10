#ifndef VULKAN_STRUCT_TYPES_H
#define VULKAN_STRUCT_TYPES_H


#define VK_CHECK(expr)              \
{                                   \
    if(expr != VK_SUCCESS){         \
        FATAL("VULKAN ERROR CODE: %d", expr);\
    }\
}


#include "vulkan_enum_types.h"

#include "camera.h"
#include "../core/dsa/darray.h"
#include "hash_table.h"
#include "input.h"
#include "str.h"
#include "../resource/resource_types.h"


/// HANDLES ///


typedef struct Shader_Handle
{
    u32 handle;
} Shader_Handle;

typedef struct Pipeline_Handle
{
    u32 handle;
} Pipeline_Handle;

typedef struct Buffer_Handle
{
    u32 handle;
    bool is_per_frame;
} Buffer_Handle;


typedef struct Staging_Buffer_Handle
{
    u32 handle;
} Staging_Buffer_Handle;

typedef struct Descriptor_Handle
{
    u32 handle;
} Descriptor_Handle;


typedef struct Vulkan_Texture
{
    VkImage texture_image;
    VkDeviceMemory texture_image_memory;
    VkImageView texture_image_view;
    VkSampler texture_sampler;
    // VkFormat texture_format; //TODO: should probably keep a copy of this

    // u32 width;
    // u32 height;

    //idk if i would want this
    // enum vk_image_type{ VK_IMAGE_TYPE_TEXTURE, VK_IMAGE_TYPE_ATTACHMENT};
    // vk_image_type image_type;
} Vulkan_Texture;


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

    Vulkan_Texture depth_attachment;

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

    s32 graphics_queue_index;
    s32 present_queue_index;
    s32 transfer_queue_index;
    s32 compute_queue_index;

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
    bool graphics;
    bool present;
    bool compute;
    bool transfer;

    // darray
    const char** device_extension_names;

    bool sampler_anisotropy;
    bool discrete_gpu;
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


typedef struct Vulkan_Buffer
{
    // u64 total_size;
    // VkBufferUsageFlagBits usage;
    VkDeviceMemory memory;
    VkBuffer handle;

    //VkDeviceSize are typedefs for u64's
    u64 current_offset;
    u64 capacity;
    Vulkan_Buffer_Type type;
    uint8_t* mapped_data;
} Vulkan_Buffer;


typedef struct vulkan_shader_pipeline
{
    VkPipelineLayout pipeline_layout;
    VkPipeline handle;
} vulkan_shader_pipeline;



typedef struct Vulkan_Shader_Batch
{
    const char* shader_name;
    Shader_Stage_Type shader_stage_type;
    Shader_Pass_Type shader_pass_type;
    Shader_Mesh_Type mesh_type;


    vulkan_shader_pipeline pipeline;

    Buffer_Handle indirect_draw_buffer_handle;
    u32 draw_count;

    Buffer_Handle draw_data_buffer_handle;
    Buffer_Handle material_data_buffer_handle;
    u32 material_stride;

    PC_General pc_data;

    // u32 pc_size;
    // void* pc_data;
}Vulkan_Shader_Batch;


#define AVAILABLE_TEXTURES 100

typedef struct Shader_System
{
    Vulkan_Texture textures[MAX_TEXTURE_COUNT];
    // count up for now, releasing is another issue
    u32 available_texture_indexes;

    vulkan_shader_pipeline pipeline_references[AVAILABLE_TEXTURES];
    u32 pipeline_indexes;

    //idk if i need this
    Pipeline_Handle default_pipeline_handle;

    hash_table* texture_file_to_handle;
    // hash_table* texture_file_to_usage_count; or // hash_table* handle_to_usage_count
    u32 max_indexes;

    //since textures could be read this frame, we wait a frame to delete them
    //TODO: ring_queue* texture_delete_queue

    //TODO: temp value for now, should probably be a dynamic array
    Vulkan_Shader_Batch shader_batches[100];
    u32 shader_batches_count;

    //the shader name is the lookup
    //we want the pointer to the shader batch,
    HASH_TABLE_TYPE(Vulkan_Shader_Batch*)* shader_batch_hash_table;


} Shader_System;


typedef struct Buffer_System
{
    u32 frames_in_flight;

    //NOTE: buffers are handles to the first instance of them, multiple are created for each frame in flight
    // but when we get them from the system, they are offset by the current frame

    Buffer_Handle global_ubo_handle;

    //per frame buffer
    Vulkan_Buffer* per_frame_cpu_to_gpu_staging_buffers; // this should be for data uploads like vertex, skinned etc

    // this should be for things that are neccessary to happen every frame, like the indirect, draw, and material buffers
    // Vulkan_Buffer* per_frame_staging_buffer_pool;

    //an array of them
    Vulkan_Buffer* buffers;
    u32 buffers_size; // total we have to be given out
    u32 buffer_current_count; // current amount given out
    //add a linked list in later for buffers we free

    //we can have a one to one mapping from buffers -> staging buffers
    //given how we might use this, it would be ok to have holes in the array
    //TODO: freelist instead of keeping a count, and we might want to differentiate these by size
    //ASSUMPTION: staging buffers are per frame, and if i ever exceed a frames staging upload limit, just increase the size
    Buffer_Handle staging_buffer_handle;

    //TODO: queries for size
    /*
    u64 temp = vulkan_context->device.properties.limits.maxStorageBufferRange;
    u64 temp1 = vulkan_context->device.properties.limits.maxUniformBufferRange;
    u64 temp3 = vulkan_context->device.properties.limits.maxMemoryAllocationCount;
    */
} Buffer_System;


typedef struct vulkan_shader_default
{
    vulkan_shader_pipeline default_shader_pipeline;

    //TODO: temporary for now
    VkDescriptorSetLayout descriptor_set_layout;

    VkDescriptorSet* descriptor_sets;
    u32 descriptor_set_count;
} vulkan_shader_default;

typedef struct vertex_3d
{
    vec3 position;
    vec3 color;
    vec2 padding;
} vertex_3d;

typedef struct vertex_tex
{
    // alignas(16) vec3 position;
    // alignas(16) vec3 color;
    // alignas(16) vec2 texture;
    vec3 position;
    vec3 color;
    vec2 texture;
} vertex_tex;


typedef struct vulkan_bindless_descriptors
{
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet* descriptor_sets; //darray, the number of max frames in use
    u32 descriptor_set_count;
    // Descriptor_Type descriptor_type; // uniform, textures, sbo
} vulkan_bindless_descriptors;


typedef struct global_descriptor_sets
{
    vulkan_bindless_descriptors uniform_descriptors;
    vulkan_bindless_descriptors texture_descriptors;
    vulkan_bindless_descriptors storage_descriptors;
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

    VkCommandPool transfer_command_pool;
    vulkan_command_buffer* transfer_command_buffer; // darray
    VkCommandPool compute_command_pool;
    vulkan_command_buffer* compute_command_buffer; // darray


    //Semaphores and Fences
    // VkSemaphore* image_available_semaphores; // darray
    // VkSemaphore* queue_complete_semaphores; // darray
    u32 current_frame;

    VkFence* queue_submit_fence;
    VkCommandPool* primary_command_pool;
    VkCommandBuffer* primary_command_buffer;
    VkSemaphore* swapchain_acquire_semaphore;
    // semaphore that tells us when our next image is ready for usage/writing to
    VkSemaphore* swapchain_release_semaphore; // semaphore that signals when we are allowed to sumbit our new buffers
} vulkan_context;


typedef struct Directional_Light
{
    vec3 direction;
    vec3 color;

    //strength values
    float diffuse;
    float specular;
} Directional_Light;

typedef struct Point_Light
{
    //point lights go out in all directions
    vec4 position;
    vec4 color;

    //light fall offs
    float constant;
    float linear;
    float quadratic;

    //strength values
    float diffuse;
    float specular;
    // float intensity;
    // float radius;
} Point_Light;

typedef struct Spot_Light
{
    //TODO:
    vec4 position;
    vec4 color;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    float diffuse;
    float specular;
} Spot_Light;

typedef struct Area_Light
{
    //TODO:
    vec3 position;
} Area_Light;

typedef struct Light_System
{
    //each type is an array of them
    Point_Light* point_lights;
    Directional_Light* directional_lights;
    u32 point_light_count;
    u32 directional_light_count;

    Buffer_Handle directional_light_storage_buffer_handle;
    Buffer_Handle point_light_storage_buffer_handle;

    Buffer_Handle directional_light_staging_buffer_handle;
    Buffer_Handle point_light_staging_buffer_handle;
} Light_System;


typedef struct global_ubo
{
    //this can basically be as largae as a i want, the max limit is 64 kib
    mat4 view;
    mat4 proj;


    VkDeviceAddress directional_lights_address;
    // which one do we want to use for this particular instance (might be better as a push constant)
    // u32 directional_light_index;
    VkDeviceAddress point_lights_address;
    u32 point_lights_count;

    //camera
    vec4 camera_position;
    vec2 screem_dimensions;

    f32 time;
    Render_Mode render_mode;


    //global list of buffer resources

    //meshes
    VkDeviceAddress vertex_buffer;
    VkDeviceAddress normal_buffer;
    VkDeviceAddress uv_buffer;
    VkDeviceAddress tangent_buffer;

    //sk meshes
    VkDeviceAddress joint_buffer;
    VkDeviceAddress weight_buffer;
    VkDeviceAddress skinned_matrix_buffer;

    //global transforms
    VkDeviceAddress transform_buffer;


} Global_Ubo;




typedef struct Descriptor_System
{
    VkDescriptorPool bindless_descriptor_pool;

    vulkan_bindless_descriptors uniform_descriptors;
    vulkan_bindless_descriptors texture_descriptors;
    vulkan_bindless_descriptors storage_descriptors;

    u32 uniform_count;
    u32 texture_count;
    u32 storage_count;
    u32 attachment_count;
} Descriptor_System;


typedef struct pipeline_cache_file_header
{
    //mpipe
    u32 magic; // an arbitrary magic header to make sure this is actually our file
    u32 data_size; // equal to *pDataSize returned by vkGetPipelineCacheData

    u32 vendor_id; // equal to VkPhysicalDeviceProperties::vendorID
    u32 device_id; // equal to VkPhysicalDeviceProperties::deviceID
    u32 driver_version; // equal to VkPhysicalDeviceProperties::driverVersion
    u32 driver_abi; // equal to sizeof(void*)

    u8 uuid[VK_UUID_SIZE]; // equal to VkPhysicalDeviceProperties::pipelineCacheUUID
} pipeline_cache_file_header;

typedef struct vulkan_pipeline_cache
{
    VkPipelineCache handle;
} vulkan_pipeline_cache;



typedef struct UI_Renderer
{
    Buffer_Handle ui_vertex_buffer_handle;
    Buffer_Handle ui_index_buffer_handle;
    Buffer_Handle ui_material_ssbo_handle;

    Buffer_Handle ui_vertex_staging_buffer_handle;
    Buffer_Handle ui_index_staging_buffer_handle;
    Buffer_Handle ui_material_staging_ssbo_handle;



    u64 draw_count;

    UI_Render_Packet* madness_ui_render_packet;
} UI_Renderer_Backend;


typedef struct Sprite_Backend
{
    u16 sprite_indices[6];

    VkIndexType index_type;

    Buffer_Handle sprite_vertex_buffer;
    Buffer_Handle sprite_index_buffer;
    Buffer_Handle sprite_indirect_buffer;
    Buffer_Handle sprite_instance_ssbo_buffer;

    Buffer_Handle sprite_vertex_staging_buffer;
    Buffer_Handle sprite_index_staging_buffer;
    Buffer_Handle sprite_instance_staging_buffer;
    Buffer_Handle sprite_indirect_staging_buffer;


    u64 draw_count;
} Sprite_Renderer;


typedef struct Mesh_Renderer
{
    Buffer_Handle vertex_buffer_handle;
    Buffer_Handle index_buffer_handle;
    Buffer_Handle normal_buffer_handle;
    Buffer_Handle uv_buffer_handle;
    Buffer_Handle tangent_buffer_handle;

    //using these to test the global staging buffer
    Buffer_Handle joint_buffer_handle;
    Buffer_Handle weight_buffer_handle;


    Buffer_Handle vertex_staging_buffer_handle;
    Buffer_Handle index_staging_buffer_handle;
    Buffer_Handle indirect_staging_buffer_handle;
    Buffer_Handle normal_staging_buffer_handle;
    Buffer_Handle tangent_staging_buffer_handle;
    Buffer_Handle uv_staging_buffer_handle;

    Buffer_Handle skinned_matrix_buffer;


    Buffer_Handle pbr_buffer_handle;
    Buffer_Handle pbr_staging_buffer_handle;

    Buffer_Handle transform_buffer_handle;
    Buffer_Handle transform_staging_buffer_handle;

} Mesh_Renderer;



typedef struct renderer
{
    camera main_camera;
    Render_Mode mode;

    Allocator allocator; // total memory for the entire renderer
    Allocator frame_allocator;
    Heap_Allocator* heap_allocator;


    Input_System* input_system; //meant only to be used for debugging

    Resource_System* resource_system; //reference

    //general resources taken from the resource system
    Shader_System* shader_system;
    Sprite_Renderer* sprite_renderer;
    Mesh_Renderer* mesh_renderer;

    //renderer specific
    Buffer_System* buffer_system;
    Light_System* light_system;
    Descriptor_System* descriptor_system;


    //draw systems
    UI_Renderer_Backend* ui_renderer;



    //mesh system
    //animation system
    //ui draw info

    //TODO: get it out of the context and drop it here
    vulkan_context context;

    //TODO: TEMP
    VkBufferMemoryBarrier2 buffer_memory_barrier_batch_release[100];
    u32 buffer_memory_barrier_batch_release_count;

    VkBufferMemoryBarrier2 buffer_memory_barrier_batch_acquire[100];
    u32 buffer_memory_barrier_batch_acquire_count;

    VkSemaphore* transfer_signal_sempahores;
    VkSemaphoreSubmitInfo transfer_sumbit_signal_semaphore;


    //pipelines
    vulkan_pipeline_cache* pipeline_cache;

    vulkan_shader_pipeline indirect_mesh_pipeline;
    vulkan_shader_pipeline skinned_mesh_pipeline;
    vulkan_shader_pipeline ui_pipeline;
    vulkan_shader_pipeline text_pipeline;
    vulkan_shader_pipeline sprite_pipeline;
} Renderer;









#endif //VULKAN_STRUCT_TYPES_H
