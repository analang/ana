#ifndef COMO_STRING_H
#define COMO_STRING_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include anastring.h directly"
#endif

typedef struct _ana_string {
  ana_object base;
  ana_size_t len;
  ana_usize_t hash;
  char *value;
} ana_string;

typedef struct _ana_string_iterator ana_string_iterator;

struct _ana_string_iterator {
  ana_object base;
  ana_string *string;
  ana_size_t position;
};

COMO_OBJECT_API ana_object *ana_stringfromstring(char *val);
COMO_OBJECT_API void ana_string_type_init(ana_vm *vm);
COMO_OBJECT_API void ana_string_type_finalize(ana_vm *vm);

#define ana_get_string(o) ((ana_string *)(o))
#define ana_cstring(o) (((ana_string *)(o))->value)

extern ana_type ana_string_type;
extern ana_type ana_string_iterator_type;

#endif
