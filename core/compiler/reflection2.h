#ifndef REFLECTION2_H
#define REFLECTION2_H

#include "allocator.h"
#include "madness_lexer.h"
#include "reflection_system.h"


#define HEADER_FILE_LIST_COUNT 10

typedef struct Reflection2
{
    Dynamic_Array* reflection_registry_constants;
    Dynamic_Array* reflection_registry_enums;
    Dynamic_Array* reflection_registry_structs;

    Allocator* allocator;
    Frame_Allocator* frame_allocator;

    String** header_file_list;
    u32 header_file_list_count;
    u32 header_file_list_capacity;
} Reflection2;


typedef struct Reflection2_Struct_Field
{
    const char* field_name;

    enum struct_field_type struct_field_oasdh;

    /*//what if a struct is nested inside?
    union
    {
        struct
        {
            // size is implicit in the type
            const char* type_name;
            Reflection_Type type;
        };

        struct
        {
            // points to the struct, which contains our reflection info for this field
            Reflection_Struct* struct_pointer;
        };

        struct
        {
            // points to our known container types like arrays/ dyamic arrays
            //with an array, we just want to know the stride of the thing, everything else, lets assume we can query
            struct Reflection_Container* struct_pointer;
        };
    };*/

    // Reflection_Container_Type container_type; // var, pointer, array (this is honestly shitty, should be encoded into the type)
} Reflection2_Struct_Field;

struct Reflection2_Struct
{
    u32 struct_name;
    Reflection2_Struct_Field* fields;
    u32 count;
};

typedef void* (*Reflect_Enums)(Reflection_Registry* reflection_registry);
typedef void* (*Reflect_Structs)(Reflection_Registry* reflection_registry);

typedef struct reflection_dll
{
    Reflect_Structs structs_function;
    Reflect_Enums enum_function;
    DLL_HANDLE dll;
} reflection_dll;

void reflection_init(Input_System* input_system, reflection_dll* reflection_dll)
{


}



void reflection_dll_load(Input_System* input_system, reflection_dll* reflection_dll)
{
    reflection_dll->dll = platform_load_dynamic_library("./REFLECTION");

    if (reflection_dll->dll.handle == 0)
    {
        MASSERT(false);
        return;
    }


    reflection_dll->enum_function = (Reflect_Enums)platform_get_function_address(
        reflection_dll->dll, "generate_runtime_enums_material");
    reflection_dll->structs_function = (Reflect_Structs)platform_get_function_address(
        reflection_dll->dll, "generate_runtime_enums_material");

    if (!reflection_dll->structs_function)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER INITIALIZATION")
        return;
    }
    if (!reflection_dll->enum_function)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER RUN")
        return;
    }
}

void dll_reload(Reflection2* reflections, Input_System* input_system)
{
    // platform_reload_dynamic_library()
}


//what about serialization?????
// type-> write type size
// array-> write type size * count
// arrays should be treated as their own types at this point
// all arrays should be treated as dynamic arrays, runtime can serialize it normally
// we will not support c arrays (probably)

//options for serialization
// list of type info
// data
// or
// type info, data,
// type info, data,
// type info, data,
//first option makes sense for binary
//second option makes sense for text formats


#endif //REFLECTION2_H
