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


