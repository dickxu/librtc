#ifndef _UBASE_TYPES_H_
#define _UBASE_TYPES_H_

#include <stdlib.h>

// basic data types
#ifdef WIN32
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed int          int32_t;
typedef unsigned int        uint32_t;
typedef signed __int64      int64_t;
typedef unsigned __int64    uint64_t;
typedef long                intptr_t;
typedef unsigned long       uintptr_t;
#else
#include <stdint.h>
#endif

typedef void *              voidptr_t;
typedef void *              handle_t;

#endif
