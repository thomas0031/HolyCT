#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

// primitive types
typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
// TODO i128, u128
typedef float f32;
typedef double f64;

// pointer types
typedef char* str_t;

#define True ((void *)1)
#define False ((void *)0)

#endif // !TYPES_H
