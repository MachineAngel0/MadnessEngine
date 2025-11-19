
#ifndef ID_GEN_H
#define ID_GEN_H


#include "defines.h"

typedef struct ID_GEN
{
    u32 id;
    u32 generation; // this can be increased to u64 if needed
}ID_GEN;

MINLINE void increment_generation(ID_GEN id)
{
    id.generation++;
}

MINLINE bool is_same_generation(const ID_GEN system_handle, const ID_GEN up_to_date_handle)
{
    //system handle is the handle that any system has, while up to date is the most recent handle
    return system_handle.generation == up_to_date_handle.generation;
}


/*
 /TODO: I dont have a need for this rn
// but assuming a subsystem wants to assign id's, this would be their struct/interface
typedef struct ID_GEN_INSTANCE
{
    //NOTE: im not making this until i have a concrete need for it, otherwise the api is going to be bad
    //each index is either going to be a 0 or 1. 0 for not in use, and 1 for in use
    // uint8_t id_list[1000]; // arbitrary value for now
    // u32 top_index;
    // u32 free_list_id[1000];

}ID_GEN_INSTANCE;
*/

//NOTE: API whenever I get around to it
// void id_instance_generate(ID_GEN_INSTANCE id_instance);
// void id_instance_free_id(ID_GEN_INSTANCE id_instance);



#endif //ID_GEN_H
