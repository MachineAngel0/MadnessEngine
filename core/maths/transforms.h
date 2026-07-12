#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include "math_types.h"


typedef struct Transform
{
    mvec3 position; //literally no need to create a vec2 version of this
    mquat rotation;
    mvec3 scale;
    mmat4 local;
    bool is_dirty;
    struct Transform* parent;
} Transform;

typedef struct Transform_SOA
{
    mvec3* position;
    mquat* rotation;
    mvec3* scale;
    mmat4* local;
}Transform_SOA;


//create operations
MAPI Transform* transform_from_position_rotation_scale(const mvec3 position, const mquat rotation, const mvec3 scale,
                                                       Allocator* arena);
MAPI Transform* transform_create(Allocator* arena);
void transform_set_default(Transform* transform);

MAPI Transform* transform_from_position(const mvec3 position, Allocator* arena);
MAPI Transform* transform_from_rotation(const mquat rotation, Allocator* arena);

MAPI Transform* transform_from_scale(const mvec3 scale, Allocator* arena);


//set operations
MAPI void transform_set_position(Transform* t, const mvec3 position);

MAPI void transform_set_rotation(Transform* t, const mquat rotation);
MAPI void transform_set_scale(Transform* t, const mvec3 scale);
MAPI void transform_set_parent(Transform* t, Transform* parent);

//perform operation
MAPI void transform_translate(Transform* t, const mvec3 position);
MAPI void transform_rotate(Transform* t, const mquat rotation);
MAPI void transform_scale(Transform* t, const mvec3 scale);
MAPI mmat4 transform_get_local_internal(Transform* t);

//

mmat4 transform_get_local(Transform* t);

mmat4 transform_get_world(Transform* t);


void transform_mark_dirty(Transform* t);


#endif //TRANSFORMS_H
