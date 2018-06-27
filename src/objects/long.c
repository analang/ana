#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>

#include <ana.h>

COMO_OBJECT_API inline ana_object *ana_longfromlong(long lval)
{
  ana_long *obj = malloc(sizeof(*obj));

  obj->base.type = &ana_long_type;
  obj->base.next = NULL;
  obj->base.flags = 0;
  obj->base.refcount = 0;
  obj->value = lval;

  return (ana_object *)obj;
}

static void long_print(ana_object *ob)
{
  ana_long *lval = (ana_long *)ob;

  printf("%ld", lval->value);
}

static inline void long_dtor(ana_object *ob)
{
  ana_long *lobj = (ana_long *)ob;

  free(lobj);
}

static int long_equals(ana_object *base, ana_object *right)
{
  int retval = 0;

  if(ana_type_is(base, ana_long_type) 
    && ana_type_is(right, ana_long_type)) {

    ana_long *a = (ana_long *)base;
    ana_long *b = (ana_long *)right;
    retval = a->value == b->value;
  }

  return retval;
}

static ana_usize_t long_hash(ana_object *obj)
{
  ana_long *lval = (ana_long *)obj;

  return lval->value & 0x7fffffffffffffffL;
}

#define willoverflow(a, b) \
  (((b > 0L) && (a > LONG_MAX - b)) || ((b < 0L) && (a < LONG_MIN - b)))

static ana_object *long_add(ana_object *xself, ana_object *b)
{
  ana_long *self = (ana_long *)xself;
  ana_object *retval = NULL;

  if(ana_type_is(b, ana_long_type))
  {
    long right = ((ana_long *)b)->value;

    if(willoverflow(self->value, right))
    {
      retval = ana_longfromlong(-1L);
    }
    else
    {
      retval = ana_longfromlong(self->value + right);
    }
  }
  else 
  { 
    if(ana_type_is(b, ana_double_type))
    {
      double left = (double)self->value;
      double right = ((ana_double *)b)->value;
    
      retval = ana_doublefromdouble(left + right);
    }
  }

  return retval;
}

static ana_object *long_mul(ana_object *xself, ana_object *b)
{
  ana_long *self = ana_get_long(xself);
  ana_object *retval = NULL;

  if(ana_type_is(b, ana_long_type))
  {
    long right = ((ana_long *)b)->value;

    if(0 /*willoverflow */)
    {
      retval = ana_longfromlong(-1L);
    }
    else
    {
      retval = ana_longfromlong(self->value * right);
    }
  }
  else 
  { 
    if(ana_type_is(b, ana_double_type))
    {
      double left = (double)self->value;
      double right = ((ana_double *)b)->value;
    
      retval = ana_doublefromdouble(left * right);
    }
  }

  return retval;
}

static ana_object *long_div(ana_object *xself, ana_object *b)
{
  ana_long *self = ana_get_long(xself);
  ana_object *retval = NULL;

  if(ana_type_is(b, ana_long_type))
  {
    long right = ((ana_long *)b)->value;

    if(0 /*willoverflow */)
    {
      retval = ana_longfromlong(-1L);
    }
    else
    {
      /* TODO divide by zero and overflow */
      if(right == 0)
      {
        return NULL;
      }

      retval = ana_longfromlong(self->value / right);
    }
  }
  else 
  { 
    if(ana_type_is(b, ana_double_type))
    {
      double left = (double)self->value;
      double right = ((ana_double *)b)->value;
    
      retval = ana_doublefromdouble(left / right);
    }
  }

  return retval;
}

static ana_object *long_sub(ana_object *xself, ana_object *b)
{
  ana_long *self = ana_get_long(xself);
  ana_object *retval = NULL;

  if(ana_type_is(b, ana_long_type))
  {
    long right = ((ana_long *)b)->value;

    if(0 /*willoverflow */)
    {
      retval = ana_longfromlong(-1L);
    }
    else
    {
      /* TODO divide by zero and overflow */
      retval = ana_longfromlong(self->value - right);
    }
  }
  else 
  { 
    if(ana_type_is(b, ana_double_type))
    {
      double left = (double)self->value;
      double right = ((ana_double *)b)->value;
    
      retval = ana_doublefromdouble(left - right);
    }
  }

  return retval;
}

static ana_object *long_rem(ana_object *xself, ana_object *right)
{
  ana_long *self = ana_get_long(xself);
  ana_object *retval = NULL;
  
  if(ana_type_is(right, ana_double_type)) 
  {
    double dividend = (double)self->value;
    double divisor = ((ana_double *)right)->value;
    
    /* http://www2.nkfust.edu.tw/~mhchen/papers/Psychologica\
     * l%20Barriers%20Effects%20on%20Futures%20Markets/mod.pdf
     */
    retval = ana_doublefromdouble(
      dividend - (divisor * ((int)dividend/(int)divisor))
    );
  } 
  else if(ana_type_is(right, ana_long_type))
  {
    retval = ana_longfromlong(self->value % ((ana_long *)right)->value);
  } 

  return retval;
}

static ana_object *long_plus(ana_object *obj)
{
  ana_long *self = ana_get_long(obj);

  return ana_longfromlong(+self->value); 
}

static ana_object *long_minus(ana_object *obj)
{
  ana_long *self = ana_get_long(obj);

  return ana_longfromlong(-self->value); 
}

static ana_object *long_string(ana_object *obj)
{
  ana_long *self = ana_get_long(obj);
  
  ana_object *retval;

  ANA_AUTO_RELEASE(ana_build_str("%ld", self->value), {
    retval = ana_stringfromstring(value);
  });

  return retval;
}

static ana_object *long_equals_wrap(ana_object *a, ana_object *b)
{
  return ana_boolfromint(long_equals(a, b));
}

static ana_object *long_neq(ana_object *a, ana_object *b)
{
  return ana_boolfromint(!long_equals(a, b));
}

static ana_object *long_gt(ana_object *a, ana_object *b)
{
  ana_long *self = ana_get_long(a);

  if(!ana_type_is(b, ana_long_type)) 
  {
    Ana_SetError(AnaTypeError, "Unsupported operands for >");
    
    return NULL;
  }
  else
    return ana_boolfromint(self->value > ((ana_long *)b)->value);
}

static ana_object *long_lt(ana_object *a, ana_object *b)
{
  ana_long *self = ana_get_long(a);

  if(!ana_type_is(b, ana_long_type)) 
  {
    Ana_SetError(AnaTypeError, 
      "Unsupported right hand operand for < operator, got type %s", ana_type_name(b));

    return NULL;
  }
  else
    return (self->value < ((ana_long *)b)->value) ? 
      ana_bool_true : ana_bool_false;
}

static ana_object *long_gte(ana_object *a, ana_object *b)
{
  ana_long *self = ana_get_long(a);

  if(!ana_type_is(b, ana_long_type)) 
  {
    Ana_SetError(AnaTypeError, "Unsupported operands for >=");

    return NULL;
  }
  else
  {
    ana_long *right = (ana_long *)b;

    if(self->value > right->value)
      return ana_true;
    else if(self->value == right->value)
      return ana_true;
    else
      return ana_false;
  }
}

static ana_object *long_lte(ana_object *a, ana_object *b)
{
  ana_long *self = ana_get_long(a);
  long rightvalue = 0;

  if(!ana_type_is(b, ana_long_type)) 
  {
    if(ana_type_is(b, ana_double_type)) 
    {
      rightvalue = (long)(ana_get_double(b)->value);
    }
    else
    {
      Ana_SetError(AnaTypeError, 
        "Unsupported operands for <=");

      return NULL;
    }
  }
  else
  {
    rightvalue = ana_get_long(b)->value;
  }

  if(self->value < rightvalue)
    return ana_bool_true;
  else if(self->value == rightvalue)
    return ana_bool_true;
  else
    return ana_bool_false;
}

static int long_bool(ana_object *obj)
{
  ana_long *self = ana_get_long(obj);
  
  return self->value != 0L;
}

static ana_object *long_ls(ana_object *a, ana_object *b)
{
  ana_long *self = ana_get_long(a);
  ana_long *right;

  if(!ana_type_is(b, ana_long_type)) 
  {
    Ana_SetError(AnaTypeError, "invalid type for right hand (<<)");
    
    return NULL;
  }

  right = ana_get_long(b);

  long value = self->value << right->value;

  return ana_longfromlong(value);
}

static ana_object *long_rs(ana_object *a, ana_object *b)
{
  ana_long *self = ana_get_long(a);
  ana_long *right;

  if(!ana_type_is(b, ana_long_type)) 
  {
    Ana_SetError(AnaTypeError, "invalid type for right hand operand (>>)");
  
    return NULL;
  }

  right = ana_get_long(b);

  long value = self->value >> right->value;

  return ana_longfromlong(value);
}


static ana_binary_ops binops = {
  .obj_add = long_add,
  .obj_mul = long_mul,
  .obj_div = long_div,
  .obj_sub = long_sub,
  .obj_rem = long_rem,
  .obj_ls  = long_ls,
  .obj_rs  = long_rs
};

static ana_unary_ops unops = { 
  .obj_plus  = long_plus,
  .obj_minus = long_minus
};

static ana_comparison_ops compops = {
  .obj_eq  = long_equals_wrap,
  .obj_neq = long_neq,
  .obj_gt  = long_gt,
  .obj_lt  = long_lt,
  .obj_gte = long_gte,
  .obj_lte = long_lte
};

ana_type ana_long_type = {
  .obj_name    = "long",
  .obj_print   = long_print,
  .obj_dtor    = long_dtor, 
  .obj_equals  = long_equals,
  .obj_bool    = long_bool,
  .obj_hash    = long_hash,
  .obj_str     = long_string,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = &binops,
  .obj_unops   = &unops,
  .obj_compops = &compops,
  .obj_seqops  = NULL,
  .obj_get_attr = NULL,
  .obj_props    = NULL
};

