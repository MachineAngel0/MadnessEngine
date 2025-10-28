//
// Created by Adams Humbert on 9/10/2025.
//

#ifndef VK_VERTEX_H
#define VK_VERTEX_H
#include <array>
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
};


const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};


const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};


struct VERTEX_DYNAMIC_INFO
{
    std::vector<Vertex> dynamic_vertices{};
    std::vector<uint16_t> dynamic_indices{};

    bool vertex_buffer_should_update = false;

    int mesh_id = 0;

};

inline void clear_vertex_info(VERTEX_DYNAMIC_INFO& vertex_info)
{
    vertex_info.dynamic_vertices.clear();
    vertex_info.dynamic_indices.clear();
    vertex_info.mesh_id = 0;
    vertex_info.vertex_buffer_should_update = true;
}


static VkVertexInputBindingDescription getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    /*
    * VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
    VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
     */
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

    //position
    attributeDescriptions[0].binding = 0; //referencing which VkVertexInputBindingDescription binding we are using
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos); //offsetof is pretty interesting

    //color
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}

//ubo using a descriptor set
/*
* Specify a descriptor set layout during pipeline creation
*Allocate a descriptor set from a descriptor pool
*Bind the descriptor set during rendering
 */



#endif //VK_VERTEX_H
