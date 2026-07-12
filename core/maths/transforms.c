#include "transforms.h"

#include "math_lib.h"


//create operations
Transform* transform_from_position_rotation_scale(const mvec3 position, const mquat rotation, const mvec3 scale,
                                                  Allocator* arena)
{
    Transform* out_transform = allocator_alloc(arena, sizeof(Transform));
    out_transform->position = position;
    out_transform->rotation = rotation;
    out_transform->scale = scale;
    return out_transform;
};

Transform* transform_create(Allocator* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(vec3_zero(), quat_identity(), vec3_one(), arena);
    return out_transform;
}

void transform_set_default(Transform* transform)
{
    *transform = (Transform){vec3_zero(), quat_identity(), vec3_one()};
    transform_get_local_internal(transform);

    transform->is_dirty = true;
}

Transform* transform_from_position(const mvec3 position, Allocator* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(position, quat_identity(), vec3_one(), arena);
    return out_transform;
}

Transform* transform_from_rotation(const mquat rotation, Allocator* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(vec3_zero(), rotation, vec3_one(), arena);
    return out_transform;
}


Transform* transform_from_scale(const mvec3 scale, Allocator* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(vec3_zero(), quat_identity(), scale, arena);
    return out_transform;
}


//set operations
void transform_set_position(Transform* t, const mvec3 position)
{
    t->position = position;
    t->is_dirty = true;
};

void transform_set_rotation(Transform* t, const mquat rotation)
{
    t->rotation = rotation;
    t->is_dirty = true;
};

void transform_set_scale(Transform* t, const mvec3 scale)
{
    t->scale = scale;
    t->is_dirty = true;
};

void transform_set_parent(Transform* t, Transform* parent)
{
    t->parent = parent;
};


//perform operation
void transform_translate(Transform* t, const mvec3 position)
{
    t->position = vec3_add(t->position, position);
    t->is_dirty = true;
}

void transform_rotate(Transform* t, const mquat rotation)
{
    t->rotation = quat_mul(t->rotation, rotation);
    t->is_dirty = true;
}

void transform_scale(Transform* t, const mvec3 scale)
{
    t->scale = vec3_mul(t->scale, scale);
    t->is_dirty = true;
}

mmat4 transform_get_local_internal(Transform* t)
{
    mmat4 tr = mat4_mul(quat_to_mat4(t->rotation), mat4_translation(t->position));
    tr = mat4_mul(mat4_scale(t->scale), tr);
    t->local = tr;
    t->is_dirty = false;
    return t->local;
}

mmat4 transform_get_local(Transform* t)
{
    if (t->is_dirty)
    {
        mmat4 tr = mat4_mul(quat_to_mat4(t->rotation), mat4_translation(t->position));
        tr = mat4_mul(mat4_scale(t->scale), tr);
        t->local = tr;
        t->is_dirty = false;
        return t->local;
    }
    return t->local;

}

mmat4 transform_get_world(Transform* t)
{
    mmat4 l = transform_get_local(t);
    if (t->parent)
    {
        mmat4 p = transform_get_world(t->parent);
        return mat4_mul(l, p);
    }
    return l;
}

void transform_mark_dirty(Transform* t)
{
    t->is_dirty = true;
}
