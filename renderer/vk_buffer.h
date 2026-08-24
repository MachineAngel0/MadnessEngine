#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include "vulkan_struct_types.h"

Buffer_System* buffer_system_init(Renderer* renderer, u32 frames_in_flight);
Buffer_System* buffer_system_free(Renderer* renderer);

void buffer_system_frame_start(Buffer_System* buffer_system, u32 current_frame);
void buffer_system_frame_end(Buffer_System* buffer_system, u32 current_frame);


void _vulkan_buffer_create_internal_new(Renderer* renderer, Vulkan_Buffer_Type buffer_type, u64 buffer_size,
                                        Vulkan_Buffer* buffer_to_create);
void _vulkan_buffer_destroy_internal(Renderer* renderer, Vulkan_Buffer* buffer);

bool vulkan_buffer_free(Renderer* renderer, Vulkan_Buffer* vk_buffer);


Buffer_Handle vulkan_buffer_create(Renderer* renderer, Buffer_System* buffer_system, Vulkan_Buffer_Type buffer_type,
                                   u64 buffer_size);
Buffer_Frame_Handle vulkan_buffer_create_frame(Renderer* renderer, Buffer_System* buffer_system,
                                               Vulkan_Buffer_Type buffer_type,
                                               u64 buffer_size);

Vulkan_Buffer* vulkan_buffer_get(Renderer* renderer, Buffer_Handle buffer_handle);
Vulkan_Buffer* vulkan_buffer_get_frame(Renderer* renderer,
                                       Buffer_Frame_Handle frame_buffer_handle);

void vulkan_buffer_reset(Renderer* renderer, Buffer_Handle buffer_handle);
void vulkan_buffer_frame_reset(Renderer* renderer, Buffer_Frame_Handle frame_buffer_handle);


//TODO: for uploads we need at startup, will cause a queue wait idle
bool vulkan_buffer_startup_uploads(Renderer* renderer, Buffer_Handle buffer_handle,
                                   void* data, u64 data_byte_size);
/**
 * @note: buffer barriers are handled externally, and not inside this function
 * @note: if zero is passed in as size, false is returned, do not use a barrier in this case
 */
bool vulkan_buffer_frame_staging_upload(Renderer* renderer,
                                        Buffer_Frame_Handle buffer_handle,
                                        Vulkan_Command_Buffer* command_buffer,
                                        void* data, u64 data_byte_size);




/**
 * @note: buffer barriers are handled externally, and not inside this function
 *
 */
//ask for a region of memory, and the buffer manager will see if the request will go through, giving back a start_offset
bool vulkan_buffer_upload_data_request(Renderer* renderer, u64 memory_request_size, u64* out_start_offset);
bool vulkan_buffer_transfer_upload(Renderer* renderer, Vulkan_Command_Buffer* command_buffer, Buffer_Handle handle,
                                   void* data, u64 data_size, u64 semaphore_value);


//Utility
uint32_t find_memory_type(Renderer* renderer, uint32_t type_filter, VkMemoryPropertyFlags properties);
void vulkan_buffer_staging_copy_range(Vulkan_Command_Buffer* command_buffer,
                              Vulkan_Buffer* buffer, Vulkan_Buffer* staging_buffer, void* data, u64 data_size,
                              u64 staging_offset, u64 buffer_offset);

// Get device address of a buffer
VkDeviceAddress get_buffer_device_address(VkDevice device, VkBuffer buffer);
VkDeviceAddress vulkan_buffer_get_frame_device_address(Renderer* renderer, Buffer_Frame_Handle buffer_handle);
VkDeviceAddress vulkan_buffer_get_device_address(Renderer* renderer, Buffer_Handle buffer_handle);

#endif //VK_BUFFER_H
