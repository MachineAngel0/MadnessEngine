#ifndef VK_BUFFER_H
#define VK_BUFFER_H


//TODO: at some point it would make sense to allocate something between 32-128mb of vertex buffers
// at any one time and keep a bunch of them allocated in some sort of free list ready to be used or make them on the fly

Buffer_System* buffer_system_init(renderer* renderer, u32 frames_in_flight);
Buffer_System* buffer_system_free(renderer* renderer);


uint32_t find_memory_type(vulkan_context* context, uint32_t type_filter, VkMemoryPropertyFlags properties);


//TODO: used in texture image, replace with a storage buffer call or retrieve
bool buffer_create(vulkan_context* vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);


void buffer_copy(vulkan_context* vulkan_context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void buffer_copy_region(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                        VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                        VkDeviceSize srcOffset, VkDeviceSize dstOffset);

//new API


//NOTE: since basically every buffer needs a staging buffer (except uniform), might as well create them upfront without asking, unless otherwise specified
// and it would be fine if we have holes in our arrays staging buffers array, since we only need to access them by index, not by looping
Buffer_Handle vulkan_buffer_create(renderer* renderer, Buffer_System* buffer_system,
                                   vulkan_buffer_type buffer_type, u64 data_size);


//TODO: so there are instances in which a buffer is associated with its staging buffer, and having to manually handle both is really annoying
Buffer_Handle vulkan_buffer_create_with_staging_buffer(renderer* renderer, Buffer_System* buffer_system,
                                   vulkan_buffer_type buffer_type, u64 data_size);


void _vulkan_buffer_create_internal(renderer* renderer, vulkan_buffer* out_buffer, vulkan_buffer_type buffer_type,
                                    u64 data_size);

bool vulkan_buffer_free(renderer* renderer, vulkan_buffer* vk_buffer);

vulkan_buffer* vulkan_buffer_get(renderer* renderer, Buffer_Handle buffer_handle);
void vulkan_buffer_reset_offset(renderer* renderer, Buffer_Handle buffer_handle);

//clears the buffer when you get it
vulkan_buffer* vulkan_buffer_get_clear(renderer* renderer, Buffer_Handle buffer_handle);


//copies data (like vertex or index data) into a staging buffer
void vulkan_buffer_data_copy_from_offset(renderer* renderer, Buffer_Handle staging_buffer_handle,
                                         void* data, u64 data_size);

//copies data from a staging buffer into a device local buffer for gpu usage
void vulkan_buffer_copy(renderer* renderer, Buffer_Handle buffer_handle, Buffer_Handle staging_buffer_handle);

//copy data into a staging buffer and immediatelyy upload it into a local buffer
//best used if there is only a single upload into the staging buffer
//dont use if there are a lot of data copies into the staging buffer, copies into the device local can be slow if done many times
void vulkan_buffer_data_copy_and_upload(renderer* renderer, Buffer_Handle buffer_handle,
                                        Buffer_Handle staging_buffer_handle,
                                        void* data, u64 data_size);


/* TODO: dont need rn but could use later
//for inserting data into a specific memory region of the buffer
void vulkan_buffer_data_insert_specify_offset(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                               vulkan_buffer* buffer, vulkan_buffer* staging_buffer, void* data, u64 data_size, u64 offset);
*/


#endif //VK_BUFFER_H
