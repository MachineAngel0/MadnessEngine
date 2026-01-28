#ifndef VK_VERTEX_H
#define VK_VERTEX_H



//NOTE: its not that each member needs to be aligned,
// its that the whole struct needs to be aligned to a power of 16 (on my system at least)


/* same thing, as above
typedef struct vertex_3d
{
    alignas(16) vec3 position;
    alignas(16) vec3 color;
} vertex; */




// const vertex_3d test_vertices[] = {
//     {{-0.5f, -0.5f, 0.0f}},
//     {{0.5f, -0.5f, 0.0f}},
//     {{0.5f, 0.5f, 0.0f}},
//     {{-0.5f, 0.5f, 0.0f}},
//
// };
const vertex_3d test_vertices[] = {
    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
};

const vertex_tex texture_vertices[] = {
    {{1.0f + 2.0f, 1.0f + 2.0f, 0.0f + 2.0f}, {1.0f, 0.0f, 0.0f},},
    {{-1.0f + 2.0f, 1.0f + 2.0f, 0.0f + 2.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.0f + 2.0f, -1.0f + 2.0f, 0.0f + 2.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}}
};


//for a square
// const uint16_t test_indices[] = {
// 0, 1, 2, 2, 3, 0,
// };
const uint16_t test_indices[] = {
    0, 1, 2
};



#endif //VK_VERTEX_H
