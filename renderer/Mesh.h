#ifndef MESH_H
#define MESH_H

#include <vector>
#include "glm/glm.hpp"


struct VERTEX_DYNAMIC_INFO;
struct Vertex;

// Modified create_quad function to properly position quads
std::vector<Vertex> create_quad(glm::vec2 pos, glm::vec3 color, float scale);

int add_quad(glm::vec2 pos, glm::vec3 color, float scale, VERTEX_DYNAMIC_INFO& vertex_info);

void move_quad(int id, VERTEX_DYNAMIC_INFO& vertex_info, glm::vec2 move_amount);


#endif
