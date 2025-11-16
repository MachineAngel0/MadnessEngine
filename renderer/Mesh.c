
#include "Mesh.h"

#include "Tetris.h"

std::vector<Vertex> create_quad(glm::vec2 pos, glm::vec3 color, float scale)
{
    return {
        {{pos.x - scale, pos.y - scale}, {color}},
        {{pos.x + scale, pos.y - scale}, {color}},
        {{pos.x + scale, pos.y + scale}, {color}},
        {{pos.x - scale, pos.y + scale}, {color}}
    };
}

int add_quad(glm::vec2 pos, glm::vec3 color, float scale, VERTEX_DYNAMIC_INFO& vertex_info)
{
    std::vector<Vertex> new_quad = create_quad(pos, color, scale);
    uint16_t base_index = static_cast<uint16_t>(vertex_info.dynamic_vertices.size());

    // Add vertices
    vertex_info.dynamic_vertices.insert(vertex_info.dynamic_vertices.end(), new_quad.begin(), new_quad.end());

    // Add indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        static_cast<uint16_t>(base_index + 0),
        static_cast<uint16_t>(base_index + 1),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 3),
        static_cast<uint16_t>(base_index + 0)
    };

    vertex_info.dynamic_indices.insert(vertex_info.dynamic_indices.end(), quad_indices.begin(), quad_indices.end());


    vertex_info.vertex_buffer_should_update = true;

    return vertex_info.mesh_id++;

    /*
    printf("Added quad at (%.2f, %.2f) with color (%.2f, %.2f, %.2f)\n",
           pos.x, pos.y, color.r, color.g, color.b);
    printf("Total vertices: %zu, Total indices: %zu\n", dynamic_vertices.size(), dynamic_indices.size());
    */
}

void move_quad(int id, VERTEX_DYNAMIC_INFO& vertex_info, glm::vec2 move_amount)
{
    int index_into_stride = id * 4;
    for (int i=0; i < 4; i++)
    {
        vertex_info.dynamic_vertices[index_into_stride+i].pos += move_amount;
    }

    vertex_info.vertex_buffer_should_update = true;

}


