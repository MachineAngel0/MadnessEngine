#ifndef PARTICLE_RENDER_H
#define PARTICLE_RENDER_H




Particle_Render* particle_renderer_init(Renderer* renderer, Asset_System* resource_system);


void particle_renderer_upload_data_draw(Renderer* renderer, Particle_Render* particle_render,
                                             Render_Packet* render_packet, vulkan_command_buffer* command_buffer);


void particle_renderer_batch_draw(Renderer* renderer, Particle_Render* particle_render,
                                  vulkan_command_buffer* command_buffer);
#endif //PARTICLE_RENDER_H
