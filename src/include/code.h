#ifndef COMO_CODE_OBJECT_H
#define COMO_CODE_OBJECT_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include code.h directly"
#endif

typedef struct _ana_code 
{
  ana_container base;
  ana_uint32_t **code;
} ana_code;

COMO_OBJECT_API ana_object  *ana_code_new(ana_size_t cap);
COMO_OBJECT_API ana_object  *ana_code_push(ana_object *, ana_uint32_t);
COMO_OBJECT_API ana_uint32_t ana_code_get(ana_object *, ana_size_t);

#define ana_get_code(x) ((ana_code *)(x))

extern ana_type ana_code_type;

#endif
