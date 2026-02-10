#include "transforms.h"

#include "math_lib.h"


//create operations
Transform* transform_from_position_rotation_scale(const vec3 position, const quat rotation, const vec3 scale,
                                                  Arena* arena)
{
    Transform* out_transform = arena_alloc(arena, sizeof(Transform));
    out_transform->position = position;
    out_transform->rotation = rotation;
    out_transform->scale = scale;
    return out_transform;
};

Transform* transform_create(Arena* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(vec3_zero(), quat_identity(), vec3_one(), arena);
    return out_transform;
}

Transform* transform_from_position(const vec3 position, Arena* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(position, quat_identity(), vec3_one(), arena);
    return out_transform;
}

Transform* transform_from_rotation(const quat rotation, Arena* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(vec3_zero(), rotation, vec3_one(), arena);
    return out_transform;
}


Transform* transform_from_scale(const vec3 scale, Arena* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(vec3_zero(), quat_identity(), scale, arena);
    return out_transform;
}


//set operations
void transform_set_position(Transform* t, const vec3 position)
{
    t->position = position;
    t->is_dirty = true;
};

void transform_set_rotation(Transform* t, const quat rotation)
{
    t->rotation = rotation;
    t->is_dirty = true;
};

void transform_set_scale(Transform* t, const vec3 scale)
{
    t->scale = scale;
    t->is_dirty = true;
};

void transform_set_parent(Transform* t, Transform* parent)
{
    t->parent = parent;
};


//perform operation
void transform_translate(Transform* t, const vec3 position)
{
    t->position = vec3_add(t->position, position);
    t->is_dirty = true;
}

void transform_rotate(Transform* t, const quat rotation)
{
    t->rotation = quat_mul(t->rotation, rotation);
    t->is_dirty = true;
}

void transform_scale(Transform* t, const vec3 scale)
{
    t->scale = vec3_mul(t->scale, scale);
    t->is_dirty = true;
}

//

mat4 transform_get_local(Transform* t)
{
    if (t->is_dirty)
    {
        mat4 tr = mat4_mul(quat_to_mat4(t->rotation), mat4_translation(t->position));
        tr = mat4_mul(mat4_scale(t->scale), tr);
        t->local = tr;
        t->is_dirty = false;
        return t->local;
    }
    return mat4_identity();
}

mat4 transform_get_world(Transform* t)
{
    mat4 l = transform_get_local(t);
    if (t->parent)
    {
        mat4 p = transform_get_world(t->parent);
        return mat4_mul(l, p);
    }
    return l;
}
