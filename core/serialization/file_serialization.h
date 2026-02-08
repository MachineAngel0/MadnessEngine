#ifndef FILE_SERIALIZATION_H
#define FILE_SERIALIZATION_H

#include <stdint.h>

#include "arena.h"

//TODO: use the allocator properly, like dynamic allocations when writing or reading
typedef struct byte_buffer
{
    //small but probably negligible performance hit, but it just makes the code nicer in not having to remember to zero offset on a read/write
    // if you want to know how big your data is, get either the write or the read offset, based on what your doing
    u64 write_offset;
    u64 read_offset;
    uint8_t* data;
    Arena* Arena; // removed capacity and this is return in favor of it
} byte_buffer;

byte_buffer* byte_buffer_init(uint32_t capacity);

// void byte_buffer_free(byte_buffer* buffer);
byte_buffer* byte_buffer_init_arena(Arena* arena, const uint32_t capacity);



void byte_buffer_print_info(const byte_buffer* buffer);


/*
 * serialize unsigned int
*/
void serialize_u8(byte_buffer* buffer, uint8_t val);

void serialize_u16(byte_buffer* buffer, uint16_t val);

void serialize_u32(byte_buffer* buffer, uint32_t val);

void serialize_u64(byte_buffer* buffer, uint64_t val);


/*
 * serialize signed int
*/
void serialize_i8(byte_buffer* buffer, int8_t val);

void serialize_i16(byte_buffer* buffer, int16_t val);

void serialize_i32(byte_buffer* buffer, int32_t val);

void serialize_i64(byte_buffer* buffer, int64_t val);

/*
 * serialize floats
*/

// void serialize_f16(byte_buffer* buffer, long double f);

void serialize_f32(byte_buffer* buffer, float val);

void serialize_f64(byte_buffer* buffer, double val);



/*
 * unpack unsigned int
*/

uint8_t deserialize_u8(byte_buffer* buffer);

uint16_t deserialize_u16(byte_buffer* buffer);

uint32_t deserialize_u32(byte_buffer* buffer);

uint64_t deserialize_u64(byte_buffer* buffer);

/*
 * unserialize signed int
*/
int8_t deserialize_i8(byte_buffer* buffer);

int16_t deserialize_i16(byte_buffer* buffer);

int32_t deserialize_i32(byte_buffer* buffer);

int64_t deserialize_i64(byte_buffer* buffer);


/*
 * deserialize floats
*/

// float deserialize_f16(byte_buffer* buffer);

float deserialize_f32(byte_buffer* buffer);

double deserialize_f64(byte_buffer* buffer);

//TODO: write this test
void serialization_test(void);


#endif //FILE_SERIALIZATION_H
