#ifndef MADNESSENGINE_UUID_H
#define MADNESSENGINE_UUID_H

#include "platform.h"


typedef struct MADNESS_UUID
{
    u64 high;
    u64 low;
} MADNESS_UUID;


void madness_uuid_generate(MADNESS_UUID* uuid);
MADNESS_UUID madness_uuid_generate_return();
u64 madness_uuid_hash(MADNESS_UUID* uuid);
bool madness_uuid_compare(MADNESS_UUID uuid1, MADNESS_UUID uuid2);


//TODO:
//u64 madness_uuid_hash(MADNESS_UUID uuid);
#endif //MADNESSENGINE_UUID_H
