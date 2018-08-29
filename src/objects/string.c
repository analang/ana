#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include <ana.h>

static ana_object *string_length(ana_object *stringobj, ana_object *arg)
{
  (void)arg;
  
  ana_object *retval = ana_longfromlong(ana_get_string(stringobj)->len);

  return retval;
}

static ana_object *string_getBytes(ana_object *stringobj, ana_object *arg)
{
  COMO_UNUSED(arg);
  
  ana_string *self = ana_get_string(stringobj);

  if(self->len == 0)
  {
    ana_object *retval = ana_array_new(2);

    return retval;
  }

  ana_object *bytes = ana_array_new(self->len);
  ana_size_t i;

  for(i = 0; i < self->len; i++)
  {
    int c = (int)self->value[i];

    ana_object *byte = ana_longfromlong((int)c);

    ana_array_push(bytes, byte);
  }

  return bytes;
}

static ana_object *string_getType(ana_object *stringobj, ana_object *arg)
{
  COMO_UNUSED(stringobj);
  COMO_UNUSED(arg);

  ana_object *res = ana_stringfromstring("string");

  return res;
}


COMO_OBJECT_API void ana_string_type_init(ana_vm *vm)
{
  ana_type *type = &ana_string_type;

  type->obj_props = ana_map_new(4);

  ana_object *func = ana_methodfromhandler("<builtin>", "length", 
    string_length, NULL);

  ana_map_put(type->obj_props, ana_vm_new_symbol(vm, "length"), func);

  func = ana_methodfromhandler("<builtin>", "getBytes", 
    string_getBytes, NULL);

  ana_map_put(type->obj_props, ana_vm_new_symbol(vm, "getBytes"), func);


  ana_map_put(type->obj_props, ana_vm_new_symbol(vm, "getType"), 
    ana_methodfromhandler("<builtin>", "getType", string_getType, NULL)
  );
}

COMO_OBJECT_API void ana_string_type_finalize(ana_vm *vm)
{
  COMO_UNUSED(vm);

  ana_map_foreach(ana_string_type.obj_props, key, value) {
    (void)key;

    ana_object_dtor(value);
  } ana_map_foreach_end();

  ana_object_dtor(ana_string_type.obj_props);
}

static inline unsigned long hash(unsigned char *str)
{
  unsigned long hash = 5381;
  int c;

  while((c = *str++))
    hash = ((hash << 5) + hash) + c;

  return hash;
}

static inline unsigned long hashlen(unsigned char *str, size_t size)
{
  unsigned long hash = 5381;
  int c;
  size_t i = 0;

  while((c = *str++) && i++ < size) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash;
}

static ana_object *ana_stringfromstringandlen(char *val, size_t len)
{
  ana_string *obj = malloc(sizeof(*obj));

  /* TODO check if len overflows */

  obj->base.type = &ana_string_type;
  obj->base.next = NULL;
  obj->base.flags = 0;
  obj->base.refcount = 0;
  obj->base.is_tracked = 0;

  obj->len = (ana_size_t)len;
  
  obj->hash = hashlen((unsigned char*)val, len);
  /* TODO check if hash value would overflow as a signed value */

  obj->value = malloc(len + 1);
  memcpy(obj->value, val, len);
  obj->value[len] = '\0';

  return (ana_object *)obj;
}

COMO_OBJECT_API ana_object *ana_stringfromstring(char *val)
{
  ana_string *obj = malloc(sizeof(*obj));
  size_t len = strlen(val);

  /* TODO check if len overflows */

  obj->base.type = &ana_string_type;
  obj->base.next = NULL;
  obj->base.flags = 0;
  obj->base.refcount = 0;
  obj->base.is_tracked = 0;

  obj->len = (ana_size_t)len;
  
  obj->hash = hash((unsigned char*)val);
  /* TODO check if hash value would overflow as a signed value */

  obj->value = malloc(len + 1);
  memcpy(obj->value, val, len + 1);

  return (ana_object *)obj;
}

static void string_print(ana_object *ob)
{
  ana_string *str = (ana_string *)ob;

  printf("%s", str->value);
}

static void string_dtor(ana_object *ob)
{
  ana_string *str = (ana_string *)ob;

  free(str->value);
  free(str);
}

static int string_equals(ana_object *a, ana_object *b)
{
  int retval = 0;

  if(ana_type_is(a, ana_string_type) &&
    ana_type_is(b, ana_string_type))
  {
    ana_string *left  = (ana_string *)a;
    ana_string *right = (ana_string *)b;

    retval = left->len == right->len 
      && memcmp(left->value, right->value, left->len) == 0;
  }

  return retval;
}

static ana_usize_t string_hash(ana_object *obj)
{
  ana_string *self  = (ana_string *)obj;

  return self->hash;
}

static int string_bool(ana_object *x)
{
  return ((ana_string *)x)->len != 0;
}

static ana_object *string_string(ana_object *x)
{
  return ana_stringfromstring(((ana_string *)x)->value);
}

static ana_object *string_add(ana_object *a, ana_object *b)
{
  ana_string *s1 = ((ana_string *)a);
  ana_string *s2;
  int shouldfrees2 = 0;

  if(!ana_type_is(b, ana_string_type)) 
  {
    s2 = (ana_string *)b->type->obj_str(b);
    shouldfrees2 = 1;
  } 
  else 
  {
    s2 = ((ana_string *)b);
  }

  size_t len = s1->len + s2->len + 1;
  /* TODO, overflow checking */
  char *buf = malloc(len);
  ana_object *ret;

  memcpy(buf, s1->value, s1->len);
  memcpy(buf + s1->len, s2->value, s2->len);

  buf[len-1] = '\0';
  ret = ana_stringfromstring(buf);
  free(buf);

  if(shouldfrees2)
    ana_object_dtor(s2);

  return ret;
}

static ana_object *str_get(ana_object *obj, ana_object *index)
{
  ana_string *self = ana_get_string(obj);
  
  if(ana_type_is(index, ana_long_type))
  {
    long value = ana_get_long(index)->value;

    if(value >= 0 && value < self->len)
      return ana_stringfromstringandlen(self->value + value, 1);
  }
  
  return NULL;
}

static ana_object *string_equals_wrap(ana_object *a, ana_object *b)
{
  return ana_boolfromint(string_equals(a, b));
}

static ana_object *string_not_equals_wrap(ana_object *a, ana_object *b)
{
  return ana_boolfromint(!string_equals(a, b));
}

static ana_object *string_greater_than(ana_object *strobj, ana_object *right)
{
  ana_string *str = ana_get_string(strobj);
  int result;

  if(!ana_type_is(right, ana_string_type))
  {
    Ana_SetError(AnaTypeError, "unsupported right hand operand for string.> operator, got %s",
      ana_type_name(right));

    return NULL;
  }

  result = strcmp(str->value, ana_get_string(right)->value);

  return ana_boolfromint(result > 0 ? 1 : 0);
}

static ana_object *string_less_than(ana_object *strobj, ana_object *right)
{
  ana_string *str = ana_get_string(strobj);
  int result;

  if(!ana_type_is(right, ana_string_type))
  {
    Ana_SetError(AnaTypeError, "unsupported right hand operand for string.< operator, got %s",
      ana_type_name(right));

    return NULL;
  }

  result = strcmp(str->value, ana_get_string(right)->value);

  return ana_boolfromint(result < 0 ? 1 : 0);
}

static ana_object *string_greater_than_or_equal(ana_object *strobj, ana_object *right)
{
  ana_string *str = ana_get_string(strobj);
  int result;

  if(!ana_type_is(right, ana_string_type))
  {
    Ana_SetError(AnaTypeError, "unsupported right hand operand for string.>= operator, got %s",
      ana_type_name(right));

    return NULL;
  }

  result = strcmp(str->value, ana_get_string(right)->value);

  return ana_boolfromint(result >= 0 ? 1 : 0);
}

static ana_object *string_less_than_or_equal(ana_object *strobj, ana_object *right)
{
  ana_string *str = ana_get_string(strobj);
  int result;

  if(!ana_type_is(right, ana_string_type))
  {
    Ana_SetError(AnaTypeError, "unsupported right hand operand for string.<= operator, got %s",
      ana_type_name(right));

    return NULL;
  }

  result = strcmp(str->value, ana_get_string(right)->value);

  return ana_boolfromint(result <= 0 ? 1 : 0);
}

static ana_object *string_iterator_get(ana_object *obj)
{ 
  obj->refcount++;

  ana_string_iterator *iter = malloc(sizeof(*iter));

  iter->base.flags = 0;
  iter->base.refcount = 0;
  iter->base.type = &ana_string_iterator_type;
  iter->base.next = NULL;

  iter->string = (ana_string *)obj;
  iter->position = 0;

  return (ana_object *)iter;
}

static ana_object *ana_string_iterator_next(ana_string_iterator *iter)
{
  if(iter->position == iter->string->len)
    return NULL;

  ana_object *retval = ana_stringfromstringandlen(&iter->string->value[iter->position++], 1);

  GC_TRACK(ana_VM, retval);

  return retval;
}

static void string_iterator_dtor(ana_object *iterobj)
{ 
  ana_string_iterator *iter = (ana_string_iterator *)iterobj;

  ana_get_base(iter->string)->refcount--;

  free(iter);
}

static void string_iterator_print(ana_object *obj)
{
  printf("<string.iterator at %p>", (void *)obj);
}

static ana_object *string_iterator_string(ana_object *obj)
{
  char *val = ana_build_str("<string.iterator at %p>", (void *)obj);

  ana_object *retval = ana_stringfromstring(val);

  free(val);

  return retval;
}

ana_type ana_string_iterator_type = {
  .obj_name    = "string.iterator",
  .obj_print   = string_iterator_print,
  .obj_dtor    = string_iterator_dtor,
  .obj_equals  = NULL,
  .obj_bool    = NULL,
  .obj_hash    = NULL,
  .obj_str     = string_iterator_string,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = NULL,
  .obj_seqops  = NULL,
  .obj_get_attr  = NULL,
  .obj_props     = NULL,
  .obj_iter      = NULL,
  .obj_iter_next = (ana_iterator_function)ana_string_iterator_next
};

static ana_comparison_ops compops = {
  .obj_eq  = string_equals_wrap,
  .obj_neq = string_not_equals_wrap,
  .obj_gt  = string_greater_than,
  .obj_lt  = string_less_than,
  .obj_gte = string_greater_than_or_equal,
  .obj_lte = string_less_than_or_equal
};

static struct _ana_seq_ops seqops = {
  str_get,
  NULL
};

static ana_binary_ops binops = {
  .obj_add = string_add,
  .obj_mul = NULL,
  .obj_div = NULL,
  .obj_sub = NULL,
  .obj_rem = NULL,
  .obj_ls  = NULL,
  .obj_rs  = NULL
};

ana_type ana_string_type = {
  .obj_name     = "string",
  .obj_print    = string_print,
  .obj_dtor     = string_dtor,
  .obj_equals   = string_equals,
  .obj_bool     = string_bool,
  .obj_hash     = string_hash,
  .obj_str      = string_string,
  .obj_init     = NULL,
  .obj_deinit   = NULL,
  .obj_binops   = &binops,
  .obj_unops    = NULL,
  .obj_compops  = &compops,
  .obj_seqops   = &seqops,
  .obj_get_attr = NULL,
  .obj_props    = NULL,
  .obj_iter     = string_iterator_get,
  .obj_iter_next = NULL
};












