#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include "math_types.h"


typedef struct Transform
{
    vec3s position; //literally no need to create a vec2 version of this
    versors rotation;
    vec3s scale;
    mat4s local;
    bool is_dirty;
    struct Transform* parent;
} Transform;

typedef struct Transform_SOA
{
    vec3s* position;
    versors* rotation;
    vec3s* scale;
    mat4s* local;
} Transform_SOA;


//create operations
MAPI Transform* transform_from_position_rotation_scale(vec3s position, versors rotation, vec3s scale,
                                                       Allocator* arena);
MAPI Transform* transform_create(Allocator* arena);
void transform_set_default(Transform* transform);

MAPI Transform* transform_from_position(vec3s position, Allocator* arena);
MAPI Transform* transform_from_rotation(versors rotation, Allocator* arena);

MAPI Transform* transform_from_scale(vec3s scale, Allocator* arena);


//set operations
MAPI void transform_set_position(Transform* t, vec3s position);

MAPI void transform_set_rotation(Transform* t, versors rotation);
MAPI void transform_set_scale(Transform* t, vec3s scale);
MAPI void transform_set_parent(Transform* t, Transform* parent);

//perform operation
MAPI void transform_translate(Transform* t, vec3s position);
MAPI void transform_rotate(Transform* t, versors rotation);
MAPI void transform_scale(Transform* t, vec3s scale);
MAPI mat4s transform_set_local_internal(Transform* t);

//

mat4s transform_get_local(Transform* t);

mat4s transform_get_world(Transform* t);


void transform_mark_dirty(Transform* t);


#endif //TRANSFORMS_H
