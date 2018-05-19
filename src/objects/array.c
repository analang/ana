#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <ana.h>

COMO_OBJECT_API ana_object *ana_array_new(ana_size_t capacity)
{
  if(capacity == 0)
    capacity = 2;

  ana_array *obj = malloc(sizeof(*obj));

  obj->base.type = &ana_array_type;
  obj->base.next = NULL;  
  obj->base.flags = 1;
  obj->base.scope = NULL;
  
  obj->size = 0;
  obj->capacity = capacity;
  obj->items = malloc(sizeof(ana_object *) * capacity);

  return (ana_object *)obj;
}

COMO_OBJECT_API ana_object *ana_array_push(ana_object *xself, 
    ana_object *value)
{
  ana_array *self = ana_get_array(xself);

  if(self->size >= self->capacity) {
    ana_size_t newcap = self->capacity * 2;
    self->items = realloc(self->items, sizeof(ana_object *) * newcap);
    assert(self->items);
    self->capacity = newcap;
  }
  
  self->items[self->size++] = value;

  return value;
}

COMO_OBJECT_API ana_object *ana_array_get(ana_object *xself,
    ana_size_t index)
{
  assert(xself);
  ana_array *self = ana_get_array(xself);
  ana_object *retval = NULL;

  if(index >= 0 && index < self->size) 
  {
    return self->items[index];
  }

  return retval;
}

/* We can use this as a sparse array, this is primary used for a stack */
COMO_OBJECT_API ana_object *ana_array_push_index(ana_object *xself,
  ana_size_t i, 
  ana_object *value)
{ 
  ana_get_array(xself)->items[i] = value;
  
  return value;
}

static inline void array_dtor(ana_object *xself)
{ 
  if(xself->scope)
    ana_object_dtor(xself->scope);
  
  free(((ana_array *)xself)->items);
  free(xself);
}

static void array_print(ana_object *ob)
{
  fputc('[', stdout);
  ana_array_foreach(ob, index, v) 
  {
    ana_tostring_fast(v, {
      if(index + 1 != array->size)
      {
        printf("%s, ", value);
      }
      else
      {
        printf("%s", value);
      }
    });
  }  
  ana_array_foreach_end();

  fputc(']', stdout);
}
  
static ana_object *array_get(ana_object *obj, ana_object *index)
{  
  ana_array *self = ana_get_array(obj);
  
  if(ana_type_is(index, ana_long_type))
  {
    long value = ana_get_long(index)->value;

    if(value >= 0 && value < self->size)
      return self->items[value];
  }

  return NULL;
}

static ana_object *array_set(ana_object *obj, ana_object *index, 
  ana_object *val)
{
  ana_array *self = ana_get_array(obj);
  
  if(ana_type_is(index, ana_long_type))
  {
    long value = ana_get_long(index)->value;

    if(value >= 0 && value < self->size)
      return self->items[value] = val;
  }

  return NULL;
}


static struct _ana_seq_ops seqops = {
  array_get,
  array_set
};

static ana_object *array_string(ana_object *obj)
{
  ana_array *self = ana_get_array(obj);
  char *buffer = NULL;
  int size = 0;
  size =  snprintf(buffer, size, "<array at %p>", (void *)self);

  size++;

  buffer = malloc(size);

  snprintf(buffer, size, "<array at %p>", (void *)self);

  buffer[size - 1] = '\0';

  ana_object *retval = ana_stringfromstring(buffer);
  
  free(buffer);

  return retval;
}

ana_type ana_array_type = {
  .obj_name    = "array",
  .obj_print   = array_print,
  .obj_dtor    = array_dtor,
  .obj_equals  = NULL,
  .obj_bool    = NULL,
  .obj_hash    = NULL,
  .obj_str     = array_string,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = NULL,
  .obj_seqops  = &seqops
};
