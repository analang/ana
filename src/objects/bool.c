#include <ana.h>

ana_object *ana_bool_true;
ana_object *ana_bool_false;

void ana_bool_type_init()
{
  ana_bool *xbool_true = malloc(sizeof(ana_bool));

  xbool_true->base.type = &ana_bool_type;
  xbool_true->base.next = NULL;
  xbool_true->base.flags = 0;
  xbool_true->base.refcount = 0;
  xbool_true->value = 1;

  ana_bool *xbool_false = malloc(sizeof(ana_bool));
  
  xbool_false->base.type = &ana_bool_type;
  xbool_false->base.next = NULL;
  xbool_false->base.flags = 0;
  xbool_false->base.refcount = 0;
  xbool_false->value = 0;

  ana_bool_true = (ana_object *)xbool_true;
  ana_bool_false = (ana_object *)xbool_false;
}

void ana_bool_type_finalize()
{ 
  free(ana_bool_true);
  free(ana_bool_false);
}

static void bool_print(ana_object *x)
{
  ana_bool *self = ana_get_bool(x);

  fprintf(stdout, "%s", self->value == 1 ? "true" : "false");
}

static void bool_dtor(ana_object *x)
{
  COMO_UNUSED(x);
  /* this a noop, however, since the singelton bools can exist
     in the constant table, this must be implemented 
  */
}

static int bool_equals(ana_object *obj, ana_object *x)
{
  ana_bool *self = ana_get_bool(obj);

  if(!ana_type_is(x, ana_bool_type))
    return 0;
  else
    return self->value == ana_get_bool(x)->value;
}

static ana_object *bool_string(ana_object *obj)
{
  ana_bool *self = ana_get_bool(obj);

  if(self->value == 1)
    return ana_stringfromstring("true");
  else
    return ana_stringfromstring("false");
}

COMO_OBJECT_API ana_object *ana_boolfromanaobject(ana_object *obj)
{
  return obj;
}

static ana_object *bool_equals_wrap(ana_object *obj, ana_object *x)
{
  return ana_boolfromint(bool_equals(obj, x));
}

static ana_object *bool_neq(ana_object *obj, ana_object *x)
{
  ana_bool *self = ana_get_bool(obj);

  if(!ana_type_is(x, ana_bool_type))
    return ana_false;
  else
    return ana_boolfromint(self->value != ana_get_bool(x)->value);
}

static ana_object *bool_gt(ana_object *obj, ana_object *x)
{
  COMO_UNUSED(obj);
  COMO_UNUSED(x);

  return ana_false;
}

static ana_object *bool_lt(ana_object *obj, ana_object *x)
{
  COMO_UNUSED(obj);
  COMO_UNUSED(x);

  return ana_false;
}

static ana_object *bool_gte(ana_object *obj, ana_object *x)
{
  COMO_UNUSED(obj);
  COMO_UNUSED(x);

  return ana_false;
}

static ana_object *bool_lte(ana_object *obj, ana_object *x)
{
  COMO_UNUSED(obj);
  COMO_UNUSED(x);

  return ana_false;
}
  
static int bool_bool(ana_object *x)
{
  return ((ana_bool *)x)->value != 0;
}

static ana_comparison_ops compops = {
  .obj_eq  = bool_equals_wrap,
  .obj_neq = bool_neq,
  .obj_gt  = bool_gt,
  .obj_lt  = bool_lt,
  .obj_gte = bool_gte,
  .obj_lte = bool_lte
};

ana_type ana_bool_type = {
  .obj_name    = "bool",
  .obj_print   = bool_print,
  .obj_dtor    = bool_dtor,
  .obj_equals  = bool_equals,
  .obj_bool    = bool_bool,
  .obj_hash    = NULL,
  .obj_str     = bool_string,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = &compops,
  .obj_seqops  = NULL
};