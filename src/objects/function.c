#include <ana.h>

static ana_function *create_function(int type)
{
  ana_function *obj = malloc(sizeof(*obj));

  obj->base.type = &ana_function_type;
  obj->base.next = NULL;
  obj->base.scope = NULL;
  obj->base.flags = 0;
  obj->flags = type;

  return obj;
}

COMO_OBJECT_API ana_object *ana_functionfromframe(ana_frame *frame)
{ 
  ana_function *fn = create_function(COMO_FUNCTION_LANG);

  fn->impl.frame = frame;

  return (ana_object *)fn;
}


COMO_OBJECT_API ana_object *ana_functionfromhandler(
  ana_function_handler handler)
{
  ana_function *fn = create_function(COMO_FUNCTION_NATIVE);

  fn->impl.handler = handler;

  return (ana_object *)fn;
}

static void function_print(ana_object *obj)
{
  ana_function *self = ana_get_function(obj); 

  fprintf(stdout, "<function at %p>", (void *)self);
}

static void function_dtor(ana_object *obj)
{
  ana_function *self = ana_get_function(obj); 

  if(obj->scope)
    ana_object_dtor(obj->scope);

#ifdef ANA_GC_DEBUG
  printf("destroying function...");
#endif

  if(self->flags & COMO_FUNCTION_LANG)
  {

#ifdef ANA_GC_DEBUG
    printf("it's a language defined function too\n");
#endif
    
    ana_frame *frame = self->impl.frame;

    ana_object_dtor(frame);
  }
  else
  {
    #ifdef ANA_GC_DEBUG
    printf("which is builtin\n");
    #endif
  }

  free(self);
}

static int function_equals(ana_object *a, ana_object *b)
{
  return a == b;
}

static int function_bool(ana_object *obj)
{
  COMO_UNUSED(obj);

  return 1;
}

static ana_object *function_string(ana_object *obj)
{
  ana_function *self = ana_get_function(obj);
  char *buffer = NULL;
  int size = 0;
  size = snprintf(buffer, size, "<function at %p>", (void *)self);

  size++;

  buffer = malloc(size + 1);

  snprintf(buffer, size, "<function at %p>", (void *)self);

  buffer[size] = '\0';

  ana_object *retval = ana_stringfromstring(buffer);
  
  free(buffer);

  return retval; 
}

static ana_object *function_equals_wrap(ana_object *obj, ana_object *x)
{
  return ana_boolfromint(function_equals(obj, x));
}

static ana_comparison_ops compops = {
  .obj_eq  = function_equals_wrap,
  .obj_neq = NULL,
  .obj_gt  = NULL,
  .obj_lt  = NULL,
  .obj_gte = NULL,
  .obj_lte = NULL
};

ana_type ana_function_type = {
  .obj_name    = "function",
  .obj_print   = function_print,
  .obj_dtor    = function_dtor,
  .obj_equals  = function_equals,
  .obj_bool    = function_bool,
  .obj_hash    = NULL,
  .obj_str     = function_string,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = &compops,
  .obj_seqops  = NULL
};