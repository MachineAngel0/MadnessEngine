#include "file_serialization.h"

#include <stdio.h>
#include <string.h>

#include "unit_test.h"

byte_buffer* byte_buffer_init(uint32_t capacity)
{
    byte_buffer* buffer = (byte_buffer *) malloc(sizeof(byte_buffer));
    buffer->data = (uint8_t *) malloc(capacity);
    buffer->write_offset = 0;
    buffer->read_offset = 0;
    return buffer;
}


byte_buffer* byte_buffer_init_arena(Arena* arena, const uint32_t capacity)
{
    //the arena should be empty
    byte_buffer* buffer = (byte_buffer *) arena_alloc(arena, sizeof(byte_buffer));
    buffer->data = arena_alloc(arena, capacity);
    buffer->write_offset = 0;
    buffer->read_offset = 0;
    buffer->Arena = arena;
    return buffer;
}


// void byte_buffer_free(byte_buffer* buffer)
// {
//     free(buffer->data);
//     free(buffer);
// }


void byte_buffer_free_arena(byte_buffer* buffer)
{
    free(buffer->Arena);
}



void byte_buffer_print_info(const byte_buffer* buffer)
{
    printf("WRITE: %lu, READ: %lu\n", buffer->write_offset, buffer->write_offset);
}

/***SERIALIZE***/


void serialize_u8(byte_buffer* buffer, const uint8_t val)
{
    memcpy(buffer->data + buffer->write_offset, &val, sizeof(uint8_t));
    buffer->write_offset += sizeof(uint8_t);
}

void serialize_u16(byte_buffer* buffer, uint16_t val)
{
    memcpy(buffer->data + buffer->write_offset, &val, sizeof(uint16_t));
    buffer->write_offset += sizeof(uint16_t);
}

void serialize_u32(byte_buffer* buffer, uint32_t val)
{
    memcpy(buffer->data + buffer->write_offset, &val, sizeof(uint32_t));
    buffer->write_offset += sizeof(uint32_t);
}

void serialize_u64(byte_buffer* buffer, uint64_t val)
{
    memcpy(buffer->data + buffer->write_offset, &val, sizeof(uint64_t));
    buffer->write_offset += sizeof(uint64_t);
}

void serialize_i8(byte_buffer* buffer, int8_t val)
{
    memcpy(buffer->data + buffer->write_offset, &val, sizeof(int8_t));
    buffer->write_offset += sizeof(uint8_t);
}

void serialize_i16(byte_buffer* buffer, int16_t val)
{
    memcpy(buffer->data + buffer->write_offset, &val, sizeof(int16_t));
    buffer->write_offset += sizeof(int16_t);
}

void serialize_i32(byte_buffer* buffer, int32_t val)
{
    memcpy(buffer->data + buffer->write_offset, &val, sizeof(int32_t));
    buffer->write_offset += sizeof(int32_t);
}

void serialize_i64(byte_buffer* buffer, int64_t val)
{
    memcpy(buffer->data + buffer->write_offset, &val, sizeof(int64_t));
    buffer->write_offset += sizeof(int64_t);
}

void serialize_f32(byte_buffer* buffer, float val)
{
    memcpy(buffer->data + buffer->write_offset, &val, sizeof(float));
    buffer->write_offset += sizeof(float);
}

void serialize_f64(byte_buffer* buffer, double val)
{
    memcpy(buffer->data + buffer->write_offset, &val, sizeof(double));
    buffer->write_offset += sizeof(double);
}

/***DESERIALIZE***/

uint8_t deserialize_u8(byte_buffer* buffer)
{
    uint8_t val;
    memcpy(&val, buffer->data + buffer->read_offset, sizeof(uint8_t));
    buffer->read_offset += sizeof(uint8_t);
    return val;
}

uint16_t deserialize_u16(byte_buffer* buffer)
{
    uint16_t val;
    memcpy(&val, buffer->data + buffer->read_offset, sizeof(uint16_t));
    buffer->read_offset += sizeof(uint16_t);
    return val;
}

uint32_t deserialize_u32(byte_buffer* buffer)
{
    uint32_t val;
    memcpy(&val, buffer->data + buffer->read_offset, sizeof(uint32_t));
    buffer->read_offset += sizeof(uint32_t);
    return val;
}

uint64_t deserialize_u64(byte_buffer* buffer)
{
    uint64_t val;
    memcpy(&val, buffer->data + buffer->read_offset, sizeof(uint64_t));
    buffer->read_offset += sizeof(uint64_t);
    return val;
}

int8_t deserialize_i8(byte_buffer* buffer)
{
    int8_t val;
    memcpy(&val, buffer->data + buffer->read_offset, sizeof(int8_t));
    buffer->read_offset += sizeof(int8_t);
    return val;
}


int16_t deserialize_i16(byte_buffer* buffer)
{
    int16_t val;
    memcpy(&val, buffer->data + buffer->read_offset, sizeof(uint16_t));
    buffer->read_offset += sizeof(int16_t);
    return val;
}

int32_t deserialize_i32(byte_buffer* buffer)
{
    int32_t val;
    memcpy(&val, buffer->data + buffer->read_offset, sizeof(int32_t));
    buffer->read_offset += sizeof(int32_t);
    return val;
}

int64_t deserialize_i64(byte_buffer* buffer)
{
    int64_t val;
    memcpy(&val, buffer->data + buffer->read_offset, sizeof(int64_t));
    buffer->read_offset += sizeof(int64_t);
    return val;
}

float deserialize_f32(byte_buffer* buffer)
{
    float val;
    memcpy(&val, buffer->data + buffer->read_offset, sizeof(float));
    buffer->read_offset += sizeof(float);
    return val;
}

double deserialize_f64(byte_buffer* buffer)
{
    double val;
    memcpy(&val, buffer->data + buffer->read_offset, sizeof(double));
    buffer->read_offset += sizeof(double);
    return val;
}

void serialization_test(void)
{
    byte_buffer* buffer = byte_buffer_init(10000);

    const uint8_t a = 1;
    const uint16_t b = 10;
    const uint32_t c = 100;
    const uint64_t d = 1000;

    const int8_t e = 1;
    const int16_t f = 10;
    const int32_t g = 100;
    const int64_t h = 1000;

    const int8_t i = -1;
    const int16_t j = -10;
    const int32_t k = -100;
    const int64_t l = -1000;

    const float m = 10000.0f;
    const double n = 10000.0f;

    const float o = -10000.0f;
    const double p = -10000.0f;

    serialize_u8(buffer, a);
    serialize_u16(buffer, b);
    serialize_u32(buffer, c);
    serialize_u64(buffer, d);

    const uint8_t A = deserialize_u8(buffer);
    const uint16_t B = deserialize_u16(buffer);
    const uint32_t C = deserialize_u32(buffer);
    const uint64_t D = deserialize_u64(buffer);


    serialize_i8(buffer, e);
    serialize_i16(buffer, f);
    serialize_i32(buffer, g);
    serialize_i64(buffer, h);
    const int8_t E = deserialize_i8(buffer);
    const int16_t F = deserialize_i16(buffer);
    const int32_t G = deserialize_i32(buffer);
    const int64_t H = deserialize_i64(buffer);

    serialize_i8(buffer, i);
    serialize_i16(buffer, j);
    serialize_i32(buffer, k);
    serialize_i64(buffer, l);
    const int8_t I = deserialize_i8(buffer);
    const int16_t J = deserialize_i16(buffer);
    const int32_t K = deserialize_i32(buffer);
    const int64_t L = deserialize_i64(buffer);


    serialize_f32(buffer, m);
    serialize_f64(buffer, n);
    const float M = deserialize_f32(buffer);
    const double N = deserialize_f64(buffer);

    serialize_f32(buffer, o);
    serialize_f64(buffer, p);
    const float O = deserialize_f32(buffer);
    const double P = deserialize_f64(buffer);

    TEST_DEBUG(a == A);
    TEST_DEBUG(b == B);
    TEST_DEBUG(c == C);
    TEST_DEBUG(d == D);
    TEST_DEBUG(e == E);
    TEST_DEBUG(f == F);
    TEST_DEBUG(g == G);
    TEST_DEBUG(h == H);
    TEST_DEBUG(i == I);
    TEST_DEBUG(j == J);
    TEST_DEBUG(k == K);
    TEST_DEBUG(l == L);
    TEST_DEBUG(m == M);
    TEST_DEBUG(n == N);
    TEST_DEBUG(EQUAL_F(o, O));
    TEST_DEBUG(EQUAL_D(p,  P));


    TEST_REPORT("FILE SERIALIZATION");

}
