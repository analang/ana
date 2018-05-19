#ifndef COMO_CLASS_H
#define COMO_CLASS_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include class.h directly"
#endif

typedef struct _ana_class
{
  ana_object base;
  ana_object *c_base;
  ana_object *name;
  ana_object *members;
} ana_class;

typedef struct _ana_instance {
  ana_object base;
  ana_object *self;       /* pointer to the class of this instance */
  ana_object *properties; /* map of instance specific properties */
} ana_instance;

COMO_OBJECT_API ana_object *ana_class_new(ana_object *base, 
  ana_object *name, ana_object *members);

COMO_OBJECT_API ana_object *ana_instance_new(ana_object *base, 
  ana_object *name, ana_object *members);

#define ana_get_class(o) ((ana_class *)(o))
#define ana_get_class_frame(o) (ana_get_frame(ana_get_class((o))))

extern ana_type ana_class_type;

#endif