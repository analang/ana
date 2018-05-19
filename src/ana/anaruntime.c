#include <stdlib.h>
#include <string.h>
#include <ana.h>
#include <assert.h>

#include "utils.h"

COMO_OBJECT_API char *ana_get_fn_name(ana_frame * frame)
{
  const char *base = "";
  const char *sep = "";
  const char *fnname = ana_cstring(frame->name);

  if(frame->self)
  {
    base = ana_cstring(ana_get_class(frame->self)->name);
    sep = ".";
  }

  return ana_build_str("%s%s%s", base, sep, fnname);
}

COMO_OBJECT_API char *ana_get_frame_name(ana_object *obj)
{
  assert(obj != NULL);

  if(ana_type_is(obj, ana_frame_type))
  {
    return ana_get_fn_name((ana_frame *)obj);
  } 
  else if(ana_type_is(obj, ana_function_type))
  {
    if(ana_get_function_flags(obj) & COMO_FUNCTION_LANG)
    {
      return ana_get_fn_name(ana_get_function_frame(obj));
    } 
    else
    {
      return ana_build_str("<builtin function>");
    }
  }
  else
  {    
    printf("fatal: can't get frame name for object of type %s\n", 
      ana_type_name(obj));
    fflush(stdout);
    abort();
  }
}