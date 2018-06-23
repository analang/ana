#ifndef COMO_FUNCTION_H
#define COMO_FUNCTION_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include function.h directly"
#endif

#define COMO_FUNCTION_LANG    (1 << 0)
#define COMO_FUNCTION_NATIVE  (1 << 1)
#define COMO_FUNCTION_METHOD  (1 << 2)

typedef ana_object*(*ana_function_handler)(ana_object *);
typedef ana_object*(*ana_method_handler)(ana_object *, ana_object *);

/* Represents a compile time function definition */
typedef struct _ana_function_def {
  ana_object  base;
  ana_object *parameters;
  ana_object *code;
  ana_object *line_mapping; /* code to source line mapping */
  ana_object *jump_targets;
} ana_function_defn;

typedef struct _ana_function 
{
  ana_object base;
  ana_size_t flags;
  ana_object *name;
  ana_object *filename;
  union 
  {
    struct _ana_function_def *func;
    ana_function_handler handler;
    ana_method_handler m_handler;
  };
} ana_function;


COMO_OBJECT_API ana_object *ana_function_defn_new(char *filename, char *name);
COMO_OBJECT_API ana_object *ana_functionfromframe(ana_frame *frame);
COMO_OBJECT_API ana_object *ana_functionfromhandler(
  char *filename, char *name, ana_function_handler handler);

COMO_OBJECT_API ana_object *ana_methodfromhandler(
  char *filename, char *name, ana_method_handler handler);

#define ana_get_function(obj) ((ana_function *)((obj)))
#define ana_get_function_frame(obj) (((ana_function *)((obj)))->impl.frame)
#define ana_get_function_flags(obj) (ana_get_function((obj))->flags)

#define ANA_GET_FUNCTION_DEF(obj) (((ana_function *)((obj)))->func)
#define ANA_GET_FUNCTION(obj) ((ana_function *)((obj)))

extern ana_type ana_function_type;

#endif