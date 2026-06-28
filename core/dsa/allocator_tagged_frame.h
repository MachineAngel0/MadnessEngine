#ifndef ALLOCATOR_TAGGED_FRAME_H
#define ALLOCATOR_TAGGED_FRAME_H


typedef enum Allocator_Frame_Tag
{
    Allocator_Frame_Tag_Game,
    Allocator_Frame_Tag_Renderer,
    Allocator_Frame_Tag_Game_To_Renderer,
    Allocator_Frame_Tag_CPU_To_GPU,
}Allocator_Frame_Tag;


typedef struct Allocator_Tagged_Frame
{
    void* memory;
    u64 capacity;
    u64 used;
    // might not want this, because sometimes its really just cause we want something for a certain period of time,
    // that isn't always a frame count
    u32 frame_duration;
    Allocator_Frame_Tag tag;
} Allocator_Tagged_Frame;


void allocator_tagged_frame_init(Allocator_Tagged_Frame* allocator, Allocator_Frame_Tag tag, void* backing_memory, u64 memory_size)
{

}
void* allocator_tagged_frame_destroy(Allocator_Tagged_Frame* allocator)
{
    return NULL;
}


void* allocator_tagged_frame_alloc(Allocator_Tagged_Frame* allocator, size_t size)
{
    return NULL;
}
void allocator_tagged_frame_reset(Allocator_Tagged_Frame* allocator, void* ptr)
{

}


//TODO:
// void pool_allocator_test(void);


#endif//ALLOCATOR_TAGGED_FRAME_H
