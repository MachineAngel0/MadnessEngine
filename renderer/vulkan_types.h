#ifndef VULKAN_TYPES_H
#define VULKAN_TYPES_H


#define VK_CHECK(expr)              \
{                                   \
    if(expr != VK_SUCCESS){         \
        FATAL("VULKAN ERROR CODE: %d", expr);\
    }\
}



#include "camera.h"
#include "darray.h"
#include "hash_set.h"
#include "str.h"
#include "maths/math_types.h"
#include "maths/transforms.h"


/// HANDLES ///

typedef struct Texture_Handle
{
    u32 handle;
} Texture_Handle;

typedef struct Material_Handle
{
    u32 handle;
} Material_Handle;

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
} Buffer_Handle;

typedef struct Descriptor_Handle
{
    u32 handle;
} Descriptor_Handle;

typedef struct Mesh_Handle
{
    u32 handle;
} Mesh_Handle;


typedef struct Texture
{
    VkImage texture_image;
    VkDeviceMemory texture_image_memory;
    VkImageView texture_image_view;
    VkSampler texture_sampler;
    // u32 width;
    // u32 height;

    //idk if i would want this
    // enum vk_image_type{ VK_IMAGE_TYPE_TEXTURE, VK_IMAGE_TYPE_ATTACHMENT};
    // vk_image_type image_type;
} Texture;

typedef struct Material_Param_Data
{
    u32 feature_mask;

    vec4 color; // this will be at a default of 1.0, which is white but won't affect the material
    //ALL FROM RANGES 0-1
    float ambient_strength; // optional for now we can remove it later
    float roughness_strength;
    float metallic_strength;
    float normal_strength;
    float ambient_occlusion_strength;
    float emissive_strength;

    u32 color_index;
    u32 normal_index;
    u32 metallic_index;
    u32 roughness_index;
    u32 ambient_occlusion_index;
    u32 emissive_index;
    u32 _padding0;
    u32 _padding1;
    u32 _padding2;
} Material_Param_Data;


/// MESH ///
typedef struct vertex_mesh
{
    vec3* pos;
    vec3* normal;
    vec4* tangent;
    vec2* uv;

    // vec4* color; //might not support
} vertex_mesh;

typedef struct PC_Mesh
{
    //per instance data change
    u32 ubo_buffer_idx;
    u32 normal_buffer_idx;
    u32 tangent_buffer_idx;
    u32 uv_buffer_idx;
    u32 transform_buffer_idx;
    u32 material_buffer_idx;
    u32 _padding;
    u32 _padding1;

} PC_Mesh;




typedef struct submesh
{
    Transform transform;

    // vertex_mesh vertices;
    vec3* pos;
    vec3* normal;
    vec4* tangent;
    vec2* uv;

    // size_t* indices;
    u8* indices;
    // u8* indices_2;
    u32 indices_count;
    u32 indices_bytes;
    VkIndexType index_type;

    u64 vertex_bytes;
    u64 normal_bytes;
    u64 tangent_bytes;
    u64 uv_bytes;

    Material_Param_Data material_params;
    Texture_Handle color_texture;
    Texture_Handle normal_texture;
    Texture_Handle metallic_texture;
    Texture_Handle roughness_texture;
    Texture_Handle ambient_occlusion_texture;
    Texture_Handle emissive_texture;

    u32 offset_into_material_data_buffer;


    u32 mesh_pipeline_mask; // determines what material features this submesh has
} submesh;


typedef struct static_mesh
{
    submesh* mesh;
    // the number of meshes in the model
    u32 mesh_size;

    //

} static_mesh;

// TODO: skinned mesh
typedef struct skinned_mesh
{
    submesh* mesh;
    // the number of meshes in the model
    u32 mesh_size;
    VkDrawIndexedIndirectCommand* indirect_draw_array; // has the same size as mesh size

    //Joints joints;
    //Weights weights;
} skinned_mesh;

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

    Texture depth_attachment;

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

typedef enum vulkan_cpu_buffer_type
{
    BUFFER_TYPE_INVALID,

    BUFFER_TYPE_VERTEX, // meant to only be used with vkCmdBindVertexBuffers
    BUFFER_TYPE_INDEX, // meant to only be used as part of a vkCmdBindIndexBuffer or  vkCmdBindIndexBuffer2

    //create this if the storage is bieng used large data
    BUFFER_TYPE_CPU_STORAGE, // meant to be used in a descriptor set
    // meant to used as part of a vkCmdDrawIndirect, vkCmdDrawIndexedIndirect, vkCmdDrawMeshTasksIndirectNV, vkCmdDrawMeshTasksIndirectCountNV, vkCmdDrawMeshTasksIndirectEXT, vkCmdDrawMeshTasksIndirectCountEXT,
    BUFFER_TYPE_INDIRECT,

    //idk what these are used for yet, but they will probably be useful later
    //UNIFORM_TEXEL,
    //STORAGE_TEXEL,

    //NOTE: TYPES BELOW ARE BUFFERS MEANT TO BE USED UPDATABLE WITHOUT A STAING BUFFER

    //create this if the storage is used for compute or if the data is relatively small and updates every frame, or static/rarely gets updated
    BUFFER_TYPE_GPU_STORAGE,
    BUFFER_TYPE_STAGING,
    BUFFER_TYPE_UNIFORM,
} vulkan_buffer_type;


typedef struct vulkan_buffer
{
    // u64 total_size;
    // VkBufferUsageFlagBits usage;
    VkDeviceMemory memory;
    VkBuffer handle;

    //VkDeviceSize are typedefs for u64's
    u64 current_offset;
    u64 capacity;
    vulkan_buffer_type type;
    uint8_t* mapped_data;
} vulkan_buffer;


typedef enum vulkan_shader_pipeline_type
{
    //EXAMPLE OF HOW I MIGHT DYNAMICALLY CONSTRUCT A PIPELINE
    //TODO: should error out if there are more than 4 descriptors set(not bits) are set, since I am only limited to 4
    //the first 3 could honestly be static for a given type like mesh,
    //will always need global ubo, materials, and ssbo's, but the 4th is custom
    PIPELINE_TYPE_GLOBAL_UBO = BITFLAG(1),
    PIPELINE_TYPE_MESH = BITFLAG(2),
    PIPELINE_TYPE_MATERIAL = BITFLAG(3),
    PIPELINE_TYPE_BUFFERS = BITFLAG(4),

    //this is more describing what the pipeline is, rather than what it requires
    PIPELINE_TYPE_OPAQUE = BITFLAG(5),
    PIPELINE_TYPE_TRANSPARENCY = BITFLAG(6),
    PIPELINE_TYPE_LIGHTS = BITFLAG(7),
    PIPELINE_TYPE_TERRAIN = BITFLAG(8),
} vulkan_shader_pipeline_type;

typedef struct vulkan_shader_pipeline
{
    VkPipelineLayout pipeline_layout;
    VkPipeline handle;
} vulkan_shader_pipeline;


typedef struct Shader_System
{
    Texture default_texture;
    Texture textures[100];
    // count up for now, releasing is another issue
    u32 available_texture_indexes;


    vulkan_shader_pipeline pipeline_references[100];
    u32 pipeline_indexes;

    Texture_Handle default_texture_handle;
    Material_Handle default_material_handle;

    //idk if i need these two rn
    Shader_Handle default_shader_handle;
    Pipeline_Handle default_pipeline_handle;

    Material_Param_Data material_params[100];
    u32 material_param_indexes;

    //TODO: this should hold all the global buffers and things for descriptor set
    Buffer_Handle material_mesh_ssbo_handle;


    u32 max_indexes;
} Shader_System;


typedef struct Buffer_System
{
    u32 frames_in_flight;

    //NOTE: buffers are handles to the first instance of them, multiple are created for each frame in flight
    // but when we get them from the system, they are offset by the current frame

    Buffer_Handle global_ubo_handle;

    //an array of them
    vulkan_buffer* buffers;
    u32 buffers_size; // total we have
    u32 buffer_current_count; // current amount given out
    //add a linked list in later for buffers we free

    vulkan_buffer* staging_buffer_ring;
    u32 staging_buffer_count;


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
    // Descriptor_Type descriptor_type; // uniform, textures, sbo
} vulkan_bindless_descriptors;

typedef struct vulkan_shader_texture
{
    Texture texture_test_object;

    vulkan_shader_pipeline shader_texture_pipeline;

    vulkan_buffer vertex_buffer;
    vulkan_buffer index_buffer;
    vertex_info vertex_info;
} vulkan_shader_texture;


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



    //TODO: Clean this up
    vulkan_buffer vertex_buffer;
    vulkan_buffer index_buffer;
    vertex_info default_vertex_info;
    vulkan_shader_default default_shader_info;


    //textured triangle
    vulkan_shader_texture shader_texture;
    vulkan_shader_texture shader_texture_bindless;


    //temp
    vulkan_mesh_default mesh_default;


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
} Light_System;

typedef enum Render_Mode
{
    RENDER_MODE_NONE,
    RENDER_MODE_NORMAL,
    RENDER_MODE_LIGHTING,
    RENDER_MODE_MAX,
} Render_Mode;

typedef struct uniform_buffer_object
{
    mat4 model;
    mat4 view;
    mat4 proj;


    VkDeviceAddress directional_lights_address;
    // which one do we want to use for this particular instance (might be better as a push constant)
    // u32 directional_light_index;
    VkDeviceAddress point_lights_address;
    u32 point_lights_count;

    //camera
    vec4 camera_position;

    Render_Mode render_mode;

    u32 _padding0;
    u32 _padding1;
    u32 _padding2;
    u32 _padding3;
    u32 _padding4;
    u32 _padding5;
} uniform_buffer_object;


typedef enum mesh_pipeline_flags
{
    MESH_PIPELINE_COLOR = BITFLAG(1),
    MESH_PIPELINE_NORMAL = BITFLAG(2),
    MESH_PIPELINE_EMISSIVE = BITFLAG(3),
    MESH_PIPELINE_ROUGHNESS = BITFLAG(4),
    MESH_PIPELINE_METALLIC = BITFLAG(5),
    MESH_PIPELINE_AO = BITFLAG(6),
    MESH_PIPELINE_ENUM_MAX,
    MESH_PIPELINE_ENUM_COUNT = 6,
} mesh_pipeline_flags;

const char* mesh_pipeline_flag_names[] = {
    "Albedo",
    "Normal",
    "Emissive",
    "Roughness",
    "Metallic",
    "AO"
};
String mesh_pipeline_flag_names_string[] = {
    {"Albedo", sizeof("Albedo") - 1},
    {"Normal", sizeof("Normal") - 1},
    {"Emissive", sizeof("Emissive") - 1},
    {"Roughness", sizeof("Roughness") - 1},
    {"Metallic", sizeof("Metallic") - 1},
    {"AO", sizeof("AO") - 1},
};



typedef struct PipelinePermutation
{
    //instead of storing every permutation known to man, we just store the ones we are actually going to use
    //we could also load them from a config file later
    darray_type(String*) debug_shader_name;
    darray_type(uint32_t*) permutation_keys; // stores the index of all permutations that are in use
    u32 permutations_count;



} Mesh_Pipeline_Permutations;



typedef struct Descriptor_System
{
    VkDescriptorPool bindless_descriptor_pool;

    vulkan_bindless_descriptors uniform_descriptors;
    vulkan_bindless_descriptors texture_descriptors;
    vulkan_bindless_descriptors storage_descriptors;

    u32 uniform_count;
    u32 texture_count;
    u32 storage_count;

    u32 max_count;
} Descriptor_System;


typedef struct Mesh_System
{
    //NOTE: this is just me figuring out how to handle buffers for a particular system
    //it would also make sense to keep reuploading the meshes into the buffers every frame, well see if its a performance penality

    //global count of all the data
    static_mesh static_mesh_array[1000];
    u32 static_mesh_array_size;
    u32 static_mesh_submesh_size;

    //total size of all mesh data
    size_t vertex_byte_size;
    size_t index_byte_size;
    size_t index_count_size;
    size_t normals_byte_size;
    size_t tangent_byte_size;
    size_t uv_byte_size;

    Buffer_Handle vertex_buffer_handle;
    Buffer_Handle index_buffer_handle;
    Buffer_Handle indirect_buffer_handle; // basically our draw data
    Buffer_Handle normal_buffer_handle;
    Buffer_Handle uv_buffer_handle;
    Buffer_Handle tangent_buffer_handle;

    Buffer_Handle transform_buffer_handle;
    Buffer_Handle material_buffer_handle;

    darray_type(Mesh_Pipeline_Permutations*) mesh_shader_permutations;


    //NOTE: NOT IN USE
    skinned_mesh* skinned_meshes;
    u32 skinned_mesh_size;


    Buffer_Handle bone_buffer_handle;
    Buffer_Handle weights_buffer_handle;

    // darray_type(VkDrawIndexedIndirectCommand*) indirect_draw_data;
    u32 indirect_draw_count;

    PC_Mesh pc_mesh;
} Mesh_System;


typedef struct renderer
{
    camera main_camera;
    Render_Mode mode;

    Arena arena; // total memory for the entire renderer
    Arena frame_arena;

    Shader_System* shader_system;
    Buffer_System* buffer_system;
    Light_System* light_system;
    Mesh_System* mesh_system;
    Descriptor_System* descriptor_system;


    //mesh system
    //animation system
    //ui draw info

    //TODO:
    vulkan_context context;

    //pipelines
    vulkan_shader_pipeline indirect_mesh_pipeline;
    vulkan_shader_pipeline ui_pipeline;
    vulkan_shader_pipeline text_pipeline;
} renderer;


#endif //VULKAN_TYPES_H
