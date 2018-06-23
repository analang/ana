#include <stdio.h>
#include <stdlib.h>

#include <ana.h>

COMO_OBJECT_API ana_object *ana_code_new(ana_size_t cap)
{
  ana_code *co = malloc(sizeof(*co));

  ana_container_init((ana_object *)&co->base, sizeof(ana_uint32_t *), cap,
    (void ***)&co->code);

  co->base.type = &ana_code_type;

  return (ana_object *)co;
}

COMO_OBJECT_API ana_object *ana_code_push(ana_object *o, ana_uint32_t val)
{
  ana_code *self = ana_get_code(o);

  ana_container_should_resize(self, self->code);

  return ana_container_push(o, (void *)(unsigned long)val);  
}

COMO_OBJECT_API ana_uint32_t ana_code_get(ana_object *o, ana_size_t idx)
{
  return (ana_uint32_t)(unsigned long)ana_container_get(o, idx);
}

static void code_dtor(ana_object *xself)
{
  free((ana_code *)xself);
}

ana_type ana_code_type = {
  .obj_name    = "code",
  .obj_print   = NULL,
  .obj_dtor    = code_dtor,
  .obj_equals  = NULL,
  .obj_bool    = NULL,
  .obj_hash    = NULL,
  .obj_str     = NULL,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = NULL,
  .obj_seqops  = NULL,
  .obj_get_attr = NULL
};
