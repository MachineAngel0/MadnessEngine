#ifndef VULKANFROMSPECS_H
#define VULKANFROMSPECS_H

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>


struct UI_STATE;
struct Descriptor;
struct Text_System;
struct Command_Buffer_Context;
struct Buffer_Context;
struct Vulkan_Context;
struct UI_DRAW_INFO;
struct Game_State;
struct VERTEX_DYNAMIC_INFO;


//TODO: move out when done
void key_callback(GLFWwindow* window, Game_State* game_state, VERTEX_DYNAMIC_INFO& vertex_info);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);







constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct GLFW_Window_Context
{
    GLFWwindow* window;
    const char* WINDOW_NAME = "MadnessEngine";
    const int WIDTH = 800;
    const int HEIGHT = 600;
    bool framebufferResized = false;
};



struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Swapchain_Context
{
    VkSwapchainKHR swapchain;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkExtent2D surface_extent;
    //also contains VKformat
    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR presentModes;
    std::vector<VkImage> swap_chain_images;
    std::vector<VkImageView> swap_chain_image_views;

};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
};


struct Graphics_Context
{
    VkRenderPass render_pass;
    std::vector<VkFramebuffer> frame_buffers;
    VkPipeline graphics_pipeline;
    VkPipelineLayout pipeline_layout;
};


struct Semaphore_Fences_Context
{
    std::vector<VkSemaphore> image_available_semaphore;
    std::vector<VkSemaphore> render_finished_semaphore;
    std::vector<VkFence> in_flight_fence;
    //idk if its right for these two to be here, might be better in the swapchain context
    uint32_t currentFrame = 0;
};


/*MAIN LOOP*/
void run();


void init_vulkan(Vulkan_Context& vulkan_context,
                 GLFW_Window_Context& window_info,
                 Swapchain_Context& swapchain_context,
                 Graphics_Context& graphics_context,
                 Buffer_Context& buffer_context,
                 Command_Buffer_Context& command_buffer_context, Semaphore_Fences_Context& semaphore_fences_context);

void init_UI_vulkan(Vulkan_Context& vulkan_context, Swapchain_Context& swapchain_context, Graphics_Context& ui_graphics_context,
                    Graphics_Context& graphics_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& ui_buffer_context, UI_STATE* ui_state);

void init_Text_vulkan(Vulkan_Context& vulkan_context, Swapchain_Context& swapchain_context,
                      Graphics_Context& text_graphics_context, Graphics_Context& graphics_context,
                      Command_Buffer_Context& command_buffer_context, Buffer_Context& text_buffer_context, Text_System& text_system, Descriptor
                      & text_descriptor);





void draw_frame(Vulkan_Context& vulkan_context, GLFW_Window_Context& window_context, Swapchain_Context& swapchain_context,
                Graphics_Context& graphics_context, Command_Buffer_Context& command_buffer_context,
                Buffer_Context& buffer_context, VERTEX_DYNAMIC_INFO& vertex_info, Semaphore_Fences_Context& semaphore_fences_info,
                Graphics_Context& ui_graphics_context, Buffer_Context& ui_buffer_context, UI_STATE* ui_state,
                Graphics_Context& text_graphics_context, Buffer_Context& text_buffer_context, Descriptor& text_descriptor);


/*CLEANUP*/
void cleanup(Vulkan_Context& vulkan_context, GLFW_Window_Context& window_info,
             Swapchain_Context& swapchain_context, Graphics_Context& graphics_context,
             Command_Buffer_Context& command_buffer_context, Buffer_Context&
             buffer_context, Semaphore_Fences_Context& semaphore_fences_context);

/* GLFW WINDOW*/
void init_window(GLFW_Window_Context& context);

static void framebufferResizeCallback(GLFWwindow* window, int width, int height);


/* SURFACE */
void create_surface(Vulkan_Context& vulkan_context, GLFW_Window_Context& window_context);

/* PHYSICAL DEVICE*/
void pick_physical_device(Vulkan_Context& vulkan_context);

bool is_device_suitable(VkSurfaceKHR surface, VkPhysicalDevice physical_device_to_query);

QueueFamilyIndices find_queue_families(VkSurfaceKHR surface, VkPhysicalDevice physical_device);

bool queue_family_indices_is_complete(QueueFamilyIndices queue_family_indices);


/* LOGICAL DEVICE */
void create_logical_device(Vulkan_Context& vulkan_context);

/* SWAPCHAIN */
void create_swapchain(Vulkan_Context& vulkan_context, Swapchain_Context& swapchain_context);

VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);

VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& present_modes_available);

/* SWAPCHAIN RECREATION */
void recreate_swapchain(Vulkan_Context& vulkan_context, GLFW_Window_Context& window_context,
                        Swapchain_Context& swapchain_context, Graphics_Context& graphics_context, UI_STATE* ui_state);

void cleanup_swapchain(Vulkan_Context& vulkan_context, Swapchain_Context& swapchain_context,
                       Graphics_Context& graphics_context);


/* IMAGE VIEWS */
void create_image_views(Vulkan_Context& vulkan_context, Swapchain_Context& swapchain_context);

/* GRAPHICS PIPELINE*/
void create_graphics_pipeline(Vulkan_Context& vulkan_context, Swapchain_Context& swapchain_context, Graphics_Context& graphics_context);

std::vector<char> read_shader_file(const std::string& filename);

VkShaderModule create_shader_module(VkDevice& logical_device, const std::vector<char>& code);

/*RENDER PASS*/ //TODO: replace with dynamic rendering local read
void create_render_pass(Vulkan_Context& vulkan_context, Swapchain_Context& swapchain_context,
                        Graphics_Context& graphics_context);

/*FRAMEBUFFER*/ //TODO: replace with dynamic rendering local read
void create_frame_buffers(Vulkan_Context& vulkan_context, Swapchain_Context& swapchain_context,
                          Graphics_Context& graphics_context);


/*VERTEX BUFFER*/
void create_vertex_buffer(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context);





/*INDEX BUFFER*/
void create_index_buffer(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context);

/*SEMAPHORES AND FENCES*/
void create_sync_objects(Vulkan_Context& vulkan_context, Semaphore_Fences_Context& semaphore_fences_info);


/*RECORD BUFFER*/
void record_command_buffer(Swapchain_Context& swapchain_context, Command_Buffer_Context& command_buffer_context,
                           Graphics_Context& graphics_context, Buffer_Context& buffer_context, VERTEX_DYNAMIC_INFO& vertex_info, uint32_t image_index, uint32_t current_frame, Graphics_Context
                           &ui_graphics_context, Buffer_Context& ui_buffer_context, UI_STATE* ui_state, Graphics_Context&
                           text_graphics_context, Buffer_Context& text_buffer_context, Descriptor& text_descriptor);




// Global dynamic vertex storage




inline bool e_key_pressed = false;
inline bool q_key_pressed = false;
inline bool up_key_pressed = false;
inline bool left_key_pressed = false;
inline bool down_key_pressed = false;
inline bool right_key_pressed = false;
inline bool space_key_pressed = false;




const uint32_t max_object_count = 10000000;
const uint32_t vertices_per_object = 4;
const uint32_t indices_per_object = 6;
const uint32_t MAX_VERTICES = max_object_count * vertices_per_object;
const uint32_t MAX_INDICES = max_object_count * indices_per_object;




//this is the current one in use, it doesn't recreate anything, it just replaces the memory
void update_vertex_buffer_update(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context, VERTEX_DYNAMIC_INFO& vertex_info);

void create_vertex_buffer_new(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context);

void create_index_buffer_new(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context);

void copy_buffer_region(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context,
                        VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                        VkDeviceSize srcOffset, VkDeviceSize dstOffset);



void create_ui_graphics_pipeline(Vulkan_Context& vulkan_context, Graphics_Context& ui_graphics_context, Graphics_Context&
                                 graphics_context_renderpass_only);
void create_text_graphics_pipeline(Vulkan_Context& vulkan_context, Graphics_Context& text_graphics_context, Graphics_Context&
                                   graphics_context_renderpass_only, Descriptor& descriptor);


#endif //VULKANFROMSPECS_H
