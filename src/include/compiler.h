#ifndef COMO_COMPILER_H
#define COMO_COMPILER_H

#include "vm.h"

ana_object *ana_compileast(char *filename, ComoVM *vm, node *ast);

ana_object *ana_compilemodule(char *modulename, char *filename, 
  ComoVM *vm, node *ast);

#endif