#include <stdlib.h>
#include <string.h>
#include <ana.h>

static char *ana_excep__IMPL = NULL;
static char *ana_except_type__IMPL = NULL;

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

char **ana__excep_location(void)
{
  return &ana_excep__IMPL;
}

char **ana__except_type_location(void)
{
  return &ana_except_type__IMPL;
}