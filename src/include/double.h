#ifndef COMO_DOUBLE_OBJECT_H
#define COMO_DOUBLE_OBJECT_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include double.h directly"
#endif

typedef struct _ana_double {
  ana_object base;
  double value;
} ana_double;

extern ana_type ana_double_type;

#define ana_get_double(o) ((ana_double *)(o))

COMO_OBJECT_API ana_object *ana_doublefromdouble(double dval);

#endif
