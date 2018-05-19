#include <stdlib.h>

#include <ana.h>

COMO_OBJECT_API void ana_container_init(ana_object *obj, 
    ana_size_t sizeofitem,
    ana_size_t capacity,
    void ***itemsp)
{
  /* Initialize the parent */
  obj->type = &ana_container_type;
  obj->flags = 1;
  obj->next = NULL;
  obj->scope = NULL;
 
  /* Initialize self */ 
  ana_container *self = ana_get_container(obj);
 
  self->size = 0;
  self->itemsize = sizeofitem;
  self->capacity = capacity;

  self->data = malloc(sizeofitem * capacity);

  *itemsp = self->data;
}

COMO_OBJECT_API void ana_container_resize(ana_object *obj, void ***itemsp)
{
  ana_container *self = ana_get_container(obj);
  ana_size_t newcap = self->capacity * 2;
  self->data = realloc(self->data, self->itemsize * newcap);

  *itemsp = self->data;
  self->capacity = newcap;
}

COMO_OBJECT_API void *ana_container_get(ana_object *obj, ana_size_t idx)
{
  ana_container *self = ana_get_container(obj);
  void *retval = NULL;

  if(idx >= 0 && idx < self->size) 
    retval = self->data[idx];

  return retval;
}

COMO_OBJECT_API void *ana_container_push(ana_object *obj, void *value)
{
  ana_container *self = ana_get_container(obj);
 
  self->data[self->size++] = value;

  return value;
}

static void container_dtor(ana_object *x)
{
  ana_container *self = ana_get_container(x);
    
  if(x->scope)
    ana_object_dtor(x->scope);


  free(self->data);
  
  /* This object (container)
   * wasn't allocated on the heap, it is simply a stack pointer */
  /* We should call the child dtor because it won't get called otherwise */
  /* The child class is actually the one allocated on the heap */
  self->type->obj_dtor(x); 
}

static void container_print_proxy(ana_object *x)
{
  ana_container *self = ana_get_container(x);
  self->type->obj_print(x);
}

static ana_object *str(ana_object *x)
{
  COMO_UNUSED(x);
  
  return ana_stringfromstring("container");
}

ana_type ana_container_type = {
  .obj_name    = "container",
  .obj_print   = container_print_proxy,
  .obj_dtor    = container_dtor,
  .obj_equals  = NULL,
  .obj_bool    = NULL,
  .obj_hash    = NULL,
  .obj_str     = str,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = NULL,
  .obj_seqops  = NULL
};
























