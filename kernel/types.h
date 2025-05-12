#ifndef KERNEL_TYPES_H_
#define KERNEL_TYPES_H_

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef u64 size_t;
typedef u64 uintptr_t;

#define HOLLOW_ARRAY_COUNT(arr) (sizeof(arr) / sizeof(*arr))

#endif

