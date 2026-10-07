#ifndef MADNESSENGINE_REFLECTION_TYPES_H
#define MADNESSENGINE_REFLECTION_TYPES_H


#include "defines.h"
#include "math_types.h"
#include "UUID.h"


//TODO: Move this out to defines and call it Madness_Types and name them {MADNESS_TYPE_U8}
typedef enum Reflection_Type
{
    REFLECTION_TYPE_INVALID,
    REFLECTION_TYPE_U8,
    REFLECTION_TYPE_U16,
    REFLECTION_TYPE_U32,
    REFLECTION_TYPE_U64,
    REFLECTION_TYPE_S8,
    REFLECTION_TYPE_S16,
    REFLECTION_TYPE_S32,
    REFLECTION_TYPE_S64,
    REFLECTION_TYPE_F32,
    REFLECTION_TYPE_F64,
    REFLECTION_TYPE_SIZE_T,
    REFLECTION_TYPE_BOOL,
    REFLECTION_TYPE_CHAR,
    REFLECTION_TYPE_CHAR_STRING, // char*
    REFLECTION_TYPE_STRING, // String*
    REFLECTION_TYPE_PATH_STRING, // Path_String*
    REFLECTION_TYPE_ENUM,
    REFLECTION_TYPE_STRUCT,

    REFLECTION_TYPE_VEC2,
    REFLECTION_TYPE_VEC3,
    REFLECTION_TYPE_VEC4,
    REFLECTION_TYPE_MAT3,
    REFLECTION_TYPE_MAT4,

    REFLECTION_TYPE_UUID,

    REFLECTION_TYPE_MAX,
} Reflection_Type;


u8 Reflection_Type_Size_LUT[REFLECTION_TYPE_MAX] = {
    [REFLECTION_TYPE_INVALID] = 0,
    [REFLECTION_TYPE_U8] = sizeof(u8),
    [REFLECTION_TYPE_U16] = sizeof(u16),
    [REFLECTION_TYPE_U32] = sizeof(u32),
    [REFLECTION_TYPE_U64] = sizeof(u64),
    [REFLECTION_TYPE_S8] = sizeof(s8),
    [REFLECTION_TYPE_S16] = sizeof(s16),
    [REFLECTION_TYPE_S32] = sizeof(s32),
    [REFLECTION_TYPE_S64] = sizeof(s64),
    [REFLECTION_TYPE_F32] = sizeof(f32),
    [REFLECTION_TYPE_F64] = sizeof(f64),
    [REFLECTION_TYPE_SIZE_T] = sizeof(size_t),
    [REFLECTION_TYPE_BOOL] = sizeof(bool),
    [REFLECTION_TYPE_CHAR] = sizeof(char),
    [REFLECTION_TYPE_CHAR_STRING] = sizeof(char*),
    [REFLECTION_TYPE_STRING] = sizeof(String),
    [REFLECTION_TYPE_PATH_STRING] = sizeof(Path_String),
    [REFLECTION_TYPE_ENUM] = sizeof(u32),
    [REFLECTION_TYPE_STRUCT] = 0,
    [REFLECTION_TYPE_VEC2] = sizeof(vec2s),
    [REFLECTION_TYPE_VEC3] = sizeof(vec3s),
    [REFLECTION_TYPE_VEC4] = sizeof(vec4s),
    [REFLECTION_TYPE_MAT3] = sizeof(mat3s),
    [REFLECTION_TYPE_MAT4] = sizeof(mat4s),
    [REFLECTION_TYPE_UUID] = sizeof(MADNESS_UUID),
};


Reflection_Type Compiler_type_to_Reflection_Type_LUT[] = {
    [Token_Unknown] = REFLECTION_TYPE_INVALID,
    [Token_U8] = REFLECTION_TYPE_U8,
    [Token_U16] = REFLECTION_TYPE_U16,
    [Token_U32] = REFLECTION_TYPE_U32,
    [Token_U64] = REFLECTION_TYPE_U64,
    [Token_S8] = REFLECTION_TYPE_S8,
    [Token_S16] = REFLECTION_TYPE_S16,
    [Token_S32] = REFLECTION_TYPE_S32,
    [Token_S64] = REFLECTION_TYPE_S64,
    [Token_F32] = REFLECTION_TYPE_F32,
    [Token_F64] = REFLECTION_TYPE_F64,
    [Token_Char] = REFLECTION_TYPE_CHAR,
    [Token_Bool] = REFLECTION_TYPE_BOOL,
    [Token_String_Type] = REFLECTION_TYPE_STRING,
    [Token_Path_String] = REFLECTION_TYPE_PATH_STRING,
    [Token_String] = REFLECTION_TYPE_CHAR_STRING,
    [Token_Size_t] = REFLECTION_TYPE_SIZE_T,
    [Token_Enum] = REFLECTION_TYPE_ENUM,
    [Token_Struct] = REFLECTION_TYPE_STRUCT,
    [Token_Identifier] = REFLECTION_TYPE_STRUCT,
    [TOKEN_VEC2] = REFLECTION_TYPE_VEC2,
    [TOKEN_VEC3] = REFLECTION_TYPE_VEC3,
    [TOKEN_VEC4] = REFLECTION_TYPE_VEC4,
    [TOKEN_MAT3] = REFLECTION_TYPE_MAT3,
    [TOKEN_MAT4] = REFLECTION_TYPE_MAT4,
    [TOKEN_UUID] = REFLECTION_TYPE_UUID,
};

/**
 * @note generally should only be used for basic types, complex types like struct will return 0
 */
MINLINE u8 reflection_type_get_size(Reflection_Type type)
{
    return Reflection_Type_Size_LUT[type];
}



size_t reflection_str_to_type(char* str)
{






    if (strcmp(str, "u8") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_U8];

    }
    if (strcmp(str, "u16") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_U16];

    }
    if (strcmp(str, "u32") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_U32];
    }
    if (strcmp(str, "u64") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_U64];
    }

    if (strcmp(str, "s8") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_S8];
    }
    if (strcmp(str, "s16") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_S16];
    }
    if (strcmp(str, "s32") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_S32];
    }
    if (strcmp(str, "s64") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_S64];
    }

    if (strcmp(str, "f32") == 0 || strcmp(str, "float") == 0)
    {
           return Reflection_Type_Size_LUT[REFLECTION_TYPE_F32];
    }
    if (strcmp(str, "f64") == 0 || strcmp(str, "double") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_F64];
    }
    if (strcmp(str, "size_t") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_SIZE_T];
    }


    if (strcmp(str, "enum") == 0 )
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_ENUM];
    }


    if (strcmp(str, "bool") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_BOOL];
    }

    if (strcmp(str, "char") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_CHAR];
    }
    if (strcmp(str, "char*") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_CHAR_STRING];
    }
    if (strcmp(str, "str") == 0 || strcmp(str, "string") == 0 || strcmp(str, "String") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_STRING];
    }
    if (strcmp(str, "Path_String") == 0 )
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_PATH_STRING];
    }

    if (strcmp(str, "vec2") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_VEC2];
    }
    if (strcmp(str, "vec3") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_VEC3];
    }
    if (strcmp(str, "vec4") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_VEC4];
    }
    if (strcmp(str, "mat3") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_MAT3];
    }
    if (strcmp(str, "mat4") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_MAT4];
    }
    if (strcmp(str, "MADNESS_UUID") == 0)
    {
        return Reflection_Type_Size_LUT[REFLECTION_TYPE_UUID];
    }

    return 0;
}

#endif //MADNESSENGINE_REFLECTION_TYPES_H