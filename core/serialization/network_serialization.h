#ifndef NETWORK_SERIALIZATION_H
#define NETWORK_SERIALIZATION_H
#include <stdint.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>



/*
 * pack signed int-- store a signed int into a char/uint8 buffer (like htons())
*/
void pack_i8(uint8_t* buf, uint32_t* offset, uint8_t val);

void pack_i16(uint8_t* buf, uint32_t* offset, uint16_t val);

void pack_i32(uint8_t* buf, uint32_t* offset, uint32_t val);

void pack_i64(uint8_t* buf, uint32_t* offset, uint64_t val);

/*
 * pack signed int-- store a signed int into a char/uint8 buffer (like htons())
 * Internally these all act the same as their signed counterparts
*/
void pack_u8(uint8_t* buf, uint32_t* offset, uint8_t val);

void pack_u16(uint8_t* buf, uint32_t* offset, uint16_t val);

void pack_u32(uint8_t* buf, uint32_t* offset, uint32_t val);

void pack_u64(uint8_t* buf, uint32_t* offset, uint64_t val);

/*
 * unpack signed int -- pack signed int from a char/uint8 buffer (like ntohs())
*/
int8_t unpack_i8(uint8_t* buf, uint32_t* offset);

int16_t unpack_i16(uint8_t* buf, uint32_t* offset);

int32_t unpack_i32(uint8_t* buf, uint32_t* offset);

int64_t unpack_i64(uint8_t* buf, uint32_t* offset);


/*
 * unpack unsigned int -- pack signed int from a char/uint8 buffer (like ntohs())
*/

uint8_t unpack_u8(uint8_t* buf, uint32_t* offset);

uint16_t unpack_u16(uint8_t* buf, uint32_t* offset);

uint32_t unpack_u32(uint8_t* buf, uint32_t* offset);

uint64_t unpack_u64(uint8_t* buf, uint32_t* offset);


/*IEEE-754 format*/
/*
 * pack floats -- pack signed int from a char/uint8 buffer (like ntohs())
*/

void pack_f16(uint8_t* buf, uint32_t* offset, long double f);

void pack_f32(uint8_t* buf, uint32_t* offset, long double f);

void pack_f64(uint8_t* buf, uint32_t* offset, long double f);

/*
 * unpack floats -- pack signed int from a char/uint8 buffer (like ntohs())
*/

float unpack_f16(uint8_t* buf, uint32_t* offset);

float unpack_f32(uint8_t* buf, uint32_t* offset);

double unpack_f64(uint8_t* buf, uint32_t* offset);

/*
 * pack754() -- pack a floating point number into IEEE-754 format
* HELPER FUNCTION
*/
uint64_t pack754(double f, uint32_t bits, uint32_t expbits);

/*
 * unpack754() -- unpack a floating point number from IEEE-754 format
*/
double unpack754(uint64_t i, uint32_t bits, uint32_t expbits);


/*
** pack() -- store data dictated by the format string in the buffer
**
**   bits |signed   unsigned   float   string
**   -----+----------------------------------
**      8 |   c        C
**     16 |   h        H         f
**     32 |   l        L         d
**     64 |   q        Q         g
**      - |                               s
**
**  (16-bit unsigned length is automatically prepended to strings)
*/

uint32_t pack(uint8_t* buf, char* format, ...);


/*
** unpack() -- unpack data dictated by the format string into the
**             buffer
**
**   bits |signed   unsigned   float   string
**   -----+----------------------------------
**      8 |   c        C
**     16 |   h        H         f
**     32 |   l        L         d
**     64 |   q        Q         g
**      - |                               s
**
**  (string is extracted based on its stored length, but 's' can be
**  prepended with a max length)
*/

void unpack(uint8_t* buf, char* format, ...);

uint32_t get_size(char* format);


void pack_unpack_test(void);





#endif //SERIALIZATION_H
