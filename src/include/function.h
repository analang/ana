#ifndef COMO_FUNCTION_H
#define COMO_FUNCTION_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include function.h directly"
#endif

#define COMO_CODE_SIZE              16
#define COMO_FUNCTION_LANG          (1 << 0)
#define COMO_FUNCTION_NATIVE        (1 << 1)
#define COMO_FUNCTION_METHOD        (1 << 2)
#define COMO_FUNCTION_BOUND         (1 << 3)
#define COMO_FUNCTION_HAS_VARARGS   (1 << 3)
#define COMO_FUNCTION_NATIVE_METHOD \
    (COMO_FUNCTION_NATIVE | COMO_FUNCTION_METHOD)

typedef ana_object*(*ana_function_handler)(ana_object *);
typedef ana_object*(*ana_method_handler)(ana_object *, ana_object *);
typedef struct ana_bounded_function ana_bounded_function;

/* Represents a compile time function definition */
typedef struct _ana_function_def {
  ana_object   base;
  ana_object   *parameters;
  unsigned int *code;
  ana_size_t code_size;
  ana_size_t code_capacity;
  ana_object *line_mapping; /* code to source line mapping */
  ana_object *jump_targets;
} ana_function_defn;

typedef struct _ana_function 
{
  ana_object base;
  int flags;
  ana_object *name;
  ana_object *filename;
  union 
  {
    struct _ana_function_def *func;
    ana_function_handler handler;
    struct {
      ana_object *m_parameters;
      ana_method_handler m_handler;
    } method;
  };
} ana_function;

struct ana_bounded_function {
  ana_object base;
  ana_object *self;
  ana_function *func;
};

COMO_OBJECT_API ana_object *ana_function_defn_new(char *filename, char *name);
COMO_OBJECT_API ana_object *ana_functionfromframe(ana_frame *frame);
COMO_OBJECT_API ana_object *ana_functionfromhandler(
  char *filename, char *name, ana_function_handler handler);

COMO_OBJECT_API ana_object *ana_methodfromhandler(
  char *filename, char *name, ana_method_handler handler, ana_object *parameters);

COMO_OBJECT_API ana_object *ana_bounded_function_new(ana_object *inst, ana_function *func);

#define ana_get_function(obj) ((ana_function *)((obj)))
#define ana_get_function_frame(obj) (((ana_function *)((obj)))->impl.frame)
#define ana_get_function_flags(obj) (ana_get_function((obj))->flags)
#define ana_get_function_defn(obj) (ana_get_function(obj)->func)
#define ana_get_bounded_function(obj) ((ana_bounded_function *)((obj)))

#define ANA_GET_FUNCTION_DEF(obj) (((ana_function *)((obj)))->func)
#define ANA_GET_FUNCTION(obj) ((ana_function *)((obj)))

extern ana_type ana_function_type;
extern ana_type ana_bounded_function_type;

#endif