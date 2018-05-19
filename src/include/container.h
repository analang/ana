#ifndef COMO_CONTAINER_H
#define COMO_CONTAINER_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include container.h directly"
#endif

/* This is an "abstract class" that holds a sequence type. It isn't 
 * supposed to be a concrete class, rather a base class that handles
 * the ever so repetive dynamic array needs
 **/
typedef struct _ana_container
{
  ana_object base;
  ana_type  *type;
  ana_size_t itemsize;
  ana_size_t size;
  ana_size_t capacity;
  /* child classes implement the actual data type aka the T** pointer,
   * but it just points back here */
  void        **data; 
} ana_container;

#define ana_get_container(o) ((ana_container *)(o))
#define ana_container_size(o) (ana_get_container((o))->size)
#define ana_container_capacity(o) (ana_get_container(o)->capacity)

#define ana_container_should_resize(self, items) do { \
  if(ana_container_size(self) >= ana_container_capacity(self)) { \
    ana_container_resize((ana_object *)self, (void ***)&items); \
  } \
} while(0)

COMO_OBJECT_API void ana_container_init(ana_object *obj, ana_size_t size,
    ana_size_t capacity, void ***itemsp);

COMO_OBJECT_API void ana_container_resize(ana_object *obj, void ***itemsp);
COMO_OBJECT_API void *ana_container_get(ana_object *obj, ana_size_t idx);
COMO_OBJECT_API void *ana_container_push(ana_object *obj, void *value);

extern ana_type ana_container_type;

#endif
