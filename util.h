#ifndef UTIL_H
#define UTIL_H

#include <cinttypes>
#include <cmath>

using namespace std;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef uint8_t byte;

static byte rev_lut[16] = {0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE, 0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF};

#define reverse_byte(b) (rev_lut[b&0x0F]<<4 | rev_lut[b>>4])

template<class T>
T distance(T x1, T y1, T x2, T y2) {
    T dx = x1-x2;
    T dy = y1-y2;
    return (T)sqrt(dx*dx + dy*dy);
}

#endif // UTIL_H
