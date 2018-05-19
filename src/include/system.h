#ifndef COMO_SYSTEM_H
#define COMO_SYSTEM_H

typedef unsigned int ana_uint32_t;
typedef unsigned long ana_usize_t;
typedef int ana_int;
typedef char* ana_exception;

#define COMO_UNUSED(x) ((void)x)
#define COMO_PTR(x) ((uintptr_t)((x)))

#define COMO_PTR_FMT "0x%X"
#define COMO_LNG_FMT "%ld"

#endif
