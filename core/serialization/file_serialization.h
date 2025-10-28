#ifndef FILE_SERIALIZATION_H
#define FILE_SERIALIZATION_H

#include <stdbool.h>
#include <stdint.h>


//NOTE: might want to transition it into a darray
typedef struct byte_buffer
{
    //small performance hit, but it just makes the code nicer in not having to remeber to zero offset on a read
    uint32_t write_offset;
    uint32_t read_offset;
    uint32_t capacity;
    uint8_t* data;
} byte_buffer;

byte_buffer* byte_buffer_init(uint32_t capacity);

void byte_buffer_free(byte_buffer* buffer);

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
void serialization_test();


#endif //FILE_SERIALIZATION_H
