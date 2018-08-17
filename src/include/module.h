#ifndef COMO_MODULE_H
#define COMO_MODULE_H 

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include module.h directly"
#endif

/* TODO not implemented yet */

/* top level exeuction unit */
/* every class, function, and variable is part of a module. These values
 * are located in members */
typedef struct _ana_module 
{
  ana_object  base;

  ana_object *name;
  ana_object *code;
  ana_object *members; /* Modules can have variables, functions, 
                          and classes, all key value pairs (ana_map) */
  ana_object *func;
} ana_module;

COMO_OBJECT_API ana_object *ana_module_new(char *name, ana_object *func);

#define ana_get_module(obj) ((ana_module *)((obj)))

extern ana_type ana_module_type;

#endif