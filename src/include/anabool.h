#ifndef COMO_BOOL_OBJECT
#define COMO_BOOL_OBJECT

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include bool.h directly"
#endif

typedef struct _ana_bool {
  ana_object base;
  int value;
} ana_bool;

extern ana_object *ana_bool_true;
extern ana_object *ana_bool_false;

COMO_OBJECT_API void ana_bool_type_init();
COMO_OBJECT_API void ana_bool_type_finalize();

#define ana_boolfromint(value) \
  ((value) == 1 ? ana_bool_true : ana_bool_false)

COMO_OBJECT_API ana_object *ana_boolfromanaobject(ana_object *);

#define ana_get_bool(o) ((ana_bool *)((o)))
#define ana_false ana_boolfromint(0)
#define ana_true ana_boolfromint(1)

extern ana_type ana_bool_type;

#endif