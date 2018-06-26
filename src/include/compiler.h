#ifndef COMO_COMPILER_H
#define COMO_COMPILER_H

#include "vm.h"

#define COMO_COMPILE_ERROR_BUFFER 4096

typedef struct ana_compile_state {
  node *ast;
  char *filename;
  int error_line;
  int error_column;
  char *errormsg[COMO_COMPILE_ERROR_BUFFER];
} ana_compile_state;

ana_function *ana_compileast(ana_vm *vm, ana_compile_state *compile_state);


#endif