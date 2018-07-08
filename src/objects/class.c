#include <ana.h>

COMO_OBJECT_API ana_object *ana_class_new(ana_object *base, ana_object *name)
{
  ana_class *obj = malloc(sizeof(*obj));

  obj->base.type = &ana_class_type;
  obj->base.next = NULL;  
  obj->base.flags = 0;
  obj->base.refcount = 0;  
  obj->base.is_tracked = 0;
  obj->c_base = base;
  obj->members = ana_map_new(4);
  obj->name = name;

  return (ana_object *)obj;
}

static inline int class_equals(ana_object *a, ana_object *b)
{
  return a == b;
}

static ana_object *class_equals_wrap(ana_object *obj, ana_object *x)
{
  return ana_boolfromint(class_equals(obj, x));
}

static ana_object *class_string(ana_object *obj)
{
  ana_class *self = (ana_class *)obj;

  return ana_stringfromstring(
      ana_build_str("<class '%s' at %p>", ana_cstring(self->name), (void *)self)
  );
}

static void class_print(ana_object *obj)
{
  fprintf(stdout, "<class `%s` at %p>", ana_cstring(ana_get_class(obj)->name),
      (void *)obj);
}

static void class_dtor(ana_object *obj)
{  
  ana_class *theclass = (ana_class *)obj;

  ana_object_dtor(theclass->members);
  
  free(theclass);
}

static int class_bool(ana_object *obj)
{
  COMO_UNUSED(obj);

  return 1;
}

static void class_init(ana_object *obj)
{
  COMO_UNUSED(obj);
}


COMO_OBJECT_API ana_object *ana_instance_new(ana_object *class_defn)
{
  assert(ana_type_is(class_defn, ana_class_type));

  ana_instance *obj = malloc(sizeof(*obj));

  obj->base.type = &ana_instance_type;
  obj->base.next = NULL;  
  obj->base.flags = 0;
  obj->base.refcount = 0;  
  obj->base.is_tracked = 0;

  obj->base_instance = NULL;
  obj->self          = (ana_class *)class_defn;
  obj->properties    = ana_map_new(4);

  return (ana_object *)obj; 
}

static int instance_equals(ana_object *a, ana_object *b)
{
  return a == b;
}

static ana_object *instance_string(ana_object *obj)
{
  ana_instance *inst = (ana_instance *)obj;

  char *str = ana_build_str("<instance of type '%s' at %p>", 
        ana_cstring(inst->self->name), (void *)inst);
  
  ana_object *retval = ana_stringfromstring(str);

  free(str);

  return retval;
}

static void instance_print(ana_object *obj)
{
  ana_instance *inst = (ana_instance *)obj;

  fprintf(stdout, "<instance of type `%s` at %p>", 
    ana_cstring(inst->self->name), (void *)obj);
}

static void instance_dtor(ana_object *obj)
{  
  ana_instance *self = (ana_instance *)obj;

  ana_object_dtor(self->properties);

  if(self->base_instance)
    instance_dtor(self->base_instance);
  
  free(self);
}

static int instance_bool(ana_object *obj)
{
  COMO_UNUSED(obj);

  return 1;
}

static void instance_init(ana_object *obj)
{
  COMO_UNUSED(obj);
}

static ana_comparison_ops compops = {
  .obj_eq  = class_equals_wrap,
  .obj_neq = NULL,
  .obj_gt  = NULL,
  .obj_lt  = NULL,
  .obj_gte = NULL,
  .obj_lte = NULL
};

ana_type ana_class_type = {
  .obj_name    = "class",
  .obj_print   = class_print,
  .obj_dtor    = class_dtor,
  .obj_equals  = class_equals,
  .obj_bool    = class_bool,
  .obj_hash    = NULL,
  .obj_str     = class_string,
  .obj_init    = class_init,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = &compops,
  .obj_seqops  = NULL,
  .obj_get_attr = NULL,
  .obj_props    = NULL,
  .obj_iter     = NULL,
  .obj_iter_next = NULL
};

ana_type ana_instance_type = {
  .obj_name    = "instance",
  .obj_print   = instance_print,
  .obj_dtor    = instance_dtor,
  .obj_equals  = instance_equals,
  .obj_bool    = instance_bool,
  .obj_hash    = NULL,
  .obj_str     = instance_string,
  .obj_init    = instance_init,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = NULL,
  .obj_seqops  = NULL,
  .obj_get_attr = NULL,
  .obj_props    = NULL,
  .obj_iter     = NULL,
  .obj_iter_next = NULL 
};