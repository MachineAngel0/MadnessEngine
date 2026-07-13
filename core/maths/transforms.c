#include "transforms.h"

#include "math_lib.h"


//create operations
Transform* transform_from_position_rotation_scale(const vec3s position, const versors rotation, const vec3s scale,
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
        transform_from_position_rotation_scale(glms_vec3_zero(), glms_quat_identity(), glms_vec3_one(), arena);
    return out_transform;
}

void transform_set_default(Transform* transform)
{
    *transform = (Transform){glms_vec3_zero(), glms_vec3_zero(), glms_quat_identity(), glms_vec3_one()};


    transform_set_local_internal(transform);

    transform->is_dirty = true;
}

Transform* transform_from_position(const vec3s position, Allocator* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(position, glms_quat_identity(), glms_vec3_one(), arena);
    return out_transform;
}

Transform* transform_from_rotation(const versors rotation, Allocator* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(glms_vec3_zero(), rotation, glms_vec3_one(), arena);
    return out_transform;
}


Transform* transform_from_scale(const vec3s scale, Allocator* arena)
{
    Transform* out_transform =
        transform_from_position_rotation_scale(glms_vec3_zero(), glms_quat_identity(), scale, arena);
    return out_transform;
}


//set operations
void transform_set_position(Transform* t, const vec3s position)
{
    t->position = position;
    t->is_dirty = true;
};

void transform_set_rotation(Transform* t, const versors rotation)
{
    t->rotation = rotation;
    t->is_dirty = true;
};

void transform_set_scale(Transform* t, const vec3s scale)
{
    t->scale = scale;
    t->is_dirty = true;
};

void transform_set_parent(Transform* t, Transform* parent)
{
    t->parent = parent;
};


//perform operation
void transform_translate(Transform* t, const vec3s position)
{
    t->position = glms_vec3_add(t->position, position);
    t->is_dirty = true;
}

void transform_rotate(Transform* t, const versors rotation)
{
    t->rotation = glms_quat_mul(t->rotation, rotation);
    t->is_dirty = true;
}

void transform_rotate_euler(Transform* t)
{
    t->rotation = glms_euler_xyz_quat(t->euler_angles);
    t->is_dirty = true;

}

void transform_scale(Transform* t, const vec3s scale)
{
    t->scale = glms_vec3_mul(t->scale, scale);
    t->is_dirty = true;
}

mat4s transform_set_local_internal(Transform* t)
{
    return glms_build_local_matrix(t->position, t->rotation, t->scale);
}

mat4s transform_get_local(Transform* t)
{
    if (t->is_dirty)
    {
        t->local =  transform_set_local_internal(t);
        t->is_dirty = false;
        return t->local;
    }
    return t->local;

}

mat4s transform_get_world(Transform* t)
{
    mat4s l = transform_get_local(t);
    if (t->parent)
    {
        mat4s p = transform_get_world(t->parent);
        return glms_mat4_mul(l, p);
    }
    return l;
}

void transform_mark_dirty(Transform* t)
{
    t->is_dirty = true;
}

