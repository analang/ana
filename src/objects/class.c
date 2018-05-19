#include <ana.h>

COMO_OBJECT_API ana_object *ana_class_new(ana_object *base, ana_object *name,
  ana_object *members)
{
  ana_class *obj = malloc(sizeof(*obj));

  obj->base.type = &ana_class_type;
  obj->base.next = NULL;  
  obj->base.flags = 1;
  obj->base.scope = NULL;
  
  obj->c_base = base;
  obj->members = members;
  obj->name = name;

  return (ana_object *)obj;
}

static int class_equals(ana_object *a, ana_object *b)
{
  return a == b;
}

static ana_object *class_equals_wrap(ana_object *obj, ana_object *x)
{
  return ana_boolfromint(class_equals(obj, x));
}

static ana_object *class_string(ana_object *obj)
{
  ana_function *self = ana_get_function(obj);
  char *buffer = NULL;
  int size = 0;
  size = snprintf(buffer, size, "<class at %p>", (void *)self);

  size++;

  buffer = malloc(size + 1);

  snprintf(buffer, size, "<class at %p>", (void *)self);

  buffer[size] = '\0';

  ana_object *retval = ana_stringfromstring(buffer);
  
  free(buffer);

  return retval; 
}

static void class_print(ana_object *obj)
{
  fprintf(stdout, "<class `%s` at %p>", ana_cstring(ana_get_class(obj)->name),
    (void *)obj);
}

static void class_dtor(ana_object *obj)
{  

  if(obj->scope)
    ana_object_dtor(obj->scope);
  
  ana_class *theclass = (ana_class *)obj;

 // fprintf(stdout, "class_dtor: dtor for %s\n",
 //   ana_cstring(theclass->name));

  ana_object_dtor(theclass->name);
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
  //fprintf(stdout, "class_init: do it\n");

  ana_class *self = ana_get_class(obj);

  ana_usize_t i;

  for(i = 0; i < ana_get_map(self->members)->capacity; i++)
  {
    ana_map_bucket *bucket = ana_get_map(self->members)->buckets[i];

    if(bucket)
    {
      while(bucket != NULL)
      {
        ana_object *value = bucket->value;
        
        if(ana_type_is(value, ana_function_type))
        {
          ana_function *func = ana_get_function(value);
          func->impl.frame->self = obj;

         // printf("class_init: setting self for %s.%s\n",
         //   ana_cstring(self->name), ana_cstring(func->impl.frame->name));
        }

        bucket = bucket->next;
      }

    }
  }

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
  .obj_seqops  = NULL
};