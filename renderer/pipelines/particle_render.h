#ifndef PARTICLE_RENDER_H
#define PARTICLE_RENDER_H




Particle_Render* particle_renderer_init(Renderer* renderer);


void particle_renderer_upload_data_draw(Renderer* renderer, Particle_Render* particle_render,
                                             Render_Packet* render_packet, Vulkan_Command_Buffer* command_buffer);


void particle_renderer_batch_draw(Renderer* renderer, Particle_Render* particle_render,
                                  Vulkan_Command_Buffer* command_buffer);
#endif //PARTICLE_RENDER_H
