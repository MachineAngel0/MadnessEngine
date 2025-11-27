#ifndef MESH_H
#define MESH_H
#include "defines.h"
#include "vulkan_types.h"

// #include <vector>


typedef struct vertex_mesh
{

}vertex_mesh;


typedef struct Mesh
{
    vertex_mesh* vertices;
    u32* indices;
    Texture* textures;
}Mesh;


// Modified create_quad function to properly position quads
// std::vector<Vertex> create_quad(glm::vec2 pos, glm::vec3 color, float scale);

// int add_quad(glm::vec2 pos, glm::vec3 color, float scale, VERTEX_DYNAMIC_INFO& vertex_info);

// void move_quad(int id, VERTEX_DYNAMIC_INFO& vertex_info, glm::vec2 move_amount);


#endif
