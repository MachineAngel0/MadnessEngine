#ifndef MADNESSENGINE_UUID_H
#define MADNESSENGINE_UUID_H


#include "defines.h"
#include "platform.h"


typedef struct MADNESS_UUID
{
    u64 high;
    u64 low;
} MADNESS_UUID;


void madness_uuid_generate(MADNESS_UUID* uuid)
{
    platform_generate_uuid(&uuid->high, &uuid->low);
}


#endif //MADNESSENGINE_UUID_H
