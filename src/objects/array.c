#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <ana.h>

COMO_OBJECT_API void ana_array_type_init(ana_vm *vm)
{
  ana_type *type = &ana_array_type;

  assert(type->obj_props == NULL);

  type->obj_props = ana_map_new(4);

  ana_object *func = ana_methodfromhandler("<builtin>", "push", ana_array_push);

  ana_map_put(type->obj_props, ana_vm_new_symbol(vm, "push"), func);
} 

COMO_OBJECT_API void ana_array_type_finalize(ana_vm *vm)
{
  COMO_UNUSED(vm);

  ana_map_foreach(ana_array_type.obj_props, key, value) {
    (void)key;

    ana_object_dtor(value);
  } ana_map_foreach_end();

  ana_object_dtor(ana_array_type.obj_props);
}

static inline void array_dtor(ana_object *xself)
{ 
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
        if(ana_type_is(v, ana_string_type)) 
        {
          printf("'%s', ", value);
        }
        else
        {
          printf("%s, ", value); 
        }
      }
      else
      {
        if(ana_type_is(v, ana_string_type)) 
        {
          printf("'%s'", value);
        }
        else
        {
          printf("%s", value);         
        }
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

static ana_object *array_string(ana_object *obj)
{
  char *val = ana_build_str("<array at %p>", (void *)obj);

  ana_object *retval = ana_stringfromstring(val);

  free(val);

  return retval;
}

static struct _ana_seq_ops seqops = {
  array_get,
  array_set
};


COMO_OBJECT_API ana_object *ana_array_new(ana_size_t capacity)
{
  if(capacity == 0)
    capacity = 2;

  ana_array *obj = malloc(sizeof(*obj));

  obj->base.type = &ana_array_type;
  obj->base.next = NULL;  
  obj->base.flags = 0;  
  obj->base.refcount = 0;
  obj->size = 0;
  obj->capacity = capacity;
  obj->items = malloc(sizeof(ana_object *) * capacity);

  return (ana_object *)obj;
}

COMO_OBJECT_API ana_object *ana_array_push(ana_object *xself, 
    ana_object *value)
{
  ana_array *self = ana_get_array(xself);

  if(self->size >= self->capacity) 
  {
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

COMO_OBJECT_API ana_object *ana_array_push_index(ana_object *xself,
  ana_size_t i, 
  ana_object *value)
{ 
  ana_get_array(xself)->items[i] = value;
  
  return value;
}

static ana_object *array_get_attr(ana_object *obj, ana_object *keyobj)
{
  ana_array *self = ana_get_array(obj);
  char *str = ana_cstring(keyobj);

  if(strcmp(str, "length") == 0)
    return ana_longfromlong(self->size);

  return NULL;
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
  .obj_seqops  = &seqops,
  .obj_get_attr = array_get_attr,
  .obj_props   = NULL
};
