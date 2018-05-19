#include <stdio.h>
#include <stdlib.h>

#include <ana.h>

COMO_OBJECT_API ana_object *ana_module_new(ana_object *name, ana_object *members)
{
  ana_module *module = malloc(sizeof(*module));

  module->base.type = &ana_module_type;
  module->base.next = NULL;
  module->base.scope = NULL;
  module->base.flags = 1;
  module->name = name;
  module->members = members;

  return (ana_object *)module;
}

static void module_print(ana_object *ob)
{
  ana_module *module = (ana_module *)ob;

  printf("<module %s at %p>", ana_cstring(module->name), (void *)module);
}

static inline void module_dtor(ana_object *ob)
{
  if(ob->scope)
    ana_object_dtor(ob->scope);

  free(ob);
}

static ana_object *module_string(ana_object *obj)
{
  ana_module *module = (ana_module *)obj;
  ana_object *retval;

  ANA_AUTO_RELEASE(ana_build_str("<module %s at %p>", 
    ana_cstring(module->name), (void *)module), {
      retval = ana_stringfromstring(value);
  });

  return retval;
}

ana_type ana_module_type = {
  .obj_name    = "module",
  .obj_print   = module_print,
  .obj_dtor    = module_dtor,
  .obj_equals  = NULL,
  .obj_bool    = NULL,
  .obj_hash    = NULL,
  .obj_str     = module_string,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = NULL,
  .obj_seqops  = NULL
};
