#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include "math_types.h"


typedef struct Transform
{
    vec3 position; //literally no need to create a vec2 version of this
    quat rotation;
    vec3 scale;
    mat4 local;
    bool is_dirty;
    struct Transform* parent;
} Transform;

typedef struct Transform_SOA
{
    vec3* position;
    quat* rotation;
    vec3* scale;
    mat4* local;
}Transform_SOA;


//create operations
MAPI Transform* transform_from_position_rotation_scale(const vec3 position, const quat rotation, const vec3 scale,
                                                       Arena* arena);
MAPI Transform* transform_create(Arena* arena);

MAPI Transform* transform_from_position(const vec3 position, Arena* arena);
MAPI Transform* transform_from_rotation(const quat rotation, Arena* arena);

MAPI Transform* transform_from_scale(const vec3 scale, Arena* arena);


//set operations
MAPI void transform_set_position(Transform* t, const vec3 position);

MAPI void transform_set_rotation(Transform* t, const quat rotation);
MAPI void transform_set_scale(Transform* t, const vec3 scale);
MAPI void transform_set_parent(Transform* t, Transform* parent);

//perform operation
MAPI void transform_translate(Transform* t, const vec3 position);
MAPI void transform_rotate(Transform* t, const quat rotation);
MAPI void transform_scale(Transform* t, const vec3 scale);

//

mat4 transform_get_local(Transform* t);

mat4 transform_get_world(Transform* t);


#endif //TRANSFORMS_H
