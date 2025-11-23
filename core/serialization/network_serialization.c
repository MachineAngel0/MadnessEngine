#include "network_serialization.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../math_lib.h"
#include "../unit_test.h"




void pack_i8(uint8_t* buf, uint32_t* offset, uint8_t val)
{
    // *(buf)++ = i; // same as buf[0] = i, *buf +=1
    buf[*offset] = val;
    *offset += sizeof(uint8_t);
}

void pack_i16(uint8_t* buf, uint32_t* offset, uint16_t val)
{
    *(buf + (*offset)) = val >> 8;
    *(buf + (*offset +1 )) = val;
    *offset += sizeof(uint16_t);
}

void pack_i32(uint8_t* buf, uint32_t* offset, uint32_t val)
{
    *(buf + (*offset)) = val >> 24;
    *(buf + (*offset) + 1) = val >> 16;
    *(buf + (*offset) + 2) = val >> 8;
    *(buf + (*offset) + 3) = val;

    /*
    printf("packi32: writing 0x%08x at offset %u -> bytes: %02x %02x %02x %02x\n",
           val, *offset, buf[*offset], buf[*offset + 1], buf[*offset + 2], buf[*offset + 3]);
*/

    *offset += sizeof(uint32_t);
}

void pack_i64(uint8_t* buf, uint32_t* offset, uint64_t val)
{
    *(buf + (*offset)) = val >> 56;
    *(buf + (*offset) + 1) = val >> 48;
    *(buf + (*offset) + 2) = val >> 40;
    *(buf + (*offset) + 3) = val >> 32;
    *(buf + (*offset) + 4) = val >> 24;
    *(buf + (*offset) + 5) = val >> 16;
    *(buf + (*offset) + 6) = val >> 8;
    *(buf + (*offset) + 7) = val;
    *offset += sizeof(uint64_t);
}

void pack_u8(uint8_t* buf, uint32_t* offset, uint8_t val)
{
    pack_i8(buf, offset, val);
}

void pack_u16(uint8_t* buf, uint32_t* offset, uint16_t val)
{
    pack_i16(buf, offset, val);
}

void pack_u32(uint8_t* buf, uint32_t* offset, uint32_t val)
{
    pack_i32(buf, offset, val);
}

void pack_u64(uint8_t* buf, uint32_t* offset, uint64_t val)
{
    pack_i64(buf, offset, val);
}


int8_t unpack_i8(uint8_t* buf, uint32_t* offset)
{
    int8_t val = buf[*offset];
    *offset += sizeof(uint8_t);

    return (int8_t) val;
}

int16_t unpack_i16(uint8_t* buf, uint32_t* offset)
{
    uint16_t val = ((uint16_t) buf[*offset] << 8) | buf[*offset + 1];
    *offset += sizeof(uint16_t);

    return (int16_t) val;
}

int32_t unpack_i32(uint8_t* buf, uint32_t* offset)
{
    uint32_t val = ((uint32_t) buf[*offset] << 24) |
                   ((uint32_t) buf[(*offset + 1)] << 16) |
                   ((uint32_t) buf[(*offset + 2)] << 8) |
                   buf[(*offset + 3)];

    *offset += sizeof(uint32_t);

    return (int32_t) val;
}

int64_t unpack_i64(uint8_t* buf, uint32_t* offset)
{
    uint64_t val =
            ((uint64_t) buf[*offset] << 56) |
            ((uint64_t) buf[(*offset + 1)] << 48) |
            ((uint64_t) buf[(*offset + 2)] << 40) |
            ((uint64_t) buf[(*offset + 3)] << 32) |
            ((uint64_t) buf[(*offset + 4)] << 24) |
            ((uint64_t) buf[(*offset + 5)] << 16) |
            ((uint64_t) buf[(*offset + 6)] << 8) |
            buf[(*offset + 7)];

    *offset += sizeof(uint64_t);

    return (int64_t) val;
}


uint8_t unpack_u8(uint8_t* buf, uint32_t* offset)
{
    uint8_t val = *(buf + *offset);
    *offset += sizeof(uint8_t);

    return val;
}

uint16_t unpack_u16(uint8_t* buf, uint32_t* offset)
{
    uint16_t val = ((uint16_t) buf[*offset] << 8) | buf[*offset + 1];
    *offset += sizeof(uint16_t);

    return val;
}


uint32_t unpack_u32(uint8_t* buf, uint32_t* offset)
{
    uint32_t val = ((uint32_t) buf[*offset] << 24) |
                   ((uint32_t) buf[(*offset + 1)] << 16) |
                   ((uint32_t) buf[(*offset + 2)] << 8) |
                   buf[(*offset + 3)];

    *offset += sizeof(uint32_t);


    return val;
}

uint64_t unpack_u64(uint8_t* buf, uint32_t* offset)
{
    uint64_t val =
            ((uint64_t) buf[*offset] << 56) |
            ((uint64_t) buf[(*offset + 1)] << 48) |
            ((uint64_t) buf[(*offset + 2)] << 40) |
            ((uint64_t) buf[(*offset + 3)] << 32) |
            ((uint64_t) buf[(*offset + 4)] << 24) |
            ((uint64_t) buf[(*offset + 5)] << 16) |
            ((uint64_t) buf[(*offset + 6)] << 8) |
            buf[(*offset + 7)];


    *offset += sizeof(uint64_t);

    return val;
}

/* FLOAT */

void pack_f16(uint8_t* buf, uint32_t* offset, long double f)
{
    //convert the value, then pack into a float, same for the rest of the functions
    uint64_t fhold = pack754(f, 16, 5);
    pack_i16(buf, offset, fhold);

}
void pack_f32(uint8_t* buf, uint32_t* offset, long double f)
{

    uint64_t fhold = pack754(f, 32, 8);
    pack_i32(buf, offset, fhold);

}
void pack_f64(uint8_t* buf, uint32_t* offset, long double f)
{
    uint64_t fhold = pack754(f, 64, 11);
    pack_i64(buf, offset, fhold);
}

float unpack_f16(uint8_t* buf, uint32_t* offset)
{
    uint64_t fhold = unpack_i16(buf, offset);
    return (float)unpack754(fhold, 16, 5);
}

float unpack_f32(uint8_t* buf, uint32_t* offset)
{
    uint64_t fhold = unpack_i32(buf, offset);
    return (float)unpack754(fhold, 32, 8);
}

double unpack_f64(uint8_t* buf, uint32_t* offset)
{
    uint64_t fhold = unpack_i64(buf, offset);
    return unpack754(fhold, 64, 11);
}

uint64_t pack754(double f, uint32_t bits, uint32_t expbits)
{
    double fnorm;
    int32_t shift;
    int64_t sign, exp, significand;
    uint32_t significandbits = bits - expbits - 1; // -1 for sign bit

    if (f == 0.0) return 0; // get this special case out of the way

    // check sign and begin normalization
    if (f < 0)
    {
        sign = 1;
        fnorm = -f;
    }
    else
    {
        sign = 0;
        fnorm = f;
    }

    // get the normalized form of f and track the exponent
    shift = 0;
    while (fnorm >= 2.0)
    {
        fnorm /= 2.0;
        shift++;
    }
    while (fnorm < 1.0)
    {
        fnorm *= 2.0;
        shift--;
    }
    fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL << significandbits) + 0.5f);

    // get the biased exponent
    exp = shift + ((1 << (expbits - 1)) - 1); // shift + bias

    // return the final answer
    return (sign << (bits - 1)) | (exp << (bits - expbits - 1)) | significand;
}

double unpack754(uint64_t i, uint32_t bits, uint32_t expbits)
{
    double result;
    int64_t shift;
    uint32_t bias;
    uint32_t significandbits = bits - expbits - 1; // -1 for sign bit

    if (i == 0) return 0.0;

    // pull the significand
    result = (i & ((1LL << significandbits) - 1)); // mask
    result /= (1LL << significandbits); // convert back to float
    result += 1.0f; // add the one back on

    // deal with the exponent
    bias = (1 << (expbits - 1)) - 1;
    shift = ((i >> significandbits) & ((1LL << expbits) - 1)) - bias;
    while (shift > 0)
    {
        result *= 2.0;
        shift--;
    }
    while (shift < 0)
    {
        result /= 2.0;
        shift++;
    }

    // sign it
    result *= (i >> (bits - 1)) & 1 ? -1.0 : 1.0;


    return result;
}

uint32_t pack(uint8_t* buf, char* format, ...)
{
    va_list ap;

    int8_t c; // 8-bit
    uint8_t C;

    int16_t h; // 16-bit
    uint16_t H;

    int32_t l; // 32-bit
    uint32_t L;

    int64_t q; // 64-bit
    uint64_t Q;

    float f; // floats
    float d;
    double g;

    char *s; // strings
    uint32_t len;


    va_start(ap, format);

    uint32_t offset = 0;

    for (; *format != '\0'; format++) {
        switch (*format) {
            /* SIGNED */
            case 'c': // 8-bit
                c = (signed char) va_arg(ap, int); // promoted
                pack_i8(buf, &offset, c);
                break;
            case 'h': // 16-bit
                h = va_arg(ap, int);
                pack_i16(buf, &offset, h);
                break;
            case 'l': // 32-bit
                l = va_arg(ap, long int);
                pack_i32(buf, &offset, l);
                break;
            case 'q': // 64-bit
                q = va_arg(ap, long long int);
                pack_i64(buf, &offset, q);
                break;

            /* UNSIGNED */

            case 'C': // 8-bit unsigned
                C = (unsigned char) va_arg(ap, unsigned int); // promoted
                pack_u8(buf, &offset, C);
                break;

            case 'H': // 16-bit unsigned
                H = va_arg(ap, unsigned int);
                pack_u16(buf, &offset, H);
                break;

            case 'L': // 32-bit unsigned
                L = va_arg(ap, unsigned long int);
                pack_u32(buf, &offset, L);
                break;

            case 'Q': // 64-bit unsigned
                Q = va_arg(ap, uint64_t);
                pack_u64(buf, &offset, Q);
                break;

            /*FLOAT*/
            case 'f': // float-16
                f = (float) va_arg(ap, double); // promoted
                pack_f16(buf, &offset, f); // convert to IEEE 754
                break;

            case 'd': // float-32
                d = va_arg(ap, double);
                pack_f32(buf, &offset, d); // convert to IEEE 754
                break;

            case 'g': // float-64
                g = va_arg(ap, long double);
                pack_f64(buf, &offset, g); // convert to IEEE 754
                break;

            /* STRING
            // TODO: use my own internal str format for this
            case 's': // string
                s = va_arg(ap, char*);
                len = strlen(s);
                size += len + 2;
                packi16(buf, len);
                buf += 2;
                memcpy(buf, s, len);
                buf += len;
                break;
            */
                default:
                printf("INVALID SPECIFIER FOR PACK");
                break;
        }
    }

    va_end(ap);

    return offset;
}

void unpack(uint8_t* buf, char* format, ...)
{
    va_list ap;

    int8_t *c; // 8-bit
    uint8_t *C;

    int16_t *h; // 16-bit
    uint16_t *H;

    int32_t *l; // 32-bit
    uint32_t *L;

    int64_t *q; // 64-bit
    uint64_t *Q;

    float *f; // floats
    float *d;
    double *g;

    char *s;
    uint32_t len, maxstrlen = 0, count;

    va_start(ap, format);

    uint32_t offset = 0;

    for (; *format != '\0'; format++) {
        switch (*format) {

            /* SIGNED */

            case 'c': // 8-bit
                c = va_arg(ap, int8_t*);
                *c = unpack_i8(buf, &offset);
                break;

            case 'h': // 16-bit
                h = va_arg(ap, int16_t*);
                *h = unpack_i16(buf, &offset);
                break;

            case 'l': // 32-bit
                l = va_arg(ap, int32_t*);
                *l = unpack_i32(buf, &offset);
                break;

            case 'q': // 64-bit
                q = va_arg(ap, int64_t*);
                *q = unpack_i64(buf, &offset);
                break;

            /* UNSIGNED */

            case 'C': // 8-bit unsigned
                C = va_arg(ap, uint8_t*);
                *C = unpack_u8(buf, &offset);
                break;

            case 'H': // 16-bit unsigned
                H = va_arg(ap, uint16_t*);
                *H = unpack_u16(buf, &offset);
                break;

            case 'L': // 32-bit unsigned
                L = va_arg(ap, uint32_t*);
                *L = unpack_u32(buf, &offset);
                break;

            case 'Q': // 64-bit unsigned
                Q = va_arg(ap, uint64_t*);
                *Q = unpack_u64(buf, &offset);
                break;

            /* FLOAT */
            case 'f': // float
                f = va_arg(ap, float*);
                *f = unpack_f16(buf, &offset);
                break;

            case 'd': // float-32
                d = va_arg(ap, float*);
                *d = unpack_f32(buf, &offset);
                break;

            case 'g': // float-64
                g = va_arg(ap, double*);
                *g = unpack_f64(buf, &offset);
                break;

            /* TODO: see comment in pack()
            case 's': // string
                s = va_arg(ap, char*);
                len = unpacku16(buf);
                buf += 2;
                if (maxstrlen > 0 && len > maxstrlen)
                    count = maxstrlen - 1;
                else
                    count = len;
                memcpy(s, buf, count);
                s[count] = '\0';
                buf += len;
                break;
            */

            default:
                if (isdigit(*format)) {
                    // track max str len
                    maxstrlen = maxstrlen * 10 + (*format - '0');
                }
        }

        if (!isdigit(*format)) maxstrlen = 0;
    }

    va_end(ap);
}

uint32_t get_size(char* format)
{
    uint32_t size = 0;
    for (; *format != '\0'; format++) {
        switch (*format)
        {
               case 'c': // 8-bit
                size += sizeof(int8_t);
                break;

            case 'h': // 16-bit
                size += sizeof(int16_t);
                break;

            case 'l': // 32-bit
                size += sizeof(int32_t);
                break;

            case 'q': // 64-bit
                size += sizeof(int64_t);
                break;

            /* UNSIGNED */

            case 'C': // 8-bit unsigned
                size += sizeof(uint8_t);
                break;

            case 'H': // 16-bit unsigned
                size += sizeof(uint16_t);
                break;

            case 'L': // 32-bit unsigned
                size += sizeof(uint32_t);
                break;

            case 'Q': // 64-bit unsigned
                size += sizeof(uint64_t);

                break;

            /* FLOAT */
            case 'f': // float
                size += sizeof(float);
                break;

            case 'd': // float-32
                size += sizeof(float);
                break;

            case 'g': // float-64
                size += sizeof(double);
                break;

            /* TODO: see comment in pack()
            case 's': // string
                s = va_arg(ap, char*);
                len = unpacku16(buf);
                buf += 2;
                if (maxstrlen > 0 && len > maxstrlen)
                    count = maxstrlen - 1;
                else
                    count = len;
                memcpy(s, buf, count);
                s[count] = '\0';
                buf += len;
                break;
            */
        }}

    return size;

}


void pack_unpack_test()
{

    bool serilization_passing = true;

    uint8_t* buf = malloc(sizeof(uint8_t) * 1024); // should be more than enough memory in bytes
    uint32_t offset = 0;


    pack_i8(buf, &offset, -8);
    pack_i16(buf, &offset, -16);
    pack_i32(buf, &offset, -32);
    pack_i64(buf, &offset, -64);

    pack_u8(buf, &offset, 8);
    pack_u16(buf, &offset, 16);
    pack_u32(buf, &offset, 32);
    pack_u64(buf, &offset, 64);

    //reset our offset, so that we can unpack properly
    offset = 0;
    int8_t i8 = unpack_i8(buf, &offset);
    TEST_DEBUG(i8 == -8);

    int16_t i16 = unpack_i16(buf, &offset);
    TEST_DEBUG(i16 == -16);

    int32_t i32 = unpack_i32(buf, &offset);
    TEST_DEBUG(i32 == -32);

    int64_t i64 = unpack_i64(buf, &offset);
    TEST_DEBUG(i64 == -64);

    uint8_t u8 = unpack_u8(buf, &offset);
    TEST_DEBUG(u8 == 8);

    uint16_t u16 = unpack_u16(buf, &offset);
    TEST_DEBUG(u16 == 16);

    uint32_t u32 = unpack_u32(buf, &offset);
    TEST_DEBUG(u32 == 32);


    uint64_t u64 = unpack_u64(buf, &offset);
    TEST_DEBUG(u64 == 64);

    buf = memset(buf, 0, sizeof(uint8_t));
    offset = 0;
    float f16_test = 16.161616f;
    float f32_test = 32.323232f;
    double f64_test = 64.646464f;

    float f16_test_n = -16.161616f;
    float f32_test_n = -32.323232f;
    double f64_test_n = -64.646464f;

    pack_f16(buf, &offset, f16_test);
    pack_f32(buf, &offset, f32_test);
    pack_f64(buf, &offset, f64_test);

    pack_f16(buf, &offset, f16_test_n);
    pack_f32(buf, &offset, f32_test_n);
    pack_f64(buf, &offset, f64_test_n);



    offset = 0;
    float f16_out = unpack_f16(buf, &offset);
    float f32_out = unpack_f32(buf, &offset);
    double f64_out = unpack_f64(buf, &offset);

    float f16_out_n = unpack_f16(buf, &offset);
    float f32_out_n = unpack_f32(buf, &offset);
    double f64_out_n = unpack_f64(buf, &offset);


    //these will fail due is using the macro, the pack and unpack functions might be wrong
    //to some conversion wierdness
    TEST_DEBUG(equal_f(f16_out, f16_test, 0.001));
    TEST_DEBUG(equal_f(f32_out, f32_test, 0.001));
    TEST_DEBUG(equal_f(f64_out, f64_test, 0.001));
    TEST_DEBUG(equal_f(f16_out, f16_test, 0.001));
    TEST_DEBUG(equal_f(f16_out, f16_test, 0.001));

    // printf("%f\n", f16_out);
    // printf("%f\n", f32_out);
    // printf("%f\n", f64_out);

    // printf("%f\n", f16_out_n);
    // printf("%f\n", f32_out_n);
    // printf("%f\n", f64_out_n);

    buf = memset(buf, 0, sizeof(uint8_t));

    int8_t magic;
    int16_t monkeycount;
    int32_t altitude;
    float absurdityfactor;
    char s2[96];
    int16_t  ps2;
    uint32_t packet_size = pack(buf, "chhld", (int8_t) 10, (int16_t) 0,
                                (int16_t) 37, (int32_t) -5, (float) -3490.6677f);
    unpack(buf, "chhl96d", &magic, &ps2, &monkeycount, &altitude,
                &absurdityfactor);

    // printf("%d, %d, %d, %d, %f\n", magic, ps2, monkeycount, altitude, absurdityfactor);

    TEST_REPORT("NETWORK SERIALIZATION");
}
