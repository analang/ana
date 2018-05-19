#ifndef COMO_FUNCTION_H
#define COMO_FUNCTION_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include function.h directly"
#endif

#define COMO_FUNCTION_LANG    0x00000001
#define COMO_FUNCTION_NATIVE  0x00000002

typedef ana_object*(*ana_function_handler)(ana_object *);

typedef struct _ana_function 
{
  ana_object base;
  ana_size_t flags;
  union 
  {
    struct _ana_frame *frame;
    ana_function_handler handler;
  } impl;
} ana_function;

COMO_OBJECT_API ana_object *ana_functionfromframe(ana_frame *frame);
COMO_OBJECT_API ana_object *ana_functionfromhandler(
  ana_function_handler handler);

#define ana_get_function(obj) ((ana_function *)((obj)))

extern ana_type ana_function_type;

#endif