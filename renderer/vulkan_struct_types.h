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
#include "../resource/resource_types.h"

//RESOURCE COUNTS
#define MAX_VULKAN_COMMAND_BUFFERS 32


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

    u32 width;
    u32 height;
    VkDeviceSize image_size;

    //idk if i would want this
    // enum vk_image_type{ VK_IMAGE_TYPE_TEXTURE, VK_IMAGE_TYPE_ATTACHMENT};
    // vk_image_type image_type;
} Vulkan_Texture;



typedef struct Vulkan_Swapchain
{
    //also contains VKformat
    VkSurfaceFormatKHR surface_format;
    VkSwapchainKHR swapchain_handle;

    u32 image_count;
    VkImage* images;
    VkImageView* image_views;

    Vulkan_Texture depth_attachment;


} Vulkan_Swapchain;


typedef struct vulkan_swapchain_support_info
{
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats; //darray
    u32 present_mode_count;
    VkPresentModeKHR* present_modes; //darray
} Vulkan_Swapchain_Capabilities_Info;


typedef struct Vulkan_Physical_Device_Suitable
{
    VkPhysicalDevice physical_device;
    u32 physical_device_index;
} Vulkan_Physical_Device_Suitable;


typedef struct Vulkan_Physical_Device_Heuristic
{
    bool dedicated_transfer;
    s32 transfer_queue_index;

    bool dedicated_compute;
    s32 compute_queue_index;

    bool dedicated_transfer_compute_sharing;

    s32 graphics_queue;
    s32 present_queue;

    s32 score;
} Vulkan_Physical_Device_Heuristic;


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
    bool integrated_gpu;
} Vulkan_Physical_Device_Requirements;

typedef struct vulkan_physical_device_queue_family_info
{
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
} vulkan_physical_device_queue_family_info;


typedef struct Vulkan_Command_Buffer
{
    // VkCommandPool command_pool;
    u32 id;
    VkCommandBuffer handle;
    Vulkan_Queue_Type queue_type;
    Vulkan_Command_Buffer_State state;


    //one per thread for nice work distribution, same threading access rules, dont share resources
    // VkCommandBuffer* secondary_buffers;
    // u32 secondary_count;
} Vulkan_Command_Buffer;

typedef struct Vulkan_Texture_Pending_Upload
{
    Madness_Texture* madness_texture;
    Vulkan_Texture* texture;
    Vulkan_Command_Buffer* command_buffer;
    u64 timeline_semaphore_value;
} Vulkan_Texture_Pending_Upload;

typedef struct Vulkan_Shader_Stage
{
    VkShaderModuleCreateInfo create_info;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo shader_stage_create_info;
} Vulkan_Shader_Stage;


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
    u8* mapped_data; // optional
} Vulkan_Buffer;


typedef struct Vulkan_Shader_Pipeline
{
    VkPipelineLayout pipeline_layout;
    VkPipeline handle;
} Vulkan_Shader_Pipeline;


typedef struct Mesh_Render_Item
{
    u64 material_key;
    u32 mesh_id;
    u32 mesh_handle;
    u32 submesh_handle;
    u32 material_handle;
    u32 transform_handle;
    // u32 cull_bounds_handle;
    u32 index_count;
    u32 index_offset;
    u32 vertex_offset;
} Mesh_Render_Item;

typedef struct Vulkan_Mesh_Draw
{
    u32 transform_idx;
    u32 material_instance_handle;
} Vulkan_Mesh_Draw;

typedef struct Skinned_Render_Item
{
    u64 material_key;
    u32 mesh_handle;
    u32 submesh_handle;
    u32 material_handle;
    u32 transform_handle;
    // u32 cull_bounds_handle;
    u32 index_count;
    u32 index_offset;
    u32 vertex_offset;

    u32 joint_idx;
    u32 weight_idx;
    u32 skinned_matrix_idx;
} Skinned_Render_Item;


typedef struct Vulkan_SKMesh_Draw
{
    u32 transform_idx;
    u32 material_instance_handle;
    u32 joint_idx;
    u32 weight_idx;
    u32 skinned_matrix_idx;
    u32 vertex_offset_count;
} Vulkan_Skinned_Draw;


typedef struct Mesh_Render_Record
{
    //passed in by the cpu mesh system
    //size of data
    u64 tangent_bytes;
    u64 vertex_color_bytes;
    u64 vertex_bytes;
    u64 normal_bytes;
    u64 uv_bytes;
    u64 indices_bytes;


    //data managed by the mesh system, to know where it is in the buffer and likely to change due to defragging
    u64 tangent_bytes_offset;
    u64 vertex_color_bytes_offset;
    u64 vertex_bytes_offset;
    u64 normal_bytes_offset;
    u64 uv_bytes_offset;
    u64 indices_bytes_offset;


    //indirect draw data
    u32 index_count; // how large is the data
    u32 vertex_count_offset; //in vec3
    u32 index_offset_count; //offset into the index buffer
    Index_Type index_type;

    bool is_uploaded;
    bool is_in_use;
} Mesh_Render_Record;


typedef struct Mesh_Gpu_Upload_Pending
{
    u32 mesh_id;
    u64 timeline_semaphore_value;
} Mesh_Gpu_Upload_Pending;

typedef struct Skinned_Render_Record
{
    //passed in by the cpu mesh system
    //size of data
    u64 tangent_bytes;
    u64 vertex_color_bytes;
    u64 vertex_bytes;
    u64 normal_bytes;
    u64 uv_bytes;
    u64 indices_bytes;

    size_t joints_byte_size;
    size_t weight_byte_size;

    //everything below is managed by system
    u64 tangent_bytes_offset;
    u64 vertex_color_bytes_offset;
    u64 vertex_bytes_offset;
    u64 normal_bytes_offset;
    u64 uv_bytes_offset;
    u64 indices_bytes_offset;

    size_t joints_byte_offset;
    size_t weight_byte_offset;


    //look up values into the skinned buffer, might not want to keep it here, we will see
    u64 joint_idx;
    u64 weight_idx;

    //indirect draw data
    u32 vertex_count_offset; //in vec3
    u32 index_count; // how large is the data
    u32 index_offset_count; //offset into the index buffer
    Index_Type index_type;

    bool is_uploaded;
    bool is_in_use;
} Skinned_Render_Record;

typedef struct Vulkan_Shader_Batch
{
    const char* shader_name;
    // const char* material_name;

    Shader_Mesh_Type mesh_type;
    Shader_Transluency_Type transluency;
    Shader_Blend_Mode blend_mode;
    Shader_Renderpass_Type renderpass_types;


    Material_ID material_id;


    Vulkan_Shader_Pipeline pipeline;
    Vulkan_Shader_Pipeline wireframe_pipeline;

    Buffer_Handle indirect_draw_buffer_handle;
    u32 draw_count;

    Buffer_Handle draw_data_buffer_handle;
    Buffer_Handle material_data_buffer_handle;
    u32 material_stride;

    PC_General pc_data;

    Material_Batch* material_batch_reference;

    // u32 pc_size;
    // void* pc_data;
} Vulkan_Shader_Batch;


#define AVAILABLE_TEXTURES 100
#define RENDERPASS_TEXTURES 20

typedef struct Shader_System
{
    Vulkan_Shader_Pipeline pipeline_references[AVAILABLE_TEXTURES];
    u32 pipeline_indexes;

    //idk if i need this
    Pipeline_Handle default_pipeline_handle;


    //TODO: I would look into this if sorting my draw calls starts to become an issue
    // https://realtimecollisiondetection.net/blog/?p=86

    //TODO: temp value for now, should probably be a dynamic array
    Vulkan_Shader_Batch mesh_batch[100];
    u32 mesh_batch_count;


    Vulkan_Shader_Batch skinned_batch[100];
    u32 skinned_batch_count;


    //the shader name is the lookup
    //we want the pointer to the shader batch,
    HASH_SET(Material_ID)* shader_batch_hash_set;
} Shader_System;


typedef struct vulkan_shader_default
{
    Vulkan_Shader_Pipeline default_shader_pipeline;

    //TODO: temporary for now
    VkDescriptorSetLayout descriptor_set_layout;

    VkDescriptorSet* descriptor_sets;
    u32 descriptor_set_count;
} vulkan_shader_default;

typedef struct vertex_3d
{
    vec3s position;
    vec3s color;
    vec2s padding;
} vertex_3d;

typedef struct vertex_tex
{
    // alignas(16) vec3 position;
    // alignas(16) vec3 color;
    // alignas(16) vec2 texture;
    vec3s position;
    vec3s color;
    vec2s texture;
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


typedef struct Directional_Light
{
    vec3s direction;
    vec3s color;

    //strength values
    float diffuse;
    float specular;
} Directional_Light;

typedef struct Point_Light
{
    //point lights go out in all directions
    vec4s position;
    vec4s color;

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
    vec4s position;
    vec4s color;
    vec3s direction;

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
    vec3s position;
} Area_Light;

typedef struct Light_System
{
    //each type is an array of them
    Directional_Light* directional_lights;
    u32 directional_light_count;

    Point_Light* point_lights;
    u32 point_light_count;

    Spot_Light* spot_lights;
    u32 spot_light_count;


    Buffer_Handle directional_light_storage_buffer_handle;
    Buffer_Handle directional_light_staging_buffer_handle;

    Buffer_Handle point_light_storage_buffer_handle;
    Buffer_Handle point_light_staging_buffer_handle;

    Buffer_Handle spot_light_storage_buffer_handle;
    Buffer_Handle spot_light_staging_buffer_handle;
} Light_System;


typedef struct global_ubo
{
    //this can basically be as largae as a i want, the max limit is 64 kib
    mat4s view;
    mat4s proj;


    VkDeviceAddress directional_lights_address;
    // which one do we want to use for this particular instance (might be better as a push constant)
    // u32 directional_light_index;
    VkDeviceAddress point_lights_address;
    u32 point_lights_count;

    VkDeviceAddress spot_lights_address;
    u32 spot_lights_count;

    //camera
    vec4s camera_position;
    vec2s screen_dimensions;

    f32 padding;


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


typedef struct Vulkan_Texture_System
{
    Vulkan_Texture textures[MAX_TEXTURE_COUNT];
    // count up for now, releasing is another issue
    u32 available_texture_indexes;

    // Vulkan_Texture renderpass_textures[100];
    // u32 renderpass_texture_indexes;


    hash_table* texture_file_to_handle;
    // hash_table* texture_file_to_usage_count; or // hash_table* handle_to_usage_count

    //since textures could be read this frame, we wait a frame to delete them
    RING_QUEUE_TYPE(Vulkan_Texture)* texture_deletion_queue;


    VkSemaphore timeline_texture_upload_semaphore;
    //textures who's uploads we are waiting on
    ARRAY_TYPE(Vulkan_Texture_Pending_Upload)* texture_pending_array;
    //u64 semaphore_singal_value = ++timeline_value; //use it like so
    u64 timeline_semaphore_texture_value;
} Vulkan_Texture_System;

typedef struct Buffer_System
{
    u32 frames_in_flight;

    //NOTE: buffers are handles to the first instance of them, multiple are created for each frame in flight
    // but when we get them from the system, they are offset by the current frame

    Buffer_Handle global_ubo_handle;

    //per frame buffer
    Vulkan_Buffer* per_frame_cpu_to_gpu_staging_buffers; // this should be for data uploads like vertex, skinned etc
    VkFence* staging_buffer_fence; // one per frame
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


    //insanity ui
    UI_Render_Packet* insanity_ui_render_packet;
    Buffer_Handle insanity_ui_material_ssbo_handle;
    u64 insanity_ui_draw_count;
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


typedef struct Vulkan_Mesh_System
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
    Buffer_Handle skinned_matrix_staging_buffer_handle;


    Buffer_Handle transform_buffer_handle;
    Buffer_Handle transform_staging_buffer_handle;

    //TODO: TEMP CODE, need a free list for this
    u32 vertex_offset_count;
    u32 index_offset_count;

    Mesh_Render_Record mesh_render_record[MAX_MESH_COUNT];
    u32 mesh_render_count;
    // Skinned_Render_Record skinned_render_record[MAX_SKINNED_MESH_COUNT];

    ARRAY_TYPE(Mesh_Gpu_Upload_Pending)* mesh_pending_array;
    VkSemaphore mesh_upload_timeline_semaphore;
    u64 upload_semaphore_value;
} Vulkan_Mesh_System;

typedef struct Particle_Render
{
    Buffer_Handle spherical_billboard_material_buffer_handle;
    u32 draw_count;

    Vulkan_Shader_Pipeline spherical_billboard_pipeline;
    Vulkan_Shader_Pipeline wireframe_spherical_billboard_pipeline;
} Particle_Render;

typedef struct Vulkan_Render_Thread
{
    //contains info for one thread

    //SPEC: one command pool per queue per thread
    //command buffers are only usable on the thread the pool spawned them from
    VkCommandPool pool;
    Vulkan_Command_Buffer command_buffer[MAX_VULKAN_COMMAND_BUFFERS];
} Vulkan_Render_Thread;


typedef struct Vulkan_Queue_Ownership_Operation
{
    Vulkan_Queue_Type from;
    Vulkan_Queue_Type to;

    VkDependencyInfo from_dependency_info;
    VkDependencyInfo to_dependency_info;

    VkSemaphoreSubmitInfo from_signal;
    VkSemaphoreSubmitInfo to_wait;
} Vulkan_Queue_Ownership_Operation;


typedef struct Vulkan_Transfer_Queue
{
    VkCommandPool pool;

    //these are aync resources flushed per frame
    //one command buffer is given out per frame, if any work, of any kind, is needed to be done
    Vulkan_Command_Buffer command_buffer[MAX_VULKAN_COMMAND_BUFFERS];
    u32 command_buffer_count;

    VkSemaphoreSubmitInfo signal_semaphore_info[100];
    uint32_t signal_semaphore_info_count;

    VkSemaphoreSubmitInfo wait_semaphore_info[100];
    uint32_t wait_semaphore_info_count;

    /* TODO:
    //ones we want to send to other systems
    VkSemaphoreSubmitInfo* wait_semaphore_to_graphics_transfer;
    uint32_t waitSemaphoreInfoCount;

    VkSemaphoreSubmitInfo* wait_semaphore_to_compute_transfer;
    uint32_t waitSemaphoreInfoCount_compute;
    */
} Vulkan_Transfer_Queue;

typedef struct Vulkan_Compute_Queue
{
    VkCommandPool pool;

    //per frame
    Vulkan_Command_Buffer* command_buffer;
    u8 command_buffer_count;

    VkFence* compute_frame_fence;
    u8 compute_frame_fence_count;

    VkSemaphoreSubmitInfo signal_semaphore[100];
    u32 signal_semaphore_count;
    VkSemaphoreSubmitInfo wait_semaphore[100];
    u32 wait_semaphore_count;


    /* NOTE: not gonna support async rn because i dont need it
    //aync commands
    Vulkan_Command_Buffer async_command_buffer[MAX_VULKAN_COMMAND_BUFFERS];

    const VkSemaphoreSubmitInfo* pSignalSemaphoreInfos;
    uint32_t signalSemaphoreInfoCount;

    //transfer operations
    const VkSemaphoreSubmitInfo* compute_frame_signal_semaphore;
    uint32_t wait_graphics_transfer;
    const VkSemaphoreSubmitInfo* compute_frame_wait_semaphore;
    uint32_t wait_transfer_transfer;*/
} Vulkan_Compute_Queue;


typedef struct Vulkan_Graphics_Queue
{
    //SPEC: Only a single thread can be submitting to a given queue at any time.
    //so either a dedicated thread or a sync point for flushing the built up commands

    //SPEC: one command pool per queue family per thread

    Vulkan_Queue_Type type;
    VkCommandPool pool;
    VkFence* frame_submit_fence;
    u32 frame_submit_fence_count;

    //one per frame in flight
    Vulkan_Command_Buffer* graphics_command_buffer;
    u32 graphics_command_buffer_count;

    //one per swapchain image
    u32 swapchain_semaphore_count;
    VkSemaphore* swapchain_wait_semaphore;
    VkSemaphore* swapchain_signal_semaphore;


    //example: if the transfer and graphics are the same then, we simple dont do the transfer queue operations
    //might not be a bad idea to have something like this, helps synchronization
    // enum upload_intent{};

    //TODO: we can definelty just use a linked list with a pool allocator
    VkSemaphoreSubmitInfo wait_semaphore_info[100];
    u32 wait_semaphore_info_count;
    VkSemaphoreSubmitInfo signal_semaphore_info[100];
    u32 signal_semaphore_info_count;

    //above creates the submit info,
    //but it makes sense that the command buffers hold onto the info,
    //and queue just gather them up
    // VkSubmitInfo2 submit_info;
} Vulkan_Graphics_Queue;


typedef struct Vulkan_Queue_System
{
    // really good resource on command buffers and multithreading
    // https://docs.vulkan.org/samples/latest/samples/performance/command_buffer_usage/README.html


    //SPEC:
    // To record command buffers concurrently, the framework needs to manage resource pools per frame and per thread.
    // According to the Vulkan Spec:
    // A command pool must not be used concurrently in multiple threads.
    // The application must not allocate and/or free descriptor sets from the same pool in multiple threads simultaneously.


    // SPEC:
    // Resetting a command pool recycles all of the resources from all of the command buffers allocated from the command pool back to the command pool. All command buffers that have been allocated from the command pool are put in the initial state.
    // Any primary command buffer allocated from another VkCommandPool that is in the recording or executable state and has a secondary command buffer allocated from commandPool recorded into it, becomes invalid.
    //NOTE: basically for our per frame data, we should reset the pool,
    // but that also means our secondary level command buffers need to be reallocated

    //NOTE: we have two distinct categories, per frame and sync
    //both are submitted at the end of the frame, but the buffer lifetimes are different
    //per frame buffers are guaranteed to be accessible that frame


    VkQueue graphics_queue;
    VkCommandPool graphics_pool;
    Vulkan_Graphics_Queue graphics_render_queue;


    VkQueue transfer_queue;
    VkCommandPool transfer_pool;
    Vulkan_Transfer_Queue transfer_render_queue;

    Vulkan_Compute_Queue comptute_render_queue;
    Vulkan_Command_Buffer compute_command_buffers[MAX_VULKAN_COMMAND_BUFFERS];
    u32 compute_command_buffer_count;
    VkQueue compute_queue;


    ring_queue* queue_ownership_change_transfer_to_graphics;
    ring_queue* queue_ownership_change_transfer_to_compute;
    ring_queue* queue_ownership_change_compute_to_graphics;
} Vulkan_Queue_System;


typedef struct Renderer
{
    bool is_init;


    Camera main_camera;
    Render_Mode mode;

    Allocator allocator; // total memory for the entire renderer
    Allocator frame_allocator;
    Heap_Allocator* heap_allocator;


    Input_System* input_system; //meant only to be used for debugging
    //general resources taken from the resource system
    Shader_System* shader_system;
    Sprite_Renderer* sprite_renderer;
    Vulkan_Mesh_System* mesh_system;
    Particle_Render* particle_render;

    //renderer systems
    Vulkan_Queue_System* queue_system;
    Buffer_System* buffer_system;
    Vulkan_Texture_System* texture_system;
    Light_System* light_system;
    Descriptor_System* descriptor_system;


    //draw systems
    UI_Renderer_Backend* ui_renderer;


    //pipelines
    vulkan_pipeline_cache* pipeline_cache;

    Vulkan_Shader_Pipeline ui_pipeline;
    Vulkan_Shader_Pipeline text_pipeline;
    Vulkan_Shader_Pipeline sprite_pipeline;

    Vulkan_Shader_Pipeline predepth_mesh_pipeline;
    Vulkan_Shader_Pipeline predepth_skinned_mesh_pipeline;

    Vulkan_Shader_Pipeline shadow_mesh_pipeline;
    Vulkan_Shader_Pipeline shadow_skinned_mesh_pipeline;


    //lights
    //NOTE: each light needs a texture, for now ill limit it to a pool of texture's,
    // but i'll need a solution at some point
    //TODO: update the material pipeline, and set piepline states
    Vulkan_Texture shadowpass_texture;
    Vulkan_Shader_Pipeline directional_shadow_pipeline;
    Vulkan_Shader_Pipeline spot_light_shadow_pipeline;
    Vulkan_Shader_Pipeline point_light_shadow_pipeline; //this has to be a 3d cubemap

    bool wireframe_mode;


    ////// Instance and Device //////


    //Instance
    VkInstance instance;

    //Validation Layer
    VkAllocationCallbacks* vulkan_allocator;
    VkDebugUtilsMessengerEXT debug_messenger;


    //device
    VkPhysicalDevice physical_device;
    u32 physical_device_index;

    VkPhysicalDeviceFeatures2 features2;
    VkPhysicalDeviceProperties2 properties2;
    VkPhysicalDeviceMemoryProperties2 physical_device_memory;

    ARRAY_TYPE(VkQueueFamilyProperties)* queue_family_properties;
    // Vulkan_Physical_Device_Heuristic device_heuristic; // copy

    s32 transfer_queue_index;
    s32 compute_queue_index;
    s32 graphics_queue_index;
    s32 present_queue_index;

    VkDevice logical_device;

    VkQueue transfer_queue;
    VkQueue compute_queue;
    VkQueue graphics_queue;
    VkQueue present_queue;


    VkCommandPool graphics_command_pool;
    VkCommandPool transfer_command_pool;
    VkCommandPool compute_command_pool;

    // debug labels loaded functions
    PFN_vkCmdBeginDebugUtilsLabelEXT debug_label_start;
    PFN_vkCmdEndDebugUtilsLabelEXT debug_label_end;

    ////// Swapchain and Surface //////

    VkFormat depth_format;
#define VULKAN_MAX_FRAMES_IN_FLIGHT 2
    u8 max_frames_in_flight;
    u32 current_frame;

    //Surface
    VkSurfaceKHR surface;
    // The framebuffer's current width and height.
    u32 framebuffer_width;
    u32 framebuffer_height;
    //temp values holder for our framebuffer values, checked once a frame if changed
    u32 framebuffer_width_new;
    u32 framebuffer_height_new;

    Vulkan_Swapchain_Capabilities_Info swapchain_capabilities;
    Vulkan_Swapchain swapchain;
    bool recreating_swapchain;

} Renderer;


#endif //VULKAN_STRUCT_TYPES_H
