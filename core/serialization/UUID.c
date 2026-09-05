#include "serialization/UUID.h"

void madness_uuid_generate(MADNESS_UUID* uuid)
{
    platform_generate_uuid(&uuid->high, &uuid->low);
}

MADNESS_UUID madness_uuid_generate_return(void)
{
    MADNESS_UUID uuid = {0};
    platform_generate_uuid(&uuid.high, &uuid.low);
    return uuid;
}

u64 madness_uuid_hash(MADNESS_UUID* uuid)
{
    return generate_hash_key_64bit((u8*)uuid, sizeof(MADNESS_UUID));
}

bool madness_uuid_compare(MADNESS_UUID uuid1, MADNESS_UUID uuid2)
{
    return uuid1.high == uuid2.high && uuid1.low == uuid2.low;
}

bool madness_uuid_serialize(MADNESS_UUID uuid, FILE* fptr)
{
    fwrite(&uuid, sizeof(MADNESS_UUID), 1, fptr);
    return true;
}

bool madness_uuid_deserialize(MADNESS_UUID* uuid, FILE* fptr)
{
    fread(uuid, sizeof(MADNESS_UUID), 1, fptr);
    return true;
}

void madness_uuid_validate(MADNESS_UUID uuid)
{
    MASSERT(uuid.high != 0)
    MASSERT(uuid.low != 0)
}

