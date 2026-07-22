#include "serialization/UUID.h"

void madness_uuid_generate(MADNESS_UUID* uuid)
{
    platform_generate_uuid(&uuid->high, &uuid->low);
}

MADNESS_UUID madness_uuid_generate_return()
{
    MADNESS_UUID uuid= {0};
    platform_generate_uuid(&uuid.high, &uuid.low);
    return uuid;
}

u64 madness_uuid_hash(MADNESS_UUID* uuid)
{
    return generate_hash_key_64bit((u8*)uuid, sizeof(MADNESS_UUID));
}


