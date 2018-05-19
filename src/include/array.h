#ifndef COMO_ARRAY_H
#define COMO_ARRAY_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include array.h directly"
#endif

/* Inheritence:
 *  ana_object
 *      ana_container
 *          ana_array
 */
typedef struct _ana_array 
{
  ana_object base;
  ana_size_t size;
  ana_size_t capacity;
  ana_object **items;
} ana_array;

COMO_OBJECT_API ana_object *ana_array_new(ana_size_t);
COMO_OBJECT_API ana_object *ana_array_push(ana_object *, ana_object *);

COMO_OBJECT_API ana_object *ana_array_push_index(ana_object *, 
  ana_size_t i, ana_object *);

COMO_OBJECT_API ana_object *ana_array_get(ana_object *, ana_size_t i);

#define ana_get_array(o) ((ana_array *)(o))
#define ana_array_size(o) (ana_get_array((o))->size)

#define ana_array_foreach(_array, indexname, valuename) { \
  ana_size_t indexname; \
  ana_object *valuename; \
  do { \
    ana_size_t _i; \
    ana_array *array = ana_get_array(_array); \
    for(_i = 0; _i < array->size; _i++) { \
      indexname = _i; \
      valuename = array->items[_i]; \


#define ana_array_foreach_end() \
    } \
  } while(0); \
} \

COMO_OBJECT_API void ana__init_array_type(void);
COMO_OBJECT_API void ana__finalize_array_type(void);

extern ana_type ana_array_type;

#endif
