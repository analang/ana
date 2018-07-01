#ifndef COMO_LONG_OBJECT
#define COMO_LONG_OBJECT

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include long.h directly"
#endif

typedef struct _ana_long {
  ana_object base;
  long value;
} ana_long;

COMO_OBJECT_API ana_object *ana_longfromlong(long lval);

COMO_OBJECT_API void ana_long_type_init(ana_vm *vm);
COMO_OBJECT_API void ana_long_type_finalize(ana_vm *vm);

#define ana_get_long(o) ((ana_long *)(o))

extern ana_type ana_long_type;

#endif
