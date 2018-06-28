#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <float.h>

#include <ana.h>

COMO_OBJECT_API ana_object *ana_doublefromdouble(double dval)
{
  ana_double *obj = malloc(sizeof(*obj));

  obj->base.type = &ana_double_type;
  obj->base.next = NULL; 
  obj->base.flags = 0;
  obj->base.refcount = 0;
  obj->value = dval;

  return (ana_object *)obj;
}

static void double_print(ana_object *ob)
{
  ana_double *dval = (ana_double *)ob;

  /* check out DBL_DECIMAL_DIG */
  printf("%.17g",  dval->value);
}

static ana_object *double_string(ana_object *obj)
{
  ana_double *self = ana_get_double(obj);
  char *buffer = NULL;
  int size = 0;
  size =  snprintf(buffer, size, "%.17g", self->value);

  size++;

  buffer = malloc(size);

  snprintf(buffer, size, "%.17g", self->value);

  buffer[size - 1] = '\0';

  ana_object *retval = ana_stringfromstring(buffer);
  
  free(buffer);

  return retval;
}

static void double_dtor(ana_object *ob)
{
  ana_double *dval = (ana_double *)ob;
  free(dval);
}

static int double_equals(ana_object *a, ana_object *b)
{
  int retval = 0;

  if(ana_type_is(a, ana_double_type) 
    && ana_type_is(b, ana_double_type))
  {
    retval = ((ana_double *)a)->value == ((ana_double *)b)->value;
  }

  return retval;
}

static ana_object *double_add(ana_object *xself, ana_object *a)
{
  ana_double *self = (ana_double *)xself;
  ana_object *retval = NULL;
  
  if(ana_type_is(a, ana_double_type))
    retval = ana_doublefromdouble(self->value + 
                  ((ana_double *)a)->value);

  else if(ana_type_is(a, ana_long_type))
      retval = ana_doublefromdouble(self->value + 
                  (double)((ana_long *)a)->value);
  

  return retval; 
}

static ana_object *double_mul(ana_object *obj, ana_object *x)
{  
  ana_double *self   = ana_get_double(obj);
  ana_object *retval = NULL;
  
  if(ana_type_is(x, ana_double_type))
    retval = ana_doublefromdouble(self->value * ((ana_double *)x)->value);
  
  else if(ana_type_is(x, ana_long_type))
    retval = ana_doublefromdouble(self->value * (double)((ana_long *)x)->value);  

  return retval;
}

static ana_object *double_div(ana_object *obj, ana_object *x)
{
  ana_double *self   = ana_get_double(obj);
  ana_object *retval = NULL;
  
  if(ana_type_is(x, ana_double_type))
    retval = ana_doublefromdouble(self->value / ((ana_double *)x)->value);
  
  else if(ana_type_is(x, ana_long_type))
    retval = ana_doublefromdouble(self->value / (double)((ana_long *)x)->value);  

  return retval;
}

static ana_object *double_sub(ana_object *obj, ana_object *x)
{
  ana_double *self   = ana_get_double(obj);
  ana_object *retval = NULL;
  
  if(ana_type_is(x, ana_double_type))
    retval = ana_doublefromdouble(self->value - ((ana_double *)x)->value);
  
  else if(ana_type_is(x, ana_long_type))
    retval = ana_doublefromdouble(self->value - (double)((ana_long *)x)->value);  

  return retval;
}

static ana_object *double_rem(ana_object *obj, ana_object *x)
{
  ana_double *self   = ana_get_double(obj);
  ana_object *retval = NULL;
  
  if(ana_type_is(x, ana_double_type)) 
  {
    double dividend = self->value;
    double divisor = ((ana_double *)x)->value;
    
    retval = ana_doublefromdouble(
      dividend - (divisor * ((int)dividend/(int)divisor))
    );
  } 
  else if(ana_type_is(x, ana_long_type))
  {
    double dividend = self->value;
    double divisor = (double)(((ana_long *)x)->value);

    /* 269.86 % 100 = 269.86 - (100 * int(269.86/100)) */
    /* http://www2.nkfust.edu.tw/~mhchen/papers/Psychologica\
     * l%20Barriers%20Effects%20on%20Futures%20Markets/mod.pdf */
    retval = ana_doublefromdouble(
        dividend - (divisor * ((int)dividend/(int)divisor))
    );
  } 

  return retval;
}

static ana_object *double_plus(ana_object *obj)
{
  ana_double *self = ana_get_double(obj);

  return ana_doublefromdouble(+self->value); 
}

static ana_object *double_minus(ana_object *obj)
{
  ana_double *self = ana_get_double(obj);

  return ana_doublefromdouble(-self->value); 
}

static int double_bool(ana_object *obj)
{
  return ((ana_double *)obj)->value != 0.0;
}

static ana_binary_ops binops = {
  .obj_add = double_add,
  .obj_mul = double_mul,
  .obj_div = double_div,
  .obj_sub = double_sub,
  .obj_rem = double_rem,
  .obj_ls  = NULL,
  .obj_rs  = NULL
};

static ana_unary_ops unops = {
  .obj_plus  = double_plus,
  .obj_minus = double_minus
};

static ana_comparison_ops compops = {
  .obj_eq  = NULL,
  .obj_neq = NULL,
  .obj_gt  = NULL,
  .obj_lt  = NULL,
  .obj_gte = NULL,
  .obj_lte = NULL
};

ana_type ana_double_type = {
  .obj_name    = "double",
  .obj_print   = double_print,
  .obj_dtor    = double_dtor,
  .obj_equals  = double_equals,
  .obj_bool    = double_bool,
  .obj_hash    = NULL,
  .obj_str     = double_string,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = &binops,
  .obj_unops   = &unops,
  .obj_compops = &compops,
  .obj_seqops  = NULL,
  .obj_get_attr = NULL,
  .obj_props    = NULL,
  .obj_iter     = NULL,
  .obj_iter_next = NULL
};
